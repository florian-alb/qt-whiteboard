#include "erasertool.h"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>

EraserTool::EraserTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId) {}

// Static method to check if a message is from an eraser tool
bool EraserTool::isEraserMessage(const QJsonObject &msg) {
  if (msg.contains("props")) {
    QJsonObject props = msg["props"].toObject();
    // Check if it's explicitly marked as an eraser tool
    if (props.contains("tool") && props["tool"].toString() == "eraser") {
      return true;
    }
    // Check if it's an eraser action
    if (props.contains("action") && 
        (props["action"].toString() == "erase_start" ||
         props["action"].toString() == "erase_move" ||
         props["action"].toString() == "erase_stroke")) {
      return true;
    }
  }
  return false;
}

void EraserTool::emitJson(const QString &action, const QPoint &pt) {
  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object"; // Add type to identify as an object message
  QJsonObject props;
  props["action"] = action;
  props["tool"] = "eraser"; // Explicitly identify this as an eraser action
  props["eraserSize"] = m_eraserSize;
  QJsonArray coords;
  coords.append(pt.x());
  coords.append(pt.y());
  props["coordinates"] = coords;
  msg["props"] = props;
  emit sendJson(msg);
}

void EraserTool::emitStrokeJson(const QVector<QPoint> &points) {
  if (points.isEmpty())
    return;

  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object";
  QJsonObject props;
  props["action"] = "erase_stroke";
  props["tool"] = "eraser"; // Explicitly identify this as an eraser action

  QJsonArray pointsArray;
  for (const QPoint &pt : points) {
    QJsonArray coords;
    coords.append(pt.x());
    coords.append(pt.y());
    pointsArray.append(coords);
  }
  props["points"] = pointsArray;
  props["eraserSize"] = m_eraserSize;
  msg["props"] = props;

  emit sendJson(msg);
}

void EraserTool::onMousePress(const QPoint &pt) {
  m_current.clear();
  m_current.append(pt);
  m_isErasing = true;
  Stroke stroke;
  stroke.id = m_userId + QString::number(QDateTime::currentMSecsSinceEpoch());
  stroke.points.append(pt);
  m_strokes.append(stroke);
  
  emitJson("erase_start", pt);
}

void EraserTool::onMouseMove(const QPoint &pt) {
  if (m_isErasing) {
    m_current.append(pt);
    emitJson("erase_move", pt);
  }
}

void EraserTool::onMouseRelease(const QPoint &pt) {
  if (m_isErasing) {
    m_current.append(pt);
    m_isErasing = false;

    // We already created the stroke in onMousePress, just update the last stroke with current points
    if (!m_strokes.isEmpty()) {
      // Add all points from current to the last stroke
      for (int i = 1; i < m_current.size(); i++) { // Start from 1 to avoid duplicating the first point
        m_strokes.last().points.append(m_current[i]);
      }
    }

    // Send the complete stroke to other clients
    emitStrokeJson(m_current);
    m_current.clear();
  }
}

void EraserTool::draw(QPainter &p) {
  // Set up the eraser with white color and larger width
  QPen eraserPen(Qt::white, m_eraserSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  p.setPen(eraserPen);
  
  // Draw all completed strokes
  for (const Stroke &stroke : m_strokes) {
    for (int i = 0; i < stroke.points.size() - 1; ++i) {
      p.drawLine(stroke.points.at(i), stroke.points.at(i + 1));
    }
  }
  
  // Draw the current stroke if we're erasing
  if (m_isErasing && m_current.size() > 1) {
    for (int i = 0; i < m_current.size() - 1; ++i) {
      p.drawLine(m_current.at(i), m_current.at(i + 1));
    }
  }
}

void EraserTool::onRemoteJson(const QJsonObject &msg) {
  QJsonObject props;
  if (msg.contains("props")) {
    props = msg["props"].toObject();
  } else {
    return;
  }
  
  // Only process messages specifically for the eraser tool
  QString action = props["action"].toString();
  QString toolType = props["tool"].toString();
  
  // Check if this message is specifically for the eraser tool
  if (toolType != "eraser" && 
      action != "erase_start" && 
      action != "erase_move" && 
      action != "erase_stroke") {
    return; // Skip messages not meant for the eraser
  }
  
  if (action == "erase_start") {
    QJsonArray coords = props["coordinates"].toArray();
    QPoint pt(coords[0].toInt(), coords[1].toInt());
    Stroke stroke;
    stroke.id = msg["id"].toString() + QString::number(QDateTime::currentMSecsSinceEpoch());
    stroke.points.append(pt);
    m_strokes.append(stroke);
  } else if (action == "erase_move") {
    QJsonArray coords = props["coordinates"].toArray();
    QPoint pt(coords[0].toInt(), coords[1].toInt());
    if (!m_strokes.isEmpty()) {
      m_strokes.last().points.append(pt);
    }
  } else if (action == "erase_stroke") {
    QJsonArray pointsArray = props["points"].toArray();
    QVector<QPoint> points;
    for (const QJsonValue &coordValue : pointsArray) {
      QJsonArray coords = coordValue.toArray();
      points.append(QPoint(coords[0].toInt(), coords[1].toInt()));
    }
    Stroke stroke;
    stroke.id = msg["id"].toString() + QString::number(QDateTime::currentMSecsSinceEpoch());
    stroke.points = points;
    m_strokes.append(stroke);
  }
}

void EraserTool::clear() {
  m_strokes.clear();
  m_current.clear();
  m_isErasing = false;
}
