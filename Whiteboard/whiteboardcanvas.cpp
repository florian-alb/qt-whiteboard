#include "whiteboardcanvas.h"
#include <QJsonObject>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>

WhiteboardCanvas::WhiteboardCanvas(QWidget *parent)
    : QWidget(parent), m_pixmap(size()) {
  m_pixmap.fill(Qt::white);

  // Créer le tracker de curseurs
  m_cursorTracker = new CursorTracker(this);

  // Connecter le tracker au système d'envoi de messages
  connect(m_cursorTracker, &CursorTracker::sendJson, this,
          &WhiteboardCanvas::sendJson);

  // Activer le tracking de la souris même sans clic
  setMouseTracking(true);
}

void WhiteboardCanvas::setTool(Tool *tool) {
  if (m_tool)
    disconnect(m_tool, nullptr, this, nullptr);
  m_tool = tool;
  connect(m_tool, &Tool::sendJson, this, &WhiteboardCanvas::sendJson);
}

void WhiteboardCanvas::applyRemote(const QJsonObject &msg) {
  // Vérifier si c'est un message de curseur ou de dessin
  if (msg.contains("type")) {
    // Message de curseur
    m_cursorTracker->onRemoteCursorUpdate(msg);
  } else {
    // Message de dessin
    if (!m_tool)
      return;
    // on appelle la slot onRemoteJson sur l'outil
    QMetaObject::invokeMethod(m_tool, "onRemoteJson", Q_ARG(QJsonObject, msg));
  }
  update();
}

void WhiteboardCanvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.drawPixmap(0, 0, m_pixmap);
  if (m_tool)
    m_tool->draw(p);

  // Dessiner les curseurs des autres utilisateurs
  m_cursorTracker->drawCursors(p);
}

void WhiteboardCanvas::mousePressEvent(QMouseEvent *e) {
  // Envoyer la position du curseur
  m_cursorTracker->sendMousePress(e->pos());

  if (m_tool) {
    m_tool->onMousePress(e->pos());
    update();
  }
}

void WhiteboardCanvas::mouseMoveEvent(QMouseEvent *e) {
  // Toujours envoyer la position du curseur, même sans dessiner
  bool isDrawing = (m_tool != nullptr);
  m_cursorTracker->sendMousePosition(e->pos(), isDrawing);

  if (m_tool) {
    m_tool->onMouseMove(e->pos());
    update();
  }
}

void WhiteboardCanvas::mouseReleaseEvent(QMouseEvent *e) {
  // Envoyer la position finale du curseur
  m_cursorTracker->sendMouseRelease(e->pos());

  if (m_tool) {
    m_tool->onMouseRelease(e->pos());
    update();
  }
}
