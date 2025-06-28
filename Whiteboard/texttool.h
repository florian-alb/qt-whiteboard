#pragma once
#include "tool.h"
#include <QLineEdit>
#include <QPoint>
#include <QVector>

class TextTool : public Tool {
  Q_OBJECT
public:
  explicit TextTool(const QString &userId, QObject *parent = nullptr);
  void onMousePress(const QPoint &pt) override;
  void onMouseMove(const QPoint &pt) override;
  void onMouseRelease(const QPoint &pt) override;
  void draw(QPainter &p) override;
  void onRemoteJson(const QJsonObject &msg) override;
  void clear() override;

private slots:
  void onTextEntered();

private:
  QString m_userId;
  struct TextElement {
    QString id;
    QPoint position;
    QString text;
    QFont font;
  };

  QVector<TextElement> m_textElements;
  QLineEdit *m_textInput;
  QPoint m_currentPosition;
  bool m_isEditing = false;

  void emitTextJson(const QPoint &position, const QString &text);
  void showTextInput(const QPoint &position);
  void hideTextInput();
};