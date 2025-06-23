#pragma once
#include <QGraphicsView>
#include <QMouseEvent>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;

signals:
    void sceneClicked(const QPointF& scenePos);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
};
