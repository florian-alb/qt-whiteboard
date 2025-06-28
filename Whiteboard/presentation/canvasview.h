#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QUuid>
#include <QMap>
#include <memory>

class WhiteboardController;
class QGraphicsTextItem;
class QGraphicsItem;

/**
 * @brief Canvas view for displaying and interacting with whiteboard elements
 */
class CanvasView : public QGraphicsView {
    Q_OBJECT

public:
    enum Tool {
        Select,
        Text,
        // Other tools will be added in future implementations
    };

    /**
     * @brief Constructor
     * @param controller Whiteboard controller
     * @param parent Parent widget
     */
    CanvasView(WhiteboardController* controller, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~CanvasView();
    
    /**
     * @brief Sets the active tool
     * @param toolId ID of the tool to activate
     */
    void setActiveTool(int toolId);

signals:
    /**
     * @brief Signal emitted when text is added
     * @param position Position where text was added
     */
    void textAdded(const QPointF& position);
    
    /**
     * @brief Signal emitted when an element is selected
     * @param elementId ID of the selected element
     */
    void elementSelected(const QUuid& elementId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    void onTextElementAdded(const QUuid& elementId, const QPointF& position, const QString& text);
    void onElementRemoved(const QUuid& elementId);
    void onElementPositionUpdated(const QUuid& elementId, const QPointF& position);
    void onTextElementUpdated(const QUuid& elementId, const QString& text);
    void onTextItemEdited();

private:
    WhiteboardController* m_controller;
    Tool m_activeTool;
    QMap<QUuid, QGraphicsItem*> m_elementItems;
    QGraphicsTextItem* m_editingTextItem;
    QUuid m_editingElementId;
    QPointF m_lastMousePos;
    bool m_isDragging;
    QGraphicsItem* m_selectedItem;
    
    void setupScene();
    void connectControllerSignals();
    QGraphicsTextItem* createTextItem(const QUuid& elementId, const QPointF& position, const QString& text);
};

#endif // CANVASVIEW_H
