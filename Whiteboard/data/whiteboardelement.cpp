#include "whiteboardelement.h"
#include <QUuid>

WhiteboardElement::WhiteboardElement(const QPointF& position, ElementType type)
    : m_id(QUuid::createUuid()), m_position(position), m_type(type), m_authorId("") {
}

WhiteboardElement::~WhiteboardElement() {
}

QJsonObject WhiteboardElement::toJson() const {
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["posX"] = m_position.x();
    json["posY"] = m_position.y();
    json["type"] = static_cast<int>(m_type);
    json["authorId"] = m_authorId;
    return json;
}

void WhiteboardElement::fromJson(const QJsonObject& json) {
    if (json.contains("id")) {
        m_id = QUuid(json["id"].toString());
    }
    
    if (json.contains("posX") && json.contains("posY")) {
        m_position = QPointF(json["posX"].toDouble(), json["posY"].toDouble());
    }
    
    if (json.contains("type")) {
        m_type = static_cast<ElementType>(json["type"].toInt());
    }
    
    if (json.contains("authorId")) {
        m_authorId = json["authorId"].toString();
    }
}
