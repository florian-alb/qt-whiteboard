#pragma once
#include "cursortracker.h"
#include "tool.h"
#include <QPixmap>
#include <QWidget>

class WhiteboardCanvas : public QWidget {
  Q_OBJECT
public:
  explicit WhiteboardCanvas(QWidget *parent = nullptr);
  void setTool(Tool *tool);
signals:
  void sendJson(const QJsonObject &msg);
public slots:
  void applyRemote(const QJsonObject &msg);

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

private:
  QPixmap m_pixmap;
  Tool *m_tool = nullptr;
  CursorTracker *m_cursorTracker = nullptr;
};
