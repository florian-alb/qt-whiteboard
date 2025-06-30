#pragma once
#include "tool.h"
#include <QPoint>
#include <QVector>
#include <QGraphicsItem>
#include "whiteboardcanvas.h"

class WhiteboardCanvas;

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
  
  // New methods for erasing strokes
  void eraseAtPoint(const QPoint &pt);
  void processRemoteErase(const QPoint &pt);

  // Set the canvas directly
  void setCanvas(WhiteboardCanvas* canvas) { m_canvas = canvas; }

private:
  QString m_userId;
  QVector<QPoint> m_current;
  bool m_isErasing = false;
  int m_eraserSize = 30;
  
  // Track erased points for potential undo functionality
  QVector<QPoint> m_erasedStrokes;

  WhiteboardCanvas* m_canvas = nullptr;  // Direct reference to the canvas

  void emitJson(const QString &action, const QPoint &pt);
  void emitStrokeJson(const QVector<QPoint> &points);
};
