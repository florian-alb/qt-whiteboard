#include "erasertool.h"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "penciltool.h"
#include "whiteboardcanvas.h"

EraserTool::EraserTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId) {
  // Initialize with default eraser size
  m_eraserSize = 30;
}

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
  
  // Find and remove strokes at this point
  eraseAtPoint(pt);
  
  // Notify other clients about the erasure
  emitJson("erase_start", pt);
}

void EraserTool::onMouseMove(const QPoint &pt) {
  if (m_isErasing) {
    m_current.append(pt);
    
    // Find and remove strokes at this point
    eraseAtPoint(pt);
    
    // Notify other clients about the erasure
    emitJson("erase_move", pt);
  }
}

void EraserTool::onMouseRelease(const QPoint &pt) {
  if (m_isErasing) {
    m_current.append(pt);
    
    // Find and remove strokes at this point
    eraseAtPoint(pt);
    
    // Send the complete erase path to other clients
    emitStrokeJson(m_current);
    
    m_isErasing = false;
    m_current.clear();
  }
}

void EraserTool::draw(QPainter &p) {
  // Only draw the current eraser position if we're actively erasing
  if (m_isErasing && !m_current.isEmpty()) {
    // Draw a circle to represent the eraser cursor
    p.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    p.setBrush(Qt::NoBrush);
    QPoint lastPoint = m_current.last();
    p.drawEllipse(lastPoint, m_eraserSize/2, m_eraserSize/2);
  }
}

void EraserTool::onRemoteJson(const QJsonObject &msg) {
  QJsonObject props;
  if (msg.contains("props")) {
    props = msg["props"].toObject();
  } else {
    return;
  }
  
  QString action = props["action"].toString();
  QString toolType = props["tool"].toString();
  
  if (toolType != "eraser" || 
      (action != "erase_start" && 
       action != "erase_move" && 
       action != "erase_stroke")) {
    return;
  }
  
  if (action == "erase_start" || action == "erase_move") {
    QJsonArray coords = props["coordinates"].toArray();
    QPoint pt(coords[0].toInt(), coords[1].toInt());
    
    processRemoteErase(pt);
  } else if (action == "erase_stroke") {
    QJsonArray pointsArray = props["points"].toArray();
    
    for (const QJsonValue &coordValue : pointsArray) {
      QJsonArray coords = coordValue.toArray();
      QPoint pt(coords[0].toInt(), coords[1].toInt());
      processRemoteErase(pt);
    }
  }
}

void EraserTool::clear() {
  m_current.clear();
  m_isErasing = false;
  m_erasedStrokes.clear();
}

void EraserTool::eraseAtPoint(const QPoint &pt) {
  qDebug() << "EraserTool: Erasing at point" << pt;
  
  if (m_canvas) {
    qDebug() << "EraserTool: Using direct canvas reference";
    
    const QVector<Tool*>& allTools = m_canvas->getAllTools();
    qDebug() << "EraserTool: Number of tools:" << allTools.size();
    
    for (Tool* tool : allTools) {
      if (tool == this) {
        qDebug() << "EraserTool: Skipping eraser tool";
        continue;
      }
      
      qDebug() << "EraserTool: Checking tool" << tool->metaObject()->className();
      
      // Call eraseNear on all tools, not just PencilTool
      tool->eraseNear(pt, m_eraserSize/2);
      qDebug() << "EraserTool: Called eraseNear on tool" << tool->metaObject()->className();
    }
  } else {
    // Fallback to the previous approach if direct reference is not set
    qDebug() << "EraserTool: No direct canvas reference, trying to find canvas";
    
    // Try to get the canvas through parent hierarchy
    WhiteboardCanvas* canvas = qobject_cast<WhiteboardCanvas*>(parent());
    if (!canvas) {
      QObject* parentObj = parent();
      while (parentObj && !canvas) {
        canvas = qobject_cast<WhiteboardCanvas*>(parentObj);
        parentObj = parentObj->parent();
      }
    }
    
    if (canvas) {
      // Store the canvas for future use
      m_canvas = canvas;
      qDebug() << "EraserTool: Found and stored canvas reference";
      
      // Now use the canvas to erase
      eraseAtPoint(pt); // Call again with the canvas set
      return;
    } else {
      qDebug() << "EraserTool: Could not find canvas";
    }
  }
  
  // Record this erasure for tracking
  m_erasedStrokes.append(pt);
}

void EraserTool::processRemoteErase(const QPoint &pt) {
  // Similar to eraseAtPoint but for remote erasures
  eraseAtPoint(pt); // We can reuse the same logic
}
