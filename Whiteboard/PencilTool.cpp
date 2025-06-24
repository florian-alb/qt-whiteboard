#include "PencilTool.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>

PencilTool::PencilTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId)
{
}

void PencilTool::emitJson(const QString &action, const QPoint &pt)
{
    QJsonObject msg;
    msg["id"] = m_userId;
    QJsonObject props;
    props["action"] = action;
    QJsonArray arr;
    arr.append(pt.x());
    arr.append(pt.y());
    props["coordinates"] = arr;
    msg["props"] = props;
    emit sendJson(msg);
}

void PencilTool::onMousePress(const QPoint &pt)
{
    m_points.clear();
    m_points.append(pt);
    emitJson("draw_start", pt);
}

void PencilTool::onMouseMove(const QPoint &pt)
{
    m_points.append(pt);
    emitJson("draw_continue", pt);
}

void PencilTool::onMouseRelease(const QPoint &pt)
{
    m_points.append(pt);
    emitJson("draw_end", pt);
    // commit stroke
    m_strokes.append({m_userId, m_points});
    m_points.clear();
}

void PencilTool::draw(QPainter &p)
{
    p.setPen(Qt::black);
    // draw committed strokes
    for (auto &s : m_strokes)
    {
        for (int i = 1; i < s.points.size(); ++i)
            p.drawLine(s.points[i - 1], s.points[i]);
    }
    // draw current
    for (int i = 1; i < m_points.size(); ++i)
        p.drawLine(m_points[i - 1], m_points[i]);
}

void PencilTool::onRemoteJson(const QJsonObject &msg)
{
    auto props = msg["props"].toObject();
    QString action = props["action"].toString();
    QJsonArray arr = props["coordinates"].toArray();
    QPoint pt(arr[0].toInt(), arr[1].toInt());
    // For simplicity, treat other users’ strokes as immediate commits
    if (action == "draw_start")
    {
        Stroke s{msg["id"].toString(), {pt}};
        m_strokes.append(s);
    }
    else if (action == "draw_continue" || action == "draw_end")
    {
        // append to last stroke
        m_strokes.last().points.append(pt);
    }
}
