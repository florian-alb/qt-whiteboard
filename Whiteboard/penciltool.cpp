#include "penciltool.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>

PencilTool::PencilTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId) {}

void PencilTool::emitJson(const QString &action, const QPoint &pt) {
  QJsonObject msg;
  msg["id"] = m_userId;
  QJsonObject props;
  props["action"] = action;
  QJsonArray coords;
  coords.append(pt.x());
  coords.append(pt.y());
  props["coordinates"] = coords;
  msg["props"] = props;
  emit sendJson(msg);
}

void PencilTool::emitStrokeJson(const QVector<QPoint> &points) {
  if (points.isEmpty())
    return;

  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object";
  QJsonObject props;
  props["action"] = "draw_stroke";

  QJsonArray pointsArray;
  for (const QPoint &pt : points) {
    QJsonArray coords;
    coords.append(pt.x());
    coords.append(pt.y());
    pointsArray.append(coords);
  }
  props["points"] = pointsArray;
  msg["props"] = props;

  emit sendJson(msg);
}

void PencilTool::onMousePress(const QPoint &pt) {
  m_current.clear();
  m_current.append(pt);
  m_isDrawing = true; // On commence à dessiner
}

void PencilTool::onMouseMove(const QPoint &pt) {
  if (m_isDrawing) {
    // Ajouter au trait seulement si on dessine
    m_current.append(pt);
  }
}

void PencilTool::onMouseRelease(const QPoint &pt) {
  if (m_isDrawing) {
    m_current.append(pt);

    // Envoyer le trait complet seulement au relâchement
    if (m_current.size() > 1) {
      emitStrokeJson(m_current);
    }

    m_strokes.append({m_userId, m_current});
    m_current.clear();
    m_isDrawing = false; // On arrête de dessiner
  }
}

void PencilTool::draw(QPainter &p) {
  p.setPen(Qt::black);
  // traits validés
  for (auto &s : m_strokes) {
    if (s.points.size() > 1) {
      for (int i = 1; i < s.points.size(); ++i) {
        p.drawLine(s.points[i - 1], s.points[i]);
      }
    }
  }
  // trait en cours seulement si on dessine
  if (m_isDrawing && m_current.size() > 1) {
    for (int i = 1; i < m_current.size(); ++i) {
      p.drawLine(m_current[i - 1], m_current[i]);
    }
  }
}

void PencilTool::onRemoteJson(const QJsonObject &msg) {
  if (!msg.contains("props"))
    return;

  auto props = msg["props"].toObject();
  if (!props.contains("action"))
    return;

  QString action = props["action"].toString();
  
  // Ignore eraser messages
  if (props.contains("tool") && props["tool"].toString() == "eraser") {
    return;
  }
  if (action.startsWith("erase_")) {
    return;
  }

  if (action == "draw_stroke") {
    // Nouveau format : réception d'un trait complet
    if (!props.contains("points"))
      return;

    QJsonArray pointsArray = props["points"].toArray();
    QVector<QPoint> points;

    for (const QJsonValue &pointValue : pointsArray) {
      if (!pointValue.isArray())
        continue;

      QJsonArray coords = pointValue.toArray();
      if (coords.size() >= 2) {
        points.append(QPoint(coords[0].toInt(), coords[1].toInt()));
      }
    }

    if (!points.isEmpty()) {
      QString userId = msg["id"].toString();
      if (!userId.isEmpty()) {
        m_strokes.append({userId, points});
      }
    }
  } else {
    // Ancien format pour compatibilité (si nécessaire)
    if (!props.contains("coordinates"))
      return;

    QJsonArray arr = props["coordinates"].toArray();
    if (arr.size() < 2)
      return;

    QPoint pt(arr[0].toInt(), arr[1].toInt());
    QString userId = msg["id"].toString();

    // find or create stroke for this user
    if (action == "draw_start") {
      if (!userId.isEmpty()) {
        m_strokes.append({userId, {pt}});
      }
    } else if (!m_strokes.isEmpty()) {
      m_strokes.last().points.append(pt);
    }
  }
}

void PencilTool::clear() {
  m_strokes.clear();
  m_current.clear();
  m_isDrawing = false;
}
