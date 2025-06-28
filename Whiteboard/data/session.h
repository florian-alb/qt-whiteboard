#ifndef SESSION_H
#define SESSION_H

#include "user.h"
#include <QString>
#include <QUuid>
#include <QList>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Represents a whiteboard session with multiple users
 */
class Session {
public:
    /**
     * @brief Constructor
     * @param name Session name
     */
    Session(const QString& name = "New Session");
    
    /**
     * @brief Gets the session ID
     * @return Session ID
     */
    QUuid getId() const { return m_id; }
    
    /**
     * @brief Gets the session name
     * @return Session name
     */
    QString getName() const { return m_name; }
    
    /**
     * @brief Sets the session name
     * @param name New session name
     */
    void setName(const QString& name) { m_name = name; }
    
    /**
     * @brief Gets the session creation time
     * @return Creation time
     */
    QDateTime getCreationTime() const { return m_creationTime; }
    
    /**
     * @brief Gets the list of users in the session
     * @return List of users
     */
    QList<User> getUsers() const { return m_users; }
    
    /**
     * @brief Adds a user to the session
     * @param user User to add
     */
    void addUser(const User& user);
    
    /**
     * @brief Removes a user from the session
     * @param userId ID of the user to remove
     * @return True if the user was removed, false otherwise
     */
    bool removeUser(const QUuid& userId);
    
    /**
     * @brief Finds a user in the session
     * @param userId ID of the user to find
     * @return Pointer to the user if found, nullptr otherwise
     */
    User* findUser(const QUuid& userId);
    
    /**
     * @brief Serializes the session to JSON
     * @return JSON object representing the session
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Deserializes the session from JSON
     * @param json JSON object to deserialize from
     */
    void fromJson(const QJsonObject& json);
    
    /**
     * @brief Gets the session code for sharing
     * @return Session code
     */
    QString getSessionCode() const;

private:
    QUuid m_id;
    QString m_name;
    QDateTime m_creationTime;
    QList<User> m_users;
};

#endif // SESSION_H
