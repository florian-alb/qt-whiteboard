#include "stylepanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFontDatabase>
#include <QColorDialog>

StylePanel::StylePanel(QWidget* parent)
    : QWidget(parent), m_currentColor(Qt::black) {
    
    setupUi();
}

StylePanel::~StylePanel() {
}

void StylePanel::setupUi() {
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Font family
    QHBoxLayout* fontFamilyLayout = new QHBoxLayout();
    QLabel* fontFamilyLabel = new QLabel("Font:", this);
    m_fontFamilyCombo = new QComboBox(this);
    
    // Add available fonts
    QFontDatabase fontDatabase;
    const QStringList fontFamilies = fontDatabase.families();
    m_fontFamilyCombo->addItems(fontFamilies);
    
    // Set default font
    int index = m_fontFamilyCombo->findText("Arial");
    if (index != -1) {
        m_fontFamilyCombo->setCurrentIndex(index);
    }
    
    fontFamilyLayout->addWidget(fontFamilyLabel);
    fontFamilyLayout->addWidget(m_fontFamilyCombo);
    mainLayout->addLayout(fontFamilyLayout);
    
    // Font size
    QHBoxLayout* fontSizeLayout = new QHBoxLayout();
    QLabel* fontSizeLabel = new QLabel("Size:", this);
    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setRange(8, 72);
    m_fontSizeSpinBox->setValue(12);
    
    fontSizeLayout->addWidget(fontSizeLabel);
    fontSizeLayout->addWidget(m_fontSizeSpinBox);
    mainLayout->addLayout(fontSizeLayout);
    
    // Font style
    QHBoxLayout* fontStyleLayout = new QHBoxLayout();
    m_boldCheckBox = new QCheckBox("Bold", this);
    m_italicCheckBox = new QCheckBox("Italic", this);
    
    fontStyleLayout->addWidget(m_boldCheckBox);
    fontStyleLayout->addWidget(m_italicCheckBox);
    mainLayout->addLayout(fontStyleLayout);
    
    // Color
    QHBoxLayout* colorLayout = new QHBoxLayout();
    QLabel* colorLabel = new QLabel("Color:", this);
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(32, 32);
    
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(m_colorButton);
    mainLayout->addLayout(colorLayout);
    
    // Add stretch to push everything to the top
    mainLayout->addStretch();
    
    // Update color button
    updateColorButton();
    
    // Connect signals
    connect(m_fontFamilyCombo, &QComboBox::currentTextChanged, this, &StylePanel::onFontFamilyChanged);
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &StylePanel::onFontSizeChanged);
    connect(m_boldCheckBox, &QCheckBox::toggled, this, &StylePanel::onBoldToggled);
    connect(m_italicCheckBox, &QCheckBox::toggled, this, &StylePanel::onItalicToggled);
    connect(m_colorButton, &QPushButton::clicked, this, &StylePanel::onColorButtonClicked);
}

void StylePanel::setTextProperties(const QFont& font, const QColor& color) {
    // Block signals to prevent feedback loops
    m_fontFamilyCombo->blockSignals(true);
    m_fontSizeSpinBox->blockSignals(true);
    m_boldCheckBox->blockSignals(true);
    m_italicCheckBox->blockSignals(true);
    
    // Update font controls
    int index = m_fontFamilyCombo->findText(font.family());
    if (index != -1) {
        m_fontFamilyCombo->setCurrentIndex(index);
    }
    
    m_fontSizeSpinBox->setValue(font.pointSize());
    m_boldCheckBox->setChecked(font.bold());
    m_italicCheckBox->setChecked(font.italic());
    
    // Update color
    m_currentColor = color;
    updateColorButton();
    
    // Unblock signals
    m_fontFamilyCombo->blockSignals(false);
    m_fontSizeSpinBox->blockSignals(false);
    m_boldCheckBox->blockSignals(false);
    m_italicCheckBox->blockSignals(false);
}

void StylePanel::onFontFamilyChanged(const QString& family) {
    QFont font;
    font.setFamily(family);
    font.setPointSize(m_fontSizeSpinBox->value());
    font.setBold(m_boldCheckBox->isChecked());
    font.setItalic(m_italicCheckBox->isChecked());
    
    emit fontChanged(font);
}

void StylePanel::onFontSizeChanged(int size) {
    QFont font;
    font.setFamily(m_fontFamilyCombo->currentText());
    font.setPointSize(size);
    font.setBold(m_boldCheckBox->isChecked());
    font.setItalic(m_italicCheckBox->isChecked());
    
    emit fontChanged(font);
}

void StylePanel::onBoldToggled(bool checked) {
    QFont font;
    font.setFamily(m_fontFamilyCombo->currentText());
    font.setPointSize(m_fontSizeSpinBox->value());
    font.setBold(checked);
    font.setItalic(m_italicCheckBox->isChecked());
    
    emit fontChanged(font);
}

void StylePanel::onItalicToggled(bool checked) {
    QFont font;
    font.setFamily(m_fontFamilyCombo->currentText());
    font.setPointSize(m_fontSizeSpinBox->value());
    font.setBold(m_boldCheckBox->isChecked());
    font.setItalic(checked);
    
    emit fontChanged(font);
}

void StylePanel::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(m_currentColor, this, "Select Color");
    
    if (color.isValid()) {
        m_currentColor = color;
        updateColorButton();
        emit colorChanged(color);
    }
}

void StylePanel::updateColorButton() {
    // Set button background color
    QString styleSheet = QString("background-color: %1;").arg(m_currentColor.name());
    m_colorButton->setStyleSheet(styleSheet);
}
