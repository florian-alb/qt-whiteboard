#include "circletool.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QtMath>

CircleTool::CircleTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId) {}

void CircleTool::emitJson(const QString &action, const QPoint &pt) {
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

void CircleTool::emitCircleJson(const QPoint &center, int radius) {
  if (radius <= 0)
    return;

  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object";
  QJsonObject props;
  props["action"] = "draw_circle";

  QJsonObject circleObj;
  circleObj["centerX"] = center.x();
  circleObj["centerY"] = center.y();
  circleObj["radius"] = radius;
  props["circle"] = circleObj;
  msg["props"] = props;

  emit sendJson(msg);
}

void CircleTool::onMousePress(const QPoint &pt) {
  m_startPoint = pt;
  m_currentPoint = pt;
  m_isDrawing = true;
}

void CircleTool::onMouseMove(const QPoint &pt) {
  if (m_isDrawing) {
    m_currentPoint = pt;
  }
}

void CircleTool::onMouseRelease(const QPoint &pt) {
  if (m_isDrawing) {
    m_currentPoint = pt;

    // Calculer le centre et le rayon du cercle
    QPoint center = (m_startPoint + m_currentPoint) / 2;
    int radius = qRound(qSqrt(qPow(m_currentPoint.x() - m_startPoint.x(), 2) +
                              qPow(m_currentPoint.y() - m_startPoint.y(), 2))) /
                 2;

    // Envoyer le cercle complet seulement au relâchement
    if (radius > 0) {
      emitCircleJson(center, radius);
    }

    m_circles.append({m_userId, center, radius});
    m_isDrawing = false;
  }
}

void CircleTool::draw(QPainter &p) {
  // Dessiner tous les cercles finis
  for (const Circle &circle : m_circles) {
    p.drawEllipse(circle.center, circle.radius, circle.radius);
  }

  // Dessiner le cercle en cours de création
  if (m_isDrawing) {
    QPoint center = (m_startPoint + m_currentPoint) / 2;
    int radius = qRound(qSqrt(qPow(m_currentPoint.x() - m_startPoint.x(), 2) +
                              qPow(m_currentPoint.y() - m_startPoint.y(), 2))) /
                 2;

    if (radius > 0) {
      p.drawEllipse(center, radius, radius);
    }
  }
}

void CircleTool::onRemoteJson(const QJsonObject &msg) {
  QString userId = msg["id"].toString();
  auto props = msg["props"].toObject();
  QString action = props["action"].toString();

  if (action == "draw_circle") {
    // Nouveau format pour les cercles
    if (props.contains("circle")) {
      QJsonObject circleObj = props["circle"].toObject();
      QPoint center(circleObj["centerX"].toInt(), circleObj["centerY"].toInt());
      int radius = circleObj["radius"].toInt();

      if (radius > 0) {
        m_circles.append({userId, center, radius});
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
      QPoint center = (m_startPoint + m_currentPoint) / 2;
      int radius =
          qRound(qSqrt(qPow(m_currentPoint.x() - m_startPoint.x(), 2) +
                       qPow(m_currentPoint.y() - m_startPoint.y(), 2))) /
          2;
      if (radius > 0) {
        m_circles.append({userId, center, radius});
      }
    }
  }
}

void CircleTool::clear() {
  m_circles.clear();
  m_isDrawing = false;
}

void CircleTool::eraseNear(const QPoint &pt, int radius) {
  // Calculate the squared radius for more efficient distance comparison
  int radiusSquared = radius * radius;
  
  // Use removeIf with a lambda to remove circles that intersect with the eraser
  auto it = m_circles.begin();
  while (it != m_circles.end()) {
    const Circle &circle = *it;
    
    // Calculate distance between eraser center and circle center
    int dx = circle.center.x() - pt.x();
    int dy = circle.center.y() - pt.y();
    int distanceSquared = dx * dx + dy * dy;
    
    // If the distance between centers is less than sum of radii, they intersect
    if (distanceSquared <= (radiusSquared + circle.radius * circle.radius)) {
      it = m_circles.erase(it);
    } else {
      ++it;
    }
  }
}