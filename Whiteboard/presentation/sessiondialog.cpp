#include "sessiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>

SessionDialog::SessionDialog(QWidget* parent)
    : QDialog(parent) {
    
    setWindowTitle("Whiteboard Session");
    setModal(true);
    setMinimumWidth(400);
    
    setupUi();
}

SessionDialog::~SessionDialog() {
}

void SessionDialog::setupUi() {
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Username section
    QHBoxLayout* usernameLayout = new QHBoxLayout();
    QLabel* usernameLabel = new QLabel("Username:", this);
    m_usernameEdit = new QLineEdit(this);
    
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(m_usernameEdit);
    mainLayout->addLayout(usernameLayout);
    
    // Session type selection
    QHBoxLayout* sessionTypeLayout = new QHBoxLayout();
    m_createRadio = new QRadioButton("Create Session", this);
    m_joinRadio = new QRadioButton("Join Session", this);
    
    // Group radio buttons
    QButtonGroup* sessionTypeGroup = new QButtonGroup(this);
    sessionTypeGroup->addButton(m_createRadio);
    sessionTypeGroup->addButton(m_joinRadio);
    
    // Set default selection
    m_createRadio->setChecked(true);
    
    sessionTypeLayout->addWidget(m_createRadio);
    sessionTypeLayout->addWidget(m_joinRadio);
    mainLayout->addLayout(sessionTypeLayout);
    
    // Session details
    QGridLayout* sessionDetailsLayout = new QGridLayout();
    m_sessionNameLabel = new QLabel("Session Name:", this);
    m_sessionNameEdit = new QLineEdit(this);
    m_sessionCodeLabel = new QLabel("Session Code:", this);
    m_sessionCodeEdit = new QLineEdit(this);
    
    sessionDetailsLayout->addWidget(m_sessionNameLabel, 0, 0);
    sessionDetailsLayout->addWidget(m_sessionNameEdit, 0, 1);
    sessionDetailsLayout->addWidget(m_sessionCodeLabel, 1, 0);
    sessionDetailsLayout->addWidget(m_sessionCodeEdit, 1, 1);
    mainLayout->addLayout(sessionDetailsLayout);
    
    // Buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    m_createButton = new QPushButton("Create", this);
    m_joinButton = new QPushButton("Join", this);
    m_cancelButton = new QPushButton("Cancel", this);
    
    buttonsLayout->addWidget(m_createButton);
    buttonsLayout->addWidget(m_joinButton);
    buttonsLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonsLayout);
    
    // Connect signals
    connect(m_createRadio, &QRadioButton::toggled, this, &SessionDialog::onCreateRadioToggled);
    connect(m_joinRadio, &QRadioButton::toggled, this, &SessionDialog::onJoinRadioToggled);
    connect(m_createButton, &QPushButton::clicked, this, &SessionDialog::onCreateButtonClicked);
    connect(m_joinButton, &QPushButton::clicked, this, &SessionDialog::onJoinButtonClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SessionDialog::onCancelButtonClicked);
    
    // Update UI based on initial selection
    updateUi();
}

void SessionDialog::updateUi() {
    bool createMode = m_createRadio->isChecked();
    
    // Show/hide session name and code fields
    m_sessionNameLabel->setVisible(createMode);
    m_sessionNameEdit->setVisible(createMode);
    m_sessionCodeLabel->setVisible(!createMode);
    m_sessionCodeEdit->setVisible(!createMode);
    
    // Enable/disable buttons
    m_createButton->setVisible(createMode);
    m_joinButton->setVisible(!createMode);
}

QString SessionDialog::getUsername() const {
    return m_usernameEdit->text();
}

QString SessionDialog::getSessionName() const {
    return m_sessionNameEdit->text();
}

QString SessionDialog::getSessionCode() const {
    return m_sessionCodeEdit->text();
}

void SessionDialog::onCreateRadioToggled(bool checked) {
    if (checked) {
        updateUi();
    }
}

void SessionDialog::onJoinRadioToggled(bool checked) {
    if (checked) {
        updateUi();
    }
}

void SessionDialog::onCreateButtonClicked() {
    emit createSessionClicked();
    accept();
}

void SessionDialog::onJoinButtonClicked() {
    emit joinSessionClicked();
    accept();
}

void SessionDialog::onCancelButtonClicked() {
    reject();
}
