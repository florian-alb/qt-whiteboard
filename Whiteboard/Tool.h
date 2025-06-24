#pragma once
#include <QObject>
#include <QJsonObject>
#include <QPainter>
#include <QPoint>

class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject *parent = nullptr) : QObject(parent) {}
    virtual void onMousePress(const QPoint &pt) = 0;
    virtual void onMouseMove(const QPoint &pt) = 0;
    virtual void onMouseRelease(const QPoint &pt) = 0;
    virtual void draw(QPainter &p) = 0;
signals:
    void sendJson(const QJsonObject &msg);
};
