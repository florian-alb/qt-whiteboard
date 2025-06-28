#pragma once
#include "tool.h"
#include <QPoint>
#include <QVector>

class CircleTool : public Tool {
  Q_OBJECT
public:
  explicit CircleTool(const QString &userId, QObject *parent = nullptr);
  void onMousePress(const QPoint &pt) override;
  void onMouseMove(const QPoint &pt) override;
  void onMouseRelease(const QPoint &pt) override;
  void draw(QPainter &p) override;
  void onRemoteJson(const QJsonObject &msg) override;
  void clear() override;

private:
  QString m_userId;
  struct Circle {
    QString id;
    QPoint center;
    int radius;
  };

  QVector<Circle> m_circles;
  QPoint m_startPoint;
  QPoint m_currentPoint;
  bool m_isDrawing = false;

  void emitJson(const QString &action, const QPoint &pt);
  void emitCircleJson(const QPoint &center, int radius);
};