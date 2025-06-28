#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include "../data/whiteboardelement.h"
#include "../data/session.h"
#include "../data/user.h"
#include "../networking/networkservice.h"
#include <QObject>
#include <memory>

/**
 * @brief Manager class for synchronizing whiteboard elements across the network
 */
class SyncManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    SyncManager(QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~SyncManager();
    
    /**
     * @brief Starts hosting a session
     * @param session Session to host
     * @param hostUser Host user
     * @return True if hosting started successfully, false otherwise
     */
    bool startHosting(Session* session, User* hostUser);
    
    /**
     * @brief Joins a session
     * @param sessionCode Code of the session to join
     * @param clientUser Client user
     * @return True if join request was sent successfully, false otherwise
     */
    bool joinSession(const QString& sessionCode, User* clientUser);
    
    /**
     * @brief Leaves the current session
     * @return True if the session was left successfully, false otherwise
     */
    bool leaveSession();
    
    /**
     * @brief Synchronizes a new element with other clients
     * @param element Element to synchronize
     */
    void syncElementAdded(std::shared_ptr<WhiteboardElement> element);
    
    /**
     * @brief Synchronizes element removal with other clients
     * @param elementId ID of the element to remove
     */
    void syncElementRemoved(const QUuid& elementId);
    
    /**
     * @brief Synchronizes element modification with other clients
     * @param element Modified element
     */
    void syncElementModified(std::shared_ptr<WhiteboardElement> element);
    
    /**
     * @brief Synchronizes the entire whiteboard model with a new client
     * @param clientId ID of the client to synchronize with
     */
    void syncFullModel(const QUuid& clientId);

signals:
    /**
     * @brief Signal emitted when a new element is received from the network
     * @param element Received element
     */
    void elementReceived(std::shared_ptr<WhiteboardElement> element);
    
    /**
     * @brief Signal emitted when an element removal is received from the network
     * @param elementId ID of the element to remove
     */
    void elementRemovalReceived(const QUuid& elementId);
    
    /**
     * @brief Signal emitted when an element modification is received from the network
     * @param element Modified element
     */
    void elementModificationReceived(std::shared_ptr<WhiteboardElement> element);
    
    /**
     * @brief Signal emitted when a user joins the session
     * @param user User who joined
     */
    void userJoined(const User& user);
    
    /**
     * @brief Signal emitted when a user leaves the session
     * @param userId ID of the user who left
     */
    void userLeft(const QUuid& userId);
    
    /**
     * @brief Signal emitted when the connection status changes
     * @param connected True if connected, false otherwise
     * @param message Status message
     */
    void connectionStatusChanged(bool connected, const QString& message);

private:
    NetworkService* m_networkService;
    Session* m_session;
    User* m_currentUser;
    
    void handleNetworkMessage(const QJsonObject& message);
    void sendMessage(const QJsonObject& message);
};

#endif // SYNCMANAGER_H
