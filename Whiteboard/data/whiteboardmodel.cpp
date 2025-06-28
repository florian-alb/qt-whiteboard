#include "whiteboardmodel.h"
#include "textitem.h"
#include <QJsonArray>

WhiteboardModel::WhiteboardModel() : QObject(nullptr) {
}

WhiteboardModel::~WhiteboardModel() {
    clear();
}

void WhiteboardModel::addElement(std::shared_ptr<WhiteboardElement> element, bool sync) {
    if (!element) {
        return;
    }
    
    m_elements.append(element);
    if (sync) {
        emit elementAdded(element->getId());
    }
}

bool WhiteboardModel::removeElement(const QUuid& elementId, bool sync) {
    for (int i = 0; i < m_elements.size(); ++i) {
        if (m_elements[i]->getId() == elementId) {
            m_elements.removeAt(i);
            if (sync) {
                emit elementRemoved(elementId);
            }
            return true;
        }
    }
    return false;
}

std::shared_ptr<WhiteboardElement> WhiteboardModel::getElement(const QUuid& elementId) {
    for (const auto& element : m_elements) {
        if (element->getId() == elementId) {
            return element;
        }
    }
    return nullptr;
}

QList<std::shared_ptr<WhiteboardElement>> WhiteboardModel::getAllElements() const {
    return m_elements;
}

bool WhiteboardModel::updateElementPosition(const QUuid& elementId, const QPointF& position, bool sync) {
    auto element = getElement(elementId);
    if (!element) {
        return false;
    }
    
    element->setPosition(position);
    
    if (sync) {
        emit elementPositionUpdated(elementId, position);
    }
    
    return true;
}

bool WhiteboardModel::updateElement(std::shared_ptr<WhiteboardElement> element, bool sync) {
    if (!element) {
        return false;
    }
    
    // Find and replace the element with the same ID
    for (int i = 0; i < m_elements.size(); ++i) {
        if (m_elements[i]->getId() == element->getId()) {
            m_elements[i] = element;
            
            // Emit appropriate signals based on element type
            if (sync) {
                emit elementPositionUpdated(element->getId(), element->getPosition());
                
                if (element->getType() == WhiteboardElement::Text) {
                    auto textItem = std::dynamic_pointer_cast<TextItem>(element);
                    if (textItem) {
                        emit textElementUpdated(element->getId(), textItem->getText());
                    }
                }
            }
            
            return true;
        }
    }
    
    // If element wasn't found, add it as a new element
    addElement(element, sync);
    return true;
}

void WhiteboardModel::clear() {
    m_elements.clear();
    emit modelCleared();
}

QJsonObject WhiteboardModel::toJson() const {
    QJsonObject json;
    QJsonArray elementsArray;
    
    for (const auto& element : m_elements) {
        QJsonObject elementJson = element->toJson();
        elementsArray.append(elementJson);
    }
    
    json["elements"] = elementsArray;
    return json;
}

void WhiteboardModel::fromJson(const QJsonObject& json) {
    clear();
    
    if (json.contains("elements") && json["elements"].isArray()) {
        QJsonArray elementsArray = json["elements"].toArray();
        
        for (const QJsonValue& value : elementsArray) {
            if (value.isObject()) {
                QJsonObject elementJson = value.toObject();
                
                if (elementJson.contains("type")) {
                    WhiteboardElement::ElementType type = static_cast<WhiteboardElement::ElementType>(elementJson["type"].toInt());
                    
                    std::shared_ptr<WhiteboardElement> element;
                    switch (type) {
                        case WhiteboardElement::Text: {
                            QPointF pos(elementJson["posX"].toDouble(), elementJson["posY"].toDouble());
                            element = std::make_shared<TextItem>(pos);
                            break;
                        }
                        // Add other element types as needed
                        default:
                            continue;
                    }
                    
                    element->fromJson(elementJson);
                    m_elements.append(element);
                }
            }
        }
    }
    
    emit modelCleared(); // Signal that the model has been completely changed
}
