#include "userlistpanel.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>

UserListPanel::UserListPanel(QWidget* parent)
    : QWidget(parent) {
    
    setupUi();
}

UserListPanel::~UserListPanel() {
}

void UserListPanel::setupUi() {
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create header label
    QLabel* headerLabel = new QLabel("Connected Users", this);
    headerLabel->setStyleSheet("font-weight: bold;");
    
    // Create list widget
    m_userList = new QListWidget(this);
    
    // Add widgets to layout
    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(m_userList);
    
    // Connect signals
    connect(m_userList, &QListWidget::itemClicked, this, &UserListPanel::onItemClicked);
}

QUuid UserListPanel::addUser(const QString& username, bool isAdmin) {
    // Create new user ID
    QUuid userId = QUuid::createUuid();
    
    // Create list item
    QListWidgetItem* item = new QListWidgetItem(m_userList);
    
    // Set display text
    QString displayText = username;
    if (isAdmin) {
        displayText += " (Admin)";
    }
    item->setText(displayText);
    
    // Store user data
    item->setData(Qt::UserRole, userId.toString());
    
    // Add to map
    m_userItems[userId] = item;
    
    return userId;
}

bool UserListPanel::removeUser(const QUuid& userId) {
    if (m_userItems.contains(userId)) {
        // Get item
        QListWidgetItem* item = m_userItems[userId];
        
        // Remove from list
        m_userList->takeItem(m_userList->row(item));
        
        // Delete item
        delete item;
        
        // Remove from map
        m_userItems.remove(userId);
        
        return true;
    }
    
    return false;
}

bool UserListPanel::updateUserStatus(const QUuid& userId, bool isAdmin) {
    if (m_userItems.contains(userId)) {
        // Get item
        QListWidgetItem* item = m_userItems[userId];
        
        // Get current text
        QString text = item->text();
        
        // Remove admin suffix if present
        if (text.endsWith(" (Admin)")) {
            text = text.left(text.length() - 8);
        }
        
        // Add admin suffix if needed
        if (isAdmin) {
            text += " (Admin)";
        }
        
        // Update text
        item->setText(text);
        
        return true;
    }
    
    return false;
}

void UserListPanel::clear() {
    // Clear list
    m_userList->clear();
    
    // Clear map
    m_userItems.clear();
}

void UserListPanel::onItemClicked(QListWidgetItem* item) {
    if (item) {
        // Get user ID
        QString userIdStr = item->data(Qt::UserRole).toString();
        QUuid userId(userIdStr);
        
        // Emit signal
        emit userSelected(userId);
    }
}
