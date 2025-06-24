#include "penciltool.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>

PencilTool::PencilTool(const QString& userId, QObject* parent)
    : Tool(parent), m_userId(userId)
{}

void PencilTool::emitJson(const QString& action, const QPoint& pt) {
    QJsonObject msg;
    msg["id"] = m_userId;
    QJsonObject props;
    props["action"] = action;
    QJsonArray coords; coords.append(pt.x()); coords.append(pt.y());
    props["coordinates"] = coords;
    msg["props"] = props;
    emit sendJson(msg);
}

void PencilTool::onMousePress(const QPoint& pt) {
    m_current.clear();
    m_current.append(pt);
    emitJson("draw_start", pt);
}

void PencilTool::onMouseMove(const QPoint& pt) {
    m_current.append(pt);
    emitJson("draw_continue", pt);
}

void PencilTool::onMouseRelease(const QPoint& pt) {
    m_current.append(pt);
    emitJson("draw_end", pt);
    m_strokes.append({m_userId, m_current});
    m_current.clear();
}

void PencilTool::draw(QPainter& p) {
    p.setPen(Qt::black);
    // traits validés
    for (auto& s : m_strokes) {
        for (int i = 1; i < s.points.size(); ++i)
            p.drawLine(s.points[i-1], s.points[i]);
    }
    // trait en cours
    for (int i = 1; i < m_current.size(); ++i)
        p.drawLine(m_current[i-1], m_current[i]);
}

void PencilTool::onRemoteJson(const QJsonObject& msg) {
    auto props = msg["props"].toObject();
    QString action = props["action"].toString();
    QJsonArray arr = props["coordinates"].toArray();
    QPoint pt(arr[0].toInt(), arr[1].toInt());

    // find or create stroke for this user
    if (action == "draw_start") {
        m_strokes.append({ msg["id"].toString(), {pt} });
    } else if (!m_strokes.isEmpty()) {
        m_strokes.last().points.append(pt);
    }
}
