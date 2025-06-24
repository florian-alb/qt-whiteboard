#include "WhiteboardCanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonObject>

WhiteboardCanvas::WhiteboardCanvas(QWidget *parent)
    : QWidget(parent), m_pixmap(size())
{
    m_pixmap.fill(Qt::white);
}

void WhiteboardCanvas::setTool(Tool *tool)
{
    if (m_tool)
        disconnect(m_tool, nullptr, this, nullptr);
    m_tool = tool;
    connect(m_tool, &Tool::sendJson,
            this, [this](const QJsonObject &msg)
            { emit sendJson(msg); });
}

void WhiteboardCanvas::applyRemote(const QJsonObject &msg)
{
    // Pass JSON to tool for drawing
    if (m_tool)
    {
        // assume Tool subclass knows how to interpret remote JSON
        QMetaObject::invokeMethod(m_tool, "onRemoteJson",
                                  Q_ARG(QJsonObject, msg));
        update();
    }
}

void WhiteboardCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, m_pixmap);
    if (m_tool)
        m_tool->draw(p);
}

void WhiteboardCanvas::mousePressEvent(QMouseEvent *e)
{
    if (!m_tool)
        return;
    m_tool->onMousePress(e->pos());
    update();
}

void WhiteboardCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_tool)
        return;
    m_tool->onMouseMove(e->pos());
    update();
}

void WhiteboardCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_tool)
        return;
    m_tool->onMouseRelease(e->pos());
    update();
}
