#include "textitem.h"

TextItem::TextItem(const QPointF& position, const QString& text)
    : WhiteboardElement(position, WhiteboardElement::Text), m_text(text), m_color(Qt::black) {
    // Set default font
    m_font.setFamily("Arial");
    m_font.setPointSize(12);
}

QJsonObject TextItem::toJson() const {
    QJsonObject json = WhiteboardElement::toJson();
    json["text"] = m_text;
    json["fontFamily"] = m_font.family();
    json["fontSize"] = m_font.pointSize();
    json["fontBold"] = m_font.bold();
    json["fontItalic"] = m_font.italic();
    json["colorR"] = m_color.red();
    json["colorG"] = m_color.green();
    json["colorB"] = m_color.blue();
    json["colorA"] = m_color.alpha();
    return json;
}

void TextItem::fromJson(const QJsonObject& json) {
    WhiteboardElement::fromJson(json);
    
    if (json.contains("text")) {
        m_text = json["text"].toString();
    }
    
    if (json.contains("fontFamily")) {
        m_font.setFamily(json["fontFamily"].toString());
    }
    
    if (json.contains("fontSize")) {
        m_font.setPointSize(json["fontSize"].toInt());
    }
    
    if (json.contains("fontBold")) {
        m_font.setBold(json["fontBold"].toBool());
    }
    
    if (json.contains("fontItalic")) {
        m_font.setItalic(json["fontItalic"].toBool());
    }
    
    if (json.contains("colorR") && json.contains("colorG") && json.contains("colorB") && json.contains("colorA")) {
        m_color = QColor(
            json["colorR"].toInt(),
            json["colorG"].toInt(),
            json["colorB"].toInt(),
            json["colorA"].toInt()
        );
    }
}
