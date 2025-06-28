#include "networkservice.h"
#include <QJsonDocument>
#include <QNetworkInterface>

// Default port for the whiteboard application
const quint16 DEFAULT_PORT = 45678;

NetworkService::NetworkService(QObject* parent)
    : QObject(parent), m_server(nullptr), m_clientSocket(nullptr), m_isServer(false) {
}

NetworkService::~NetworkService() {
    disconnect();
}

bool NetworkService::startServer() {
    // Clean up any existing connections
    disconnect();
    
    // Create and start the server
    m_server = new QTcpServer(this);
    
    connect(m_server, &QTcpServer::newConnection, this, &NetworkService::onNewConnection);
    
    if (!m_server->listen(QHostAddress::Any, DEFAULT_PORT)) {
        emit connectionStatusChanged(false, "Failed to start server: " + m_server->errorString());
        return false;
    }
    
    m_isServer = true;
    emit connectionStatusChanged(true, "Server started on port " + QString::number(DEFAULT_PORT));
    return true;
}

bool NetworkService::connectToServer(const QString& sessionCode) {
    // Clean up any existing connections
    disconnect();
    
    // Create and connect the client socket
    m_clientSocket = new QTcpSocket(this);
    
    connect(m_clientSocket, &QTcpSocket::connected, this, &NetworkService::onConnected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &NetworkService::onSocketDisconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &NetworkService::onReadyRead);
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &NetworkService::onError);
    
    // For this simple POC, we'll connect to localhost
    // In a real application, we would resolve the session code to an IP address
    m_clientSocket->connectToHost("localhost", DEFAULT_PORT);
    
    m_isServer = false;
    emit connectionStatusChanged(false, "Connecting to server...");
    return true;
}

bool NetworkService::disconnect() {
    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
    
    if (m_clientSocket) {
        m_clientSocket->disconnectFromHost();
        delete m_clientSocket;
        m_clientSocket = nullptr;
    }
    
    // Close all client connections if we're a server
    for (auto socket : m_clientSockets.values()) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    
    m_clientSockets.clear();
    m_socketToClientId.clear();
    
    m_isServer = false;
    emit connectionStatusChanged(false, "Disconnected");
    return true;
}

bool NetworkService::sendMessage(const QJsonObject& message) {
    if (m_isServer) {
        // Send to all clients
        bool success = true;
        for (auto socket : m_clientSockets.values()) {
            QByteArray data = serializeMessage(message);
            if (socket->write(data) == -1) {
                success = false;
            }
        }
        return success;
    }
    else {
        // Send to server
        if (m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState) {
            QByteArray data = serializeMessage(message);
            return m_clientSocket->write(data) != -1;
        }
    }
    
    return false;
}

bool NetworkService::sendMessageToClient(const QUuid& clientId, const QJsonObject& message) {
    if (!m_isServer) {
        return false; // Only the server can send messages to specific clients
    }
    
    if (m_clientSockets.contains(clientId)) {
        QTcpSocket* socket = m_clientSockets[clientId];
        QByteArray data = serializeMessage(message);
        return socket->write(data) != -1;
    }
    
    return false;
}

void NetworkService::onNewConnection() {
    QTcpSocket* socket = m_server->nextPendingConnection();
    
    if (!socket) {
        return;
    }
    
    // Generate a client ID
    QUuid clientId = QUuid::createUuid();
    
    // Add the client to our maps
    m_clientSockets[clientId] = socket;
    m_socketToClientId[socket] = clientId;
    
    // Connect socket signals
    connect(socket, &QTcpSocket::disconnected, this, &NetworkService::onSocketDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkService::onReadyRead);
    
    emit clientConnected(clientId);
}

void NetworkService::onSocketDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    
    if (!socket) {
        return;
    }
    
    if (m_isServer) {
        // A client disconnected
        if (m_socketToClientId.contains(socket)) {
            QUuid clientId = m_socketToClientId[socket];
            m_clientSockets.remove(clientId);
            m_socketToClientId.remove(socket);
            
            emit clientDisconnected(clientId);
        }
        
        socket->deleteLater();
    }
    else {
        // We disconnected from the server
        emit connectionStatusChanged(false, "Disconnected from server");
    }
}

void NetworkService::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    
    if (!socket) {
        return;
    }
    
    // Read all available data
    QByteArray data = socket->readAll();
    
    // Process the message
    processMessage(socket, data);
}

void NetworkService::onConnected() {
    emit connectionStatusChanged(true, "Connected to server");
}

void NetworkService::onError(QAbstractSocket::SocketError socketError) {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    
    if (!socket) {
        return;
    }
    
    emit connectionStatusChanged(false, "Socket error: " + socket->errorString());
}

void NetworkService::processMessage(QTcpSocket* socket, const QByteArray& data) {
    // Deserialize the message
    QJsonObject message = deserializeMessage(data);
    
    if (message.isEmpty()) {
        return;
    }
    
    // Emit the message received signal
    emit messageReceived(message);
}

QByteArray NetworkService::serializeMessage(const QJsonObject& message) {
    QJsonDocument doc(message);
    return doc.toJson(QJsonDocument::Compact);
}

QJsonObject NetworkService::deserializeMessage(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isObject()) {
        return doc.object();
    }
    
    return QJsonObject();
}
