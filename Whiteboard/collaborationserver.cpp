#include "collaborationserver.h"
#include <QJsonDocument>
#include <QJsonArray>

CollaborationServer::CollaborationServer(quint16 listenPort, QObject* parent)
    : QObject(parent)
    , m_listenPort(listenPort)
    , m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection,
            this, &CollaborationServer::onNewConnection);
    m_server->listen(QHostAddress::Any, listenPort);
}

void CollaborationServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* client = m_server->nextPendingConnection();
        connect(client, &QTcpSocket::readyRead,
                this, &CollaborationServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected,
                this, &CollaborationServer::onDisconnected);
        m_clients << client;
        broadcastClientList();
    }
}

void CollaborationServer::onReadyRead()
{
    QTcpSocket* senderSock = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = senderSock->readAll();
    auto doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    // relay to all other clients
    broadcast(obj, senderSock);
}

void CollaborationServer::onDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    m_clients.removeAll(client);
    client->deleteLater();
    broadcastClientList();
}

void CollaborationServer::broadcast(const QJsonObject& msg, QTcpSocket* exclude)
{
    QJsonDocument doc(msg);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);
    for (QTcpSocket* client : m_clients) {
        if (client != exclude) client->write(bytes);
    }
}

void CollaborationServer::broadcastClientList()
{
    QJsonArray arr;
    for (QTcpSocket* client : m_clients) {
        QJsonObject info;
        info["ip"] = client->peerAddress().toString();
        info["port"] = (qint64)m_listenPort;
        arr.append(info);
    }
    QJsonObject msg;
    msg["type"] = "clientList";
    msg["clients"] = arr;
    broadcast(msg);
}
