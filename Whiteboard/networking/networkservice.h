#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QUuid>
#include <QJsonObject>

/**
 * @brief Service class for network communication
 */
class NetworkService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    NetworkService(QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~NetworkService();
    
    /**
     * @brief Starts the server for hosting a session
     * @return True if the server started successfully, false otherwise
     */
    bool startServer();
    
    /**
     * @brief Connects to a server for joining a session
     * @param sessionCode Code of the session to join
     * @return True if the connection attempt was started successfully, false otherwise
     */
    bool connectToServer(const QString& sessionCode);
    
    /**
     * @brief Disconnects from the current session
     * @return True if disconnected successfully, false otherwise
     */
    bool disconnect();
    
    /**
     * @brief Sends a message to all connected clients (if host) or to the server (if client)
     * @param message Message to send
     * @return True if the message was sent successfully, false otherwise
     */
    bool sendMessage(const QJsonObject& message);
    
    /**
     * @brief Sends a message to a specific client (host only)
     * @param clientId ID of the client to send the message to
     * @param message Message to send
     * @return True if the message was sent successfully, false otherwise
     */
    bool sendMessageToClient(const QUuid& clientId, const QJsonObject& message);

signals:
    /**
     * @brief Signal emitted when a message is received
     * @param message Received message
     */
    void messageReceived(const QJsonObject& message);
    
    /**
     * @brief Signal emitted when the connection status changes
     * @param connected True if connected, false otherwise
     * @param message Status message
     */
    void connectionStatusChanged(bool connected, const QString& message);
    
    /**
     * @brief Signal emitted when a client connects
     * @param clientId ID of the connected client
     */
    void clientConnected(const QUuid& clientId);
    
    /**
     * @brief Signal emitted when a client disconnects
     * @param clientId ID of the disconnected client
     */
    void clientDisconnected(const QUuid& clientId);

private slots:
    void onNewConnection();
    void onSocketDisconnected();
    void onReadyRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer* m_server;
    QTcpSocket* m_clientSocket; // Used when in client mode
    QMap<QUuid, QTcpSocket*> m_clientSockets; // Used when in server mode
    QMap<QTcpSocket*, QUuid> m_socketToClientId;
    bool m_isServer;
    
    void processMessage(QTcpSocket* socket, const QByteArray& data);
    QByteArray serializeMessage(const QJsonObject& message);
    QJsonObject deserializeMessage(const QByteArray& data);
};

#endif // NETWORKSERVICE_H
