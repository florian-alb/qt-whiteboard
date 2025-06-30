#pragma once
#include "tool.h"
#include <QPoint>
#include <QRect>
#include <QVector>

class RectangleTool : public Tool {
  Q_OBJECT
public:
  explicit RectangleTool(const QString &userId, QObject *parent = nullptr);
  void onMousePress(const QPoint &pt) override;
  void onMouseMove(const QPoint &pt) override;
  void onMouseRelease(const QPoint &pt) override;
  void draw(QPainter &p) override;
  void onRemoteJson(const QJsonObject &msg) override;
  void clear() override;
  void eraseNear(const QPoint &pt, int radius) override;

private:
  QString m_userId;
  struct Rectangle {
    QString id;
    QRect rect;
  };

  QVector<Rectangle> m_rectangles;
  QPoint m_startPoint;
  QPoint m_currentPoint;
  bool m_isDrawing = false;

  void emitJson(const QString &action, const QPoint &pt);
  void emitRectangleJson(const QRect &rect);
};