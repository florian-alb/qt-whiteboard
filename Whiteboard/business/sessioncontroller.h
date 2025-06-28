#ifndef SESSIONCONTROLLER_H
#define SESSIONCONTROLLER_H

#include "../data/session.h"
#include "../data/user.h"
#include "syncmanager.h"
#include <QObject>
#include <memory>

/**
 * @brief Controller class for session operations
 */
class SessionController : public QObject {
    Q_OBJECT

public:
    enum SessionRole {
        Host,
        Client
    };
    
    enum ConnectionStatus {
        Disconnected,
        Connecting,
        Connected,
        Error
    };

    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    SessionController(QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~SessionController();
    
    /**
     * @brief Creates a new session
     * @param sessionName Name of the session
     * @param username Name of the host user
     * @return True if the session was created successfully, false otherwise
     */
    bool createSession(const QString& sessionName, const QString& username);
    
    /**
     * @brief Joins an existing session
     * @param sessionCode Code of the session to join
     * @param username Name of the client user
     * @return True if the join request was sent successfully, false otherwise
     */
    bool joinSession(const QString& sessionCode, const QString& username);
    
    /**
     * @brief Leaves the current session
     * @return True if the session was left successfully, false otherwise
     */
    bool leaveSession();
    
    /**
     * @brief Gets the current user
     * @return Pointer to the current user
     */
    User* getCurrentUser() const { return m_currentUser.get(); }
    
    /**
     * @brief Gets the current session
     * @return Pointer to the current session
     */
    Session* getSession() const { return m_session.get(); }
    
    /**
     * @brief Gets the current role
     * @return Current role (host or client)
     */
    SessionRole getRole() const { return m_role; }
    
    /**
     * @brief Gets the current connection status
     * @return Current connection status
     */
    ConnectionStatus getConnectionStatus() const { return m_connectionStatus; }
    
    /**
     * @brief Gets the session code
     * @return Session code if a session is active, empty string otherwise
     */
    QString getSessionCode() const;
    
    /**
     * @brief Sets the sync manager
     * @param syncManager Pointer to the sync manager
     */
    void setSyncManager(SyncManager* syncManager) { m_syncManager = syncManager; }

signals:
    /**
     * @brief Signal emitted when the session status changes
     * @param status New connection status
     * @param message Status message
     */
    void connectionStatusChanged(ConnectionStatus status, const QString& message);
    
    /**
     * @brief Signal emitted when a user joins the session
     * @param userId ID of the user who joined
     * @param username Name of the user who joined
     */
    void userJoined(const QUuid& userId, const QString& username);
    
    /**
     * @brief Signal emitted when a user leaves the session
     * @param userId ID of the user who left
     * @param username Name of the user who left
     */
    void userLeft(const QUuid& userId, const QString& username);

private:
    std::unique_ptr<Session> m_session;
    std::unique_ptr<User> m_currentUser;
    SessionRole m_role;
    ConnectionStatus m_connectionStatus;
    SyncManager* m_syncManager;
    QString m_lastErrorMessage;
    
    void setConnectionStatus(ConnectionStatus status, const QString& message = "");
};

#endif // SESSIONCONTROLLER_H
