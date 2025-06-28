#include "texttool.h"
#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QWidget>

TextTool::TextTool(const QString &userId, QObject *parent)
    : Tool(parent), m_userId(userId), m_textInput(nullptr), m_isEditing(false) {

  // Créer le champ de saisie de texte
  m_textInput = new QLineEdit();
  m_textInput->setVisible(false);
  m_textInput->setStyleSheet("QLineEdit {"
                             "  background-color: white;"
                             "  border: 2px solid #0078d4;"
                             "  border-radius: 3px;"
                             "  padding: 5px;"
                             "  font-size: 14px;"
                             "  font-family: Arial, sans-serif;"
                             "}"
                             "QLineEdit:focus {"
                             "  border-color: #005a9e;"
                             "}");

  // Connecter le signal de validation
  connect(m_textInput, &QLineEdit::editingFinished, this,
          &TextTool::onTextEntered);
  connect(m_textInput, &QLineEdit::returnPressed, this,
          &TextTool::onTextEntered);
}

void TextTool::onMousePress(const QPoint &pt) {
  if (!m_isEditing) {
    m_currentPosition = pt;
    showTextInput(pt);
  }
}

void TextTool::onMouseMove(const QPoint &pt) {
  // Pas d'action pendant le mouvement pour l'outil texte
}

void TextTool::onMouseRelease(const QPoint &pt) {
  // Pas d'action au relâchement pour l'outil texte
}

void TextTool::showTextInput(const QPoint &position) {
  if (m_isEditing) {
    return; // Éviter les doublons
  }

  m_isEditing = true;
  m_currentPosition = position;

  // Positionner le champ de saisie
  QWidget *parentWidget = qobject_cast<QWidget *>(parent());
  if (parentWidget) {
    QPoint globalPos = parentWidget->mapToGlobal(position);
    m_textInput->move(globalPos);
    m_textInput->clear();
    m_textInput->setVisible(true);
    m_textInput->setFocus();
  }
}

void TextTool::hideTextInput() {
  m_textInput->setVisible(false);
  m_textInput->clear();
  m_isEditing = false;
}

void TextTool::onTextEntered() {
  QString text = m_textInput->text().trimmed();

  if (!text.isEmpty()) {
    // Créer un élément de texte
    TextElement textElement;
    textElement.id = m_userId;
    textElement.position = m_currentPosition;
    textElement.text = text;
    textElement.font = QFont("Arial", 14);

    // Ajouter à la liste locale
    m_textElements.append(textElement);

    // Envoyer au serveur
    emitTextJson(m_currentPosition, text);
  }

  hideTextInput();
}

void TextTool::emitTextJson(const QPoint &position, const QString &text) {
  QJsonObject msg;
  msg["id"] = m_userId;
  msg["type"] = "object";
  QJsonObject props;
  props["action"] = "add_text";

  QJsonObject textObj;
  textObj["x"] = position.x();
  textObj["y"] = position.y();
  textObj["text"] = text;
  textObj["fontSize"] = 14;
  textObj["fontFamily"] = "Arial";
  props["text"] = textObj;
  msg["props"] = props;

  emit sendJson(msg);
}

void TextTool::draw(QPainter &p) {
  // Dessiner tous les éléments de texte
  for (const TextElement &element : m_textElements) {
    p.setFont(element.font);
    p.setPen(Qt::black);
    p.drawText(element.position, element.text);
  }
}

void TextTool::onRemoteJson(const QJsonObject &msg) {
  QString userId = msg["id"].toString();
  auto props = msg["props"].toObject();
  QString action = props["action"].toString();

  if (action == "add_text") {
    if (props.contains("text")) {
      QJsonObject textObj = props["text"].toObject();
      QPoint position(textObj["x"].toInt(), textObj["y"].toInt());
      QString text = textObj["text"].toString();
      int fontSize = textObj["fontSize"].toInt(14);
      QString fontFamily = textObj["fontFamily"].toString("Arial");

      TextElement textElement;
      textElement.id = userId;
      textElement.position = position;
      textElement.text = text;
      textElement.font = QFont(fontFamily, fontSize);

      m_textElements.append(textElement);
    }
  }
}

void TextTool::clear() {
  m_textElements.clear();
  hideTextInput();
}