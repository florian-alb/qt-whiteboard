#pragma once
#include <QWidget>
#include <QVector>
#include <QPoint>

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget* parent = nullptr);
    void addPoint(int x, int y);

signals:
    void pointCreated(int x, int y);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* ev) override;

private:
    QVector<QPoint> m_points;
};
