#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "whiteboardelement.h"
#include <QString>
#include <QFont>
#include <QColor>
#include <QJsonObject>

/**
 * @brief Represents a text element on the whiteboard
 */
class TextItem : public WhiteboardElement {
public:
    /**
     * @brief Constructor
     * @param position Initial position of the text
     * @param text Initial text content
     */
    TextItem(const QPointF& position, const QString& text = "");
    
    /**
     * @brief Gets the text content
     * @return Text content
     */
    QString getText() const { return m_text; }
    
    /**
     * @brief Sets the text content
     * @param text New text content
     */
    void setText(const QString& text) { m_text = text; }
    
    /**
     * @brief Gets the text font
     * @return Text font
     */
    QFont getFont() const { return m_font; }
    
    /**
     * @brief Sets the text font
     * @param font New font
     */
    void setFont(const QFont& font) { m_font = font; }
    
    /**
     * @brief Gets the text color
     * @return Text color
     */
    QColor getColor() const { return m_color; }
    
    /**
     * @brief Sets the text color
     * @param color New color
     */
    void setColor(const QColor& color) { m_color = color; }
    
    /**
     * @brief Serializes the text item to JSON
     * @return JSON object representing the text item
     */
    QJsonObject toJson() const override;
    
    /**
     * @brief Deserializes the text item from JSON
     * @param json JSON object to deserialize from
     */
    void fromJson(const QJsonObject& json) override;

private:
    QString m_text;
    QFont m_font;
    QColor m_color;
};

#endif // TEXTITEM_H
