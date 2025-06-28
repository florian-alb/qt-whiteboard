#include "canvasview.h"
#include "../business/whiteboardcontroller.h"
#include "../data/textitem.h"

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QDebug>

CanvasView::CanvasView(WhiteboardController* controller, QWidget* parent)
    : QGraphicsView(parent), m_controller(controller), m_activeTool(Select),
      m_editingTextItem(nullptr), m_isDragging(false), m_selectedItem(nullptr) {
    
    setupScene();
    connectControllerSignals();
}

CanvasView::~CanvasView() {
}

void CanvasView::setupScene() {
    // Create and set up the scene
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 2000, 2000); // Large canvas
    setScene(scene);
    
    // Set view properties
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Set background color to white
    setBackgroundBrush(Qt::white);
    
    // Enable drag mode for panning
    setDragMode(QGraphicsView::NoDrag);
    
    // Enable mouse tracking
    setMouseTracking(true);
}

void CanvasView::connectControllerSignals() {
    if (!m_controller) {
        return;
    }
    
    // Connect controller signals
    connect(m_controller, &WhiteboardController::textElementAdded,
            this, &CanvasView::onTextElementAdded);
    connect(m_controller, &WhiteboardController::elementRemoved,
            this, &CanvasView::onElementRemoved);
    connect(m_controller, &WhiteboardController::elementPositionUpdated,
            this, &CanvasView::onElementPositionUpdated);
    connect(m_controller, &WhiteboardController::textElementUpdated,
            this, &CanvasView::onTextElementUpdated);
}

void CanvasView::setActiveTool(int toolId) {
    m_activeTool = static_cast<Tool>(toolId);
    
    // Update cursor based on active tool
    switch (m_activeTool) {
        case Select:
            setCursor(Qt::ArrowCursor);
            break;
        case Text:
            setCursor(Qt::IBeamCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

void CanvasView::mousePressEvent(QMouseEvent* event) {
    if (!m_controller) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    
    // Get scene position
    QPointF scenePos = mapToScene(event->pos());
    m_lastMousePos = scenePos;
    
    // Handle based on active tool
    switch (m_activeTool) {
        case Select: {
            // Find item under cursor
            QGraphicsItem* item = scene()->itemAt(scenePos, transform());
            
            // Deselect previous item
            if (m_selectedItem) {
                m_selectedItem->setSelected(false);
            }
            
            // Select new item
            if (item) {
                item->setSelected(true);
                m_selectedItem = item;
                
                // Find element ID for this item
                QUuid elementId;
                for (auto it = m_elementItems.begin(); it != m_elementItems.end(); ++it) {
                    if (it.value() == item) {
                        elementId = it.key();
                        break;
                    }
                }
                
                if (!elementId.isNull()) {
                    emit elementSelected(elementId);
                }
                
                m_isDragging = true;
            }
            break;
        }
        case Text:
            // Add text at click position
            emit textAdded(scenePos);
            break;
        default:
            break;
    }
    
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    
    // Handle dragging
    if (m_isDragging && m_selectedItem) {
        QPointF delta = scenePos - m_lastMousePos;
        m_selectedItem->moveBy(delta.x(), delta.y());
        
        // Find element ID for this item
        QUuid elementId;
        for (auto it = m_elementItems.begin(); it != m_elementItems.end(); ++it) {
            if (it.value() == m_selectedItem) {
                elementId = it.key();
                break;
            }
        }
        
        if (!elementId.isNull() && m_controller) {
            m_controller->updateElementPosition(elementId, m_selectedItem->pos());
        }
        
        m_lastMousePos = scenePos;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent* event) {
    m_isDragging = false;
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    
    // Check if double-clicked on a text item
    QGraphicsItem* item = scene()->itemAt(scenePos, transform());
    if (item && item->type() == QGraphicsTextItem::Type) {
        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        
        // Find element ID for this item
        QUuid elementId;
        for (auto it = m_elementItems.begin(); it != m_elementItems.end(); ++it) {
            if (it.value() == textItem) {
                elementId = it.key();
                break;
            }
        }
        
        if (!elementId.isNull()) {
            // Enable editing
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setFocus();
            
            // Store editing state
            m_editingTextItem = textItem;
            m_editingElementId = elementId;
            
            // Connect to text changes
            connect(textItem->document(), &QTextDocument::contentsChanged, this, &CanvasView::onTextItemEdited);
        }
    }
    
    QGraphicsView::mouseDoubleClickEvent(event);
}

void CanvasView::onTextElementAdded(const QUuid& elementId, const QPointF& position, const QString& text) {
    // Create text item
    QGraphicsTextItem* textItem = createTextItem(elementId, position, text);
    
    // Add to map
    m_elementItems[elementId] = textItem;
    
    // If this is a new empty text item and we're in text tool mode, start editing
    if (text.isEmpty() && m_activeTool == Text) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus();
        
        // Store editing state
        m_editingTextItem = textItem;
        m_editingElementId = elementId;
        
        // Connect to text changes
        connect(textItem->document(), &QTextDocument::contentsChanged, this, &CanvasView::onTextItemEdited);
    }
}

void CanvasView::onElementRemoved(const QUuid& elementId) {
    if (m_elementItems.contains(elementId)) {
        QGraphicsItem* item = m_elementItems[elementId];
        
        // Remove from scene
        scene()->removeItem(item);
        
        // Clean up
        delete item;
        m_elementItems.remove(elementId);
        
        // Reset editing state if this was the editing item
        if (m_editingElementId == elementId) {
            m_editingTextItem = nullptr;
            m_editingElementId = QUuid();
        }
        
        // Reset selected item if this was the selected item
        if (m_selectedItem == item) {
            m_selectedItem = nullptr;
        }
    }
}

void CanvasView::onElementPositionUpdated(const QUuid& elementId, const QPointF& position) {
    if (m_elementItems.contains(elementId)) {
        QGraphicsItem* item = m_elementItems[elementId];
        item->setPos(position);
    }
}

void CanvasView::onTextElementUpdated(const QUuid& elementId, const QString& text) {
    if (m_elementItems.contains(elementId)) {
        QGraphicsItem* item = m_elementItems[elementId];
        
        if (item->type() == QGraphicsTextItem::Type) {
            QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
            
            // Only update if this isn't the item being edited
            if (textItem != m_editingTextItem) {
                textItem->setPlainText(text);
            }
        }
    }
}

void CanvasView::onTextItemEdited() {
    if (m_editingTextItem && !m_editingElementId.isNull() && m_controller) {
        // Update the text in the model
        m_controller->updateTextElement(m_editingElementId, m_editingTextItem->toPlainText());
    }
}

QGraphicsTextItem* CanvasView::createTextItem(const QUuid& elementId, const QPointF& position, const QString& text) {
    // Create text item
    QGraphicsTextItem* textItem = new QGraphicsTextItem(text);
    textItem->setPos(position);
    textItem->setFlag(QGraphicsItem::ItemIsSelectable);
    textItem->setFlag(QGraphicsItem::ItemIsMovable);
    
    // Add to scene
    scene()->addItem(textItem);
    
    return textItem;
}
