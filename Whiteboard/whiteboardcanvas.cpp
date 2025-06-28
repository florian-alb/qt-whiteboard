#include "whiteboardcanvas.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
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

void WhiteboardCanvas::addTool(Tool *tool) {
  if (tool && !m_tools.contains(tool)) {
    m_tools.append(tool);
    connect(tool, &Tool::sendJson, this, &WhiteboardCanvas::sendJson);

    // Si c'est le premier outil, le définir comme actif
    if (!m_activeTool) {
      m_activeTool = tool;
    }
  }
}

void WhiteboardCanvas::setActiveTool(Tool *tool) {
  if (tool && m_tools.contains(tool)) {
    m_activeTool = tool;
  }
}

void WhiteboardCanvas::saveDrawingsToFile(const QString &filename) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Impossible d'ouvrir le fichier pour écriture:" << filename;
    return;
  }

  QJsonObject root;
  root["version"] = "1.0";
  root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
  root["drawings"] = m_drawingLog;

  QJsonDocument doc(root);
  file.write(doc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "Dessins sauvegardés dans:" << filename;
}

void WhiteboardCanvas::loadDrawingsFromFile(const QString &filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Impossible d'ouvrir le fichier pour lecture:" << filename;
    return;
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  if (doc.isNull()) {
    qDebug() << "Fichier JSON invalide:" << filename;
    return;
  }

  QJsonObject root = doc.object();
  if (!root.contains("drawings")) {
    qDebug() << "Fichier ne contient pas de section 'drawings'";
    return;
  }

  // Vider le log actuel et tous les outils
  m_drawingLog = QJsonArray();
  for (Tool *tool : m_tools) {
    if (tool) {
      tool->clear();
    }
  }

  // Charger tous les dessins de l'historique
  QJsonArray drawings = root["drawings"].toArray();
  for (const QJsonValue &value : drawings) {
    if (value.isObject()) {
      QJsonObject drawing = value.toObject();
      // Appliquer le dessin à tous les outils
      for (Tool *tool : m_tools) {
        if (tool) {
          tool->onRemoteJson(drawing);
        }
      }
      // Ajouter au log
      m_drawingLog.append(drawing);
    }
  }

  update();
  qDebug() << "Dessins chargés depuis:" << filename;
}

void WhiteboardCanvas::clearAllDrawings() {
  m_drawingLog = QJsonArray();
  // Vider tous les outils
  for (Tool *tool : m_tools) {
    if (tool) {
      tool->clear();
    }
  }
  update();
}

void WhiteboardCanvas::applyRemote(const QJsonObject &msg) {
  // Vérifier si c'est un message de curseur ou de dessin

  if (msg.contains("type")) {
    QString type = msg["type"].toString();

    qDebug() << "Type de message reçu:" << type;

    if (type == "object") {
      qDebug() << "Message d'objet reçu:" << msg;
      for (Tool *tool : m_tools) {
        if (tool) {
          tool->onRemoteJson(msg);
        }
      }
      m_drawingLog.append(msg);
    } else if (type == "history") {
      // Message d'historique du serveur
      qDebug() << "Réception de l'historique des dessins";

      if (msg.contains("drawings")) {
        QJsonArray drawings = msg["drawings"].toArray();
        qDebug() << "Chargement de" << drawings.size()
                 << "dessins depuis l'historique";

        // Vider le log actuel et tous les outils
        m_drawingLog = QJsonArray();
        for (Tool *tool : m_tools) {
          if (tool) {
            tool->clear();
          }
        }

        // Charger tous les dessins de l'historique
        for (const QJsonValue &value : drawings) {
          if (value.isObject()) {
            QJsonObject drawing = value.toObject();
            // Appliquer le dessin à tous les outils
            for (Tool *tool : m_tools) {
              if (tool) {
                tool->onRemoteJson(drawing);
              }
            }
            // Ajouter au log
            m_drawingLog.append(drawing);
          }
        }

        update();
        qDebug() << "Historique chargé avec succès";
      }
    } else {
      // Message de curseur
      m_cursorTracker->onRemoteCursorUpdate(msg);
    }
  } else {
    // Message de dessin - l'envoyer à tous les outils
    for (Tool *tool : m_tools) {
      if (tool) {
        tool->onRemoteJson(msg);
      }
    }

    // Ajouter au log local
    m_drawingLog.append(msg);
  }
  update();
}

void WhiteboardCanvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.drawPixmap(0, 0, m_pixmap);

  // Dessiner tous les outils
  for (Tool *tool : m_tools) {
    if (tool) {
      tool->draw(p);
    }
  }

  // Dessiner les curseurs des autres utilisateurs
  m_cursorTracker->drawCursors(p);
}

void WhiteboardCanvas::mousePressEvent(QMouseEvent *e) {
  // Envoyer la position du curseur
  m_cursorTracker->sendMousePress(e->pos());

  if (m_activeTool) {
    m_activeTool->onMousePress(e->pos());
    update();
  }
}

void WhiteboardCanvas::mouseMoveEvent(QMouseEvent *e) {
  // Toujours envoyer la position du curseur, même sans dessiner
  bool isDrawing = (m_activeTool != nullptr);
  m_cursorTracker->sendMousePosition(e->pos(), isDrawing);

  if (m_activeTool) {
    m_activeTool->onMouseMove(e->pos());
    update();
  }
}

void WhiteboardCanvas::mouseReleaseEvent(QMouseEvent *e) {
  // Envoyer la position finale du curseur
  m_cursorTracker->sendMouseRelease(e->pos());

  if (m_activeTool) {
    m_activeTool->onMouseRelease(e->pos());
    update();
  }
}
