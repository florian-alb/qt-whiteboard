#ifndef USERLISTPANEL_H
#define USERLISTPANEL_H

#include <QWidget>
#include <QUuid>
#include <QMap>

class QListWidget;
class QListWidgetItem;

/**
 * @brief Panel for displaying connected users
 */
class UserListPanel : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    UserListPanel(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~UserListPanel();
    
    /**
     * @brief Adds a user to the list
     * @param username Username to add
     * @param isAdmin Whether the user is an admin
     * @return User ID
     */
    QUuid addUser(const QString& username, bool isAdmin = false);
    
    /**
     * @brief Removes a user from the list
     * @param userId ID of the user to remove
     * @return True if the user was removed, false otherwise
     */
    bool removeUser(const QUuid& userId);
    
    /**
     * @brief Updates a user's status
     * @param userId ID of the user to update
     * @param isAdmin New admin status
     * @return True if the user was updated, false otherwise
     */
    bool updateUserStatus(const QUuid& userId, bool isAdmin);
    
    /**
     * @brief Clears the user list
     */
    void clear();

signals:
    /**
     * @brief Signal emitted when a user is selected
     * @param userId ID of the selected user
     */
    void userSelected(const QUuid& userId);

private slots:
    void onItemClicked(QListWidgetItem* item);

private:
    QListWidget* m_userList;
    QMap<QUuid, QListWidgetItem*> m_userItems;
    
    void setupUi();
};

#endif // USERLISTPANEL_H
