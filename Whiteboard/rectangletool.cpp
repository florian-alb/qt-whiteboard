#include "rectangletool.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>

RectangleTool::RectangleTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId) {}

void RectangleTool::emitJson(const QString &action, const QPoint &pt) {
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

void RectangleTool::emitRectangleJson(const QRect &rect) {
  if (rect.width() <= 0 || rect.height() <= 0)
    return;

  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object";
  QJsonObject props;
  props["action"] = "draw_rectangle";

  QJsonObject rectObj;
  rectObj["x"] = rect.x();
  rectObj["y"] = rect.y();
  rectObj["width"] = rect.width();
  rectObj["height"] = rect.height();
  props["rectangle"] = rectObj;
  msg["props"] = props;

  emit sendJson(msg);
}

void RectangleTool::onMousePress(const QPoint &pt) {
  m_startPoint = pt;
  m_currentPoint = pt;
  m_isDrawing = true;
}

void RectangleTool::onMouseMove(const QPoint &pt) {
  if (m_isDrawing) {
    m_currentPoint = pt;
  }
}

void RectangleTool::onMouseRelease(const QPoint &pt) {
  if (m_isDrawing) {
    m_currentPoint = pt;

    // Créer le rectangle à partir des points de début et de fin
    QRect rect = QRect(m_startPoint, m_currentPoint).normalized();

    // Envoyer le rectangle complet seulement au relâchement
    if (rect.width() > 0 && rect.height() > 0) {
      emitRectangleJson(rect);
    }

    m_rectangles.append({m_userId, rect});
    m_isDrawing = false;
  }
}

void RectangleTool::draw(QPainter &p) {
  p.setPen(Qt::black);
  p.setBrush(Qt::NoBrush);

  // Rectangles validés
  for (auto &r : m_rectangles) {
    if (r.rect.width() > 0 && r.rect.height() > 0) {
      p.drawRect(r.rect);
    }
  }

  // Rectangle en cours seulement si on dessine
  if (m_isDrawing) {
    QRect currentRect = QRect(m_startPoint, m_currentPoint).normalized();
    if (currentRect.width() > 0 && currentRect.height() > 0) {
      p.setPen(QPen(Qt::black, 1, Qt::DashLine));
      p.drawRect(currentRect);
    }
  }
}

void RectangleTool::onRemoteJson(const QJsonObject &msg) {
  if (!msg.contains("props"))
    return;

  auto props = msg["props"].toObject();
  if (!props.contains("action"))
    return;

  QString action = props["action"].toString();

  if (action == "draw_rectangle") {
    // Réception d'un rectangle complet
    if (!props.contains("rectangle"))
      return;

    QJsonObject rectObj = props["rectangle"].toObject();
    if (!rectObj.contains("x") || !rectObj.contains("y") ||
        !rectObj.contains("width") || !rectObj.contains("height"))
      return;

    QRect rect(rectObj["x"].toInt(), rectObj["y"].toInt(),
               rectObj["width"].toInt(), rectObj["height"].toInt());

    if (rect.width() > 0 && rect.height() > 0) {
      QString userId = msg["id"].toString();
      if (!userId.isEmpty()) {
        m_rectangles.append({userId, rect});
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

    if (action == "draw_start") {
      m_startPoint = pt;
      m_currentPoint = pt;
    } else if (action == "draw_move") {
      m_currentPoint = pt;
    } else if (action == "draw_end") {
      m_currentPoint = pt;
      QRect rect = QRect(m_startPoint, m_currentPoint).normalized();
      if (rect.width() > 0 && rect.height() > 0) {
        QString userId = msg["id"].toString();
        if (!userId.isEmpty()) {
          m_rectangles.append({userId, rect});
        }
      }
    }
  }
}

void RectangleTool::clear() {
  m_rectangles.clear();
  m_isDrawing = false;
}