#ifndef USER_H
#define USER_H

#include <QString>
#include <QUuid>
#include <QJsonObject>

/**
 * @brief Represents a user in the whiteboard session
 */
class User {
public:
    /**
     * @brief Constructor
     * @param username User's display name
     * @param isAdmin Whether the user is an admin
     */
    User(const QString& username = "", bool isAdmin = false);
    
    /**
     * @brief Gets the user's unique ID
     * @return User ID
     */
    QUuid getId() const { return m_id; }
    
    /**
     * @brief Gets the user's username
     * @return Username
     */
    QString getUsername() const { return m_username; }
    
    /**
     * @brief Sets the user's username
     * @param username New username
     */
    void setUsername(const QString& username) { m_username = username; }
    
    /**
     * @brief Checks if the user is an admin
     * @return True if admin, false otherwise
     */
    bool isAdmin() const { return m_isAdmin; }
    
    /**
     * @brief Sets the user's admin status
     * @param isAdmin New admin status
     */
    void setAdmin(bool isAdmin) { m_isAdmin = isAdmin; }
    
    /**
     * @brief Serializes the user to JSON
     * @return JSON object representing the user
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Deserializes the user from JSON
     * @param json JSON object to deserialize from
     */
    void fromJson(const QJsonObject& json);

private:
    QUuid m_id;
    QString m_username;
    bool m_isAdmin;
};

#endif // USER_H
