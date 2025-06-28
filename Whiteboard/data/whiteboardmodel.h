#ifndef WHITEBOARDMODEL_H
#define WHITEBOARDMODEL_H

#include "whiteboardelement.h"
#include <QList>
#include <QUuid>
#include <memory>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Model class that stores all whiteboard elements
 */
class WhiteboardModel : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    WhiteboardModel();
    
    /**
     * @brief Destructor
     */
    ~WhiteboardModel();
    
    /**
     * @brief Adds an element to the model
     * @param element Element to add
     * @param sync Whether to emit signals for synchronization (default: true)
     */
    void addElement(std::shared_ptr<WhiteboardElement> element, bool sync = true);
    
    /**
     * @brief Removes an element from the model
     * @param elementId ID of the element to remove
     * @param sync Whether to emit signals for synchronization (default: true)
     * @return True if the element was removed, false otherwise
     */
    bool removeElement(const QUuid& elementId, bool sync = true);
    
    /**
     * @brief Updates an element in the model
     * @param elementId ID of the element to update
     * @param position New position
     * @param sync Whether to emit signals for synchronization (default: true)
     * @return True if the element was updated, false otherwise
     */
    bool updateElementPosition(const QUuid& elementId, const QPointF& position, bool sync = true);
    
    /**
     * @brief Updates an element in the model with a new element
     * @param element Updated element
     * @param sync Whether to emit signals for synchronization (default: true)
     * @return True if the element was updated, false otherwise
     */
    bool updateElement(std::shared_ptr<WhiteboardElement> element, bool sync = true);
    
    /**
     * @brief Gets an element from the model
     * @param elementId ID of the element to get
     * @return Pointer to the element if found, nullptr otherwise
     */
    std::shared_ptr<WhiteboardElement> getElement(const QUuid& elementId);
    
    /**
     * @brief Gets all elements in the model
     * @return List of all elements
     */
    QList<std::shared_ptr<WhiteboardElement>> getAllElements() const;
    
    /**
     * @brief Clears all elements from the model
     */
    void clear();
    
    /**
     * @brief Serializes the model to JSON
     * @return JSON object representing the model
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Deserializes the model from JSON
     * @param json JSON object to deserialize from
     */
    void fromJson(const QJsonObject& json);

signals:
    /**
     * @brief Signal emitted when an element is added
     * @param elementId ID of the added element
     */
    void elementAdded(const QUuid& elementId);
    
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
    
    /**
     * @brief Signal emitted when the model is cleared
     */
    void modelCleared();
    
    /**
     * @brief Signal emitted when an element is modified
     * @param elementId ID of the modified element
     */
    void elementModified(const QUuid& elementId);

private:
    QList<std::shared_ptr<WhiteboardElement>> m_elements;
};

#endif // WHITEBOARDMODEL_H
