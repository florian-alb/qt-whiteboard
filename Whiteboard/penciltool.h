#pragma once
#include "tool.h"
#include <QPoint>
#include <QVector>
#include <QSet>

class PencilTool : public Tool {
  Q_OBJECT
public:
  explicit PencilTool(const QString &userId, QObject *parent = nullptr);
  void onMousePress(const QPoint &pt) override;
  void onMouseMove(const QPoint &pt) override;
  void onMouseRelease(const QPoint &pt) override;
  void draw(QPainter &p) override;
  void onRemoteJson(const QJsonObject &msg) override;
  void clear() override;
  
  // New method to erase strokes near a point
  void eraseNear(const QPoint &pt, int radius);

private:
  QString m_userId;
  struct Stroke {
    QString id;
    QVector<QPoint> points;
  };

  QVector<Stroke> m_strokes;
  QVector<QPoint> m_current;
  bool m_isDrawing = false;

  void emitJson(const QString &action, const QPoint &pt);
  void emitStrokeJson(const QVector<QPoint> &points);
  
  // Helper method to calculate distance from a point to a line segment
  double distanceToLineSegment(const QPoint &p1, const QPoint &p2, const QPoint &point);
};
