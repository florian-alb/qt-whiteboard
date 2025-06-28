#pragma once
#include "cursortracker.h"
#include "tool.h"
#include <QJsonArray>
#include <QPixmap>
#include <QVector>
#include <QWidget>

class WhiteboardCanvas : public QWidget {
  Q_OBJECT
public:
  explicit WhiteboardCanvas(QWidget *parent = nullptr);
  void addTool(Tool *tool);
  void setActiveTool(Tool *tool);
  Tool *getActiveTool() const { return m_activeTool; }

  // Méthodes pour sauvegarder/charger les dessins
  void saveDrawingsToFile(const QString &filename);
  void loadDrawingsFromFile(const QString &filename);
  void clearAllDrawings();

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
  QVector<Tool *> m_tools;
  Tool *m_activeTool = nullptr;
  CursorTracker *m_cursorTracker = nullptr;
  QJsonArray m_drawingLog;
};
