#include "WhiteboardServer.h"
#include <QDebug>

WhiteboardServer::WhiteboardServer(quint16 port, QObject *parent)
    : QObject(parent),
      m_server(QStringLiteral("WhiteboardServer"), QWebSocketServer::NonSecureMode, this)
{
    if (!m_server.listen(QHostAddress::Any, port))
    {
        qFatal("Failed to start server");
    }
    connect(&m_server, &QWebSocketServer::newConnection,
            this, &WhiteboardServer::onNewConnection);
    qDebug() << "Server listening on port" << port;
}

void WhiteboardServer::onNewConnection()
{
    QWebSocket *socket = m_server.nextPendingConnection();
    m_clients << socket;
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WhiteboardServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WhiteboardServer::socketDisconnected);
}

void WhiteboardServer::onTextMessageReceived(const QString &msg)
{
    // Broadcast to everyone
    for (auto *client : m_clients)
    {
        client->sendTextMessage(msg);
    }
}

void WhiteboardServer::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    m_clients.removeAll(client);
    client->deleteLater();
}
