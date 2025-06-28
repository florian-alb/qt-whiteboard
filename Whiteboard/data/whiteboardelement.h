#ifndef WHITEBOARDELEMENT_H
#define WHITEBOARDELEMENT_H

#include <QPointF>
#include <QString>
#include <QColor>
#include <QJsonObject>
#include <QUuid>

/**
 * @brief Base class for all whiteboard elements
 */
class WhiteboardElement {
public:
    enum ElementType {
        Text,
        Shape,
        Drawing,
        Image
    };

    /**
     * @brief Constructor
     * @param position Initial position of the element
     * @param type Type of the element
     */
    WhiteboardElement(const QPointF& position, ElementType type);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~WhiteboardElement();

    /**
     * @brief Gets the element's unique ID
     * @return UUID of the element
     */
    QUuid getId() const { return m_id; }
    
    /**
     * @brief Gets the element's position
     * @return Position as QPointF
     */
    QPointF getPosition() const { return m_position; }
    
    /**
     * @brief Sets the element's position
     * @param position New position
     */
    void setPosition(const QPointF& position) { m_position = position; }
    
    /**
     * @brief Gets the element's type
     * @return Element type
     */
    ElementType getType() const { return m_type; }
    
    /**
     * @brief Gets the element's author ID
     * @return Author ID
     */
    QString getAuthorId() const { return m_authorId; }
    
    /**
     * @brief Sets the element's author ID
     * @param authorId Author ID
     */
    void setAuthorId(const QString& authorId) { m_authorId = authorId; }
    
    /**
     * @brief Serializes the element to JSON
     * @return JSON object representing the element
     */
    virtual QJsonObject toJson() const;
    
    /**
     * @brief Deserializes the element from JSON
     * @param json JSON object to deserialize from
     */
    virtual void fromJson(const QJsonObject& json);

protected:
    QUuid m_id;
    QPointF m_position;
    ElementType m_type;
    QString m_authorId;
};

#endif // WHITEBOARDELEMENT_H
