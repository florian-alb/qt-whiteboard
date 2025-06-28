#pragma once
#include "tool.h"
#include <QPoint>
#include <QVector>

class EraserTool : public Tool {
  Q_OBJECT
public:
  explicit EraserTool(const QString &userId, QObject *parent = nullptr);
  void onMousePress(const QPoint &pt) override;
  void onMouseMove(const QPoint &pt) override;
  void onMouseRelease(const QPoint &pt) override;
  void draw(QPainter &p) override;
  void onRemoteJson(const QJsonObject &msg) override;
  void clear() override;

  // Static method to check if a message is from an eraser tool
  static bool isEraserMessage(const QJsonObject &msg);

private:
  QString m_userId;
  struct Stroke {
    QString id;
    QVector<QPoint> points;
  };

  QVector<Stroke> m_strokes;
  QVector<QPoint> m_current;
  bool m_isErasing = false;
  int m_eraserSize = 30;

  void emitJson(const QString &action, const QPoint &pt);
  void emitStrokeJson(const QVector<QPoint> &points);
};
