#pragma once
#include "Tool.h"
#include <QPoint>
#include <QVector>

class PencilTool : public Tool
{
    Q_OBJECT
public:
    PencilTool(const QString &userId, QObject *parent = nullptr);
    void onMousePress(const QPoint &pt) override;
    void onMouseMove(const QPoint &pt) override;
    void onMouseRelease(const QPoint &pt) override;
    void draw(QPainter &p) override;
public slots:
    void onRemoteJson(const QJsonObject &msg);

private:
    QString m_userId;
    QVector<QPoint> m_points;
    struct Stroke
    {
        QString id;
        QVector<QPoint> points;
    };
    QVector<Stroke> m_strokes;
    void emitJson(const QString &action, const QPoint &pt);
};
