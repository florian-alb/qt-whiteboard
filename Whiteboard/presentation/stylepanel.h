#ifndef STYLEPANEL_H
#define STYLEPANEL_H

#include <QWidget>
#include <QFont>
#include <QColor>

class QComboBox;
class QSpinBox;
class QPushButton;
class QCheckBox;

/**
 * @brief Panel for styling whiteboard elements
 */
class StylePanel : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    StylePanel(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~StylePanel();
    
    /**
     * @brief Sets the text properties in the panel
     * @param font Font to set
     * @param color Color to set
     */
    void setTextProperties(const QFont& font, const QColor& color);

signals:
    /**
     * @brief Signal emitted when the font is changed
     * @param font New font
     */
    void fontChanged(const QFont& font);
    
    /**
     * @brief Signal emitted when the color is changed
     * @param color New color
     */
    void colorChanged(const QColor& color);

private slots:
    void onFontFamilyChanged(const QString& family);
    void onFontSizeChanged(int size);
    void onBoldToggled(bool checked);
    void onItalicToggled(bool checked);
    void onColorButtonClicked();

private:
    QComboBox* m_fontFamilyCombo;
    QSpinBox* m_fontSizeSpinBox;
    QCheckBox* m_boldCheckBox;
    QCheckBox* m_italicCheckBox;
    QPushButton* m_colorButton;
    QColor m_currentColor;
    
    void setupUi();
    void updateColorButton();
};

#endif // STYLEPANEL_H
