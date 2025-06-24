#pragma once
#include <QWidget>
#include <QPixmap>
#include "tool.h"

class WhiteboardCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WhiteboardCanvas(QWidget* parent = nullptr);
    void setTool(Tool* tool);
signals:
    void sendJson(const QJsonObject& msg);
public slots:
    void applyRemote(const QJsonObject& msg);
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
private:
    QPixmap m_pixmap;
    Tool*   m_tool = nullptr;
};
