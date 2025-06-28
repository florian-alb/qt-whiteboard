#ifndef WHITEBOARDCONTROLLER_H
#define WHITEBOARDCONTROLLER_H

#include "../data/whiteboardmodel.h"
#include "../data/whiteboardelement.h"
#include "syncmanager.h"
#include <QObject>
#include <memory>

/**
 * @brief Controller class for whiteboard operations
 */
class WhiteboardController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    WhiteboardController(QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~WhiteboardController();
    
    /**
     * @brief Adds a text element to the whiteboard
     * @param position Position of the text
     * @param text Initial text content
     * @return ID of the created element
     */
    QUuid addTextElement(const QPointF& position, const QString& text = "");
    
    /**
     * @brief Removes an element from the whiteboard
     * @param elementId ID of the element to remove
     * @return True if the element was removed, false otherwise
     */
    bool removeElement(const QUuid& elementId);
    
    /**
     * @brief Updates the position of an element
     * @param elementId ID of the element to update
     * @param position New position
     * @return True if the element was updated, false otherwise
     */
    bool updateElementPosition(const QUuid& elementId, const QPointF& position);
    
    /**
     * @brief Updates the text of a text element
     * @param elementId ID of the text element to update
     * @param text New text content
     * @return True if the element was updated, false otherwise
     */
    bool updateTextElement(const QUuid& elementId, const QString& text);
    
    /**
     * @brief Gets the whiteboard model
     * @return Pointer to the whiteboard model
     */
    WhiteboardModel* getModel() const { return m_model.get(); }
    
    /**
     * @brief Sets the current user ID
     * @param userId ID of the current user
     */
    void setCurrentUserId(const QString& userId) { m_currentUserId = userId; }
    
    /**
     * @brief Gets the current user ID
     * @return Current user ID
     */
    QString getCurrentUserId() const { return m_currentUserId; }
    
    /**
     * @brief Sets the sync manager
     * @param syncManager Pointer to the sync manager
     */
    void setSyncManager(SyncManager* syncManager);

signals:
    /**
     * @brief Signal emitted when a text element is added
     * @param elementId ID of the added element
     * @param position Position of the text
     * @param text Text content
     */
    void textElementAdded(const QUuid& elementId, const QPointF& position, const QString& text);
    
    /**
     * @brief Signal emitted when an element is removed
     * @param elementId ID of the removed element
     */
    void elementRemoved(const QUuid& elementId);
    
    /**
     * @brief Signal emitted when an element's position is updated
     * @param elementId ID of the updated element
     * @param position New position
     */
    void elementPositionUpdated(const QUuid& elementId, const QPointF& position);
    
    /**
     * @brief Signal emitted when a text element is updated
     * @param elementId ID of the updated element
     * @param text New text content
     */
    void textElementUpdated(const QUuid& elementId, const QString& text);

private:
    std::unique_ptr<WhiteboardModel> m_model;
    QString m_currentUserId;
    SyncManager* m_syncManager;
};

#endif // WHITEBOARDCONTROLLER_H
