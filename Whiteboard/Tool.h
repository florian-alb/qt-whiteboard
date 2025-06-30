#pragma once
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPoint>

class Tool : public QObject {
  Q_OBJECT
public:
  explicit Tool(QObject *parent = nullptr) : QObject(parent) {}
  virtual void onMousePress(const QPoint &pt) = 0;
  virtual void onMouseMove(const QPoint &pt) = 0;
  virtual void onMouseRelease(const QPoint &pt) = 0;
  virtual void draw(QPainter &p) = 0;
  virtual void onRemoteJson(const QJsonObject &msg) = 0;
  virtual void clear() = 0;
  
  // Method to erase elements near a point with a given radius
  virtual void eraseNear(const QPoint &pt, int radius) {}
  
signals:
  void sendJson(const QJsonObject &msg);
};
