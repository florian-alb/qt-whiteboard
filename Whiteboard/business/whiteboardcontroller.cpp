#include "whiteboardcontroller.h"
#include "../data/textitem.h"

WhiteboardController::WhiteboardController(QObject* parent)
    : QObject(parent), m_model(std::make_unique<WhiteboardModel>()), m_syncManager(nullptr) {
    
    // Connect model signals to controller signals
    connect(m_model.get(), &WhiteboardModel::elementAdded, this, [this](const QUuid& elementId) {
        auto element = m_model->getElement(elementId);
        if (element && element->getType() == WhiteboardElement::Text) {
            auto textItem = std::dynamic_pointer_cast<TextItem>(element);
            if (textItem) {
                emit textElementAdded(elementId, textItem->getPosition(), textItem->getText());
            }
        }
    });
    
    connect(m_model.get(), &WhiteboardModel::elementRemoved, this, &WhiteboardController::elementRemoved);
}

WhiteboardController::~WhiteboardController() {
}

QUuid WhiteboardController::addTextElement(const QPointF& position, const QString& text) {
    auto textItem = std::make_shared<TextItem>(position, text);
    textItem->setAuthorId(m_currentUserId);
    
    m_model->addElement(textItem);
    
    // Sync with other clients if sync manager is available
    if (m_syncManager) {
        m_syncManager->syncElementAdded(textItem);
    }
    
    return textItem->getId();
}

bool WhiteboardController::removeElement(const QUuid& elementId) {
    auto element = m_model->getElement(elementId);
    if (!element) {
        return false;
    }
    
    // Sync with other clients if sync manager is available
    if (m_syncManager) {
        m_syncManager->syncElementRemoved(elementId);
    }
    
    return m_model->removeElement(elementId);
}

bool WhiteboardController::updateElementPosition(const QUuid& elementId, const QPointF& position) {
    auto element = m_model->getElement(elementId);
    if (!element) {
        return false;
    }
    
    element->setPosition(position);
    emit elementPositionUpdated(elementId, position);
    
    // Sync with other clients if sync manager is available
    if (m_syncManager) {
        m_syncManager->syncElementModified(element);
    }
    
    return true;
}

void WhiteboardController::setSyncManager(SyncManager* syncManager) {
    m_syncManager = syncManager;
    
    // Connect SyncManager signals to update the model when network messages are received
    if (m_syncManager) {
        // Handle new elements received from the network
        connect(m_syncManager, &SyncManager::elementReceived, this, 
            [this](std::shared_ptr<WhiteboardElement> element) {
                if (element) {
                    // Add the element to the model without re-syncing (to avoid loops)
                    m_model->addElement(element, false);
                    
                    // Emit appropriate signals based on element type
                    if (element->getType() == WhiteboardElement::Text) {
                        auto textItem = std::dynamic_pointer_cast<TextItem>(element);
                        if (textItem) {
                            emit textElementAdded(element->getId(), textItem->getPosition(), textItem->getText());
                        }
                    }
                }
            });
        
        // Handle element removals received from the network
        connect(m_syncManager, &SyncManager::elementRemovalReceived, this,
            [this](const QUuid& elementId) {
                // Remove the element from the model without re-syncing
                m_model->removeElement(elementId, false);
                
                // Emit signal for the UI to update
                emit elementRemoved(elementId);
            });
        
        // Handle element modifications received from the network
        connect(m_syncManager, &SyncManager::elementModificationReceived, this,
            [this](std::shared_ptr<WhiteboardElement> element) {
                if (element) {
                    // Update the element in the model without re-syncing
                    m_model->updateElement(element, false);
                    
                    // Emit appropriate signals based on element type
                    if (element->getType() == WhiteboardElement::Text) {
                        auto textItem = std::dynamic_pointer_cast<TextItem>(element);
                        if (textItem) {
                            emit textElementUpdated(element->getId(), textItem->getText());
                            emit elementPositionUpdated(element->getId(), textItem->getPosition());
                        }
                    }
                }
            });
    }
}

bool WhiteboardController::updateTextElement(const QUuid& elementId, const QString& text) {
    auto element = m_model->getElement(elementId);
    if (!element || element->getType() != WhiteboardElement::Text) {
        return false;
    }
    
    auto textItem = std::dynamic_pointer_cast<TextItem>(element);
    if (!textItem) {
        return false;
    }
    
    textItem->setText(text);
    emit textElementUpdated(elementId, text);
    
    // Sync with other clients if sync manager is available
    if (m_syncManager) {
        m_syncManager->syncElementModified(element);
    }
    
    return true;
}
