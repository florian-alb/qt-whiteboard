#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>

BoardWidget::BoardWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
}

void BoardWidget::addPoint(int x, int y)
{
    m_points.append({x, y});
    update();
}

void BoardWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    for (auto& pt : m_points) {
        p.drawEllipse(pt, 3, 3);
    }
}

void BoardWidget::mousePressEvent(QMouseEvent* ev)
{
    QPoint pt = ev->pos();
    m_points.append(pt);
    emit pointCreated(pt.x(), pt.y());
    update();
}
