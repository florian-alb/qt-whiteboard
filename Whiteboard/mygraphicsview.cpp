#include "mygraphicsview.h"

void MyGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        emit sceneClicked(scenePos);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
