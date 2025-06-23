#include "CollaborationClient.h"
#include <QJsonDocument>
#include <QHostAddress>
#include <QJsonValue>

CollaborationClient::CollaborationClient(quint16 listenPort, QObject* parent)
    : QObject(parent), m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &CollaborationClient::onNewConnection);
    m_server->listen(QHostAddress::Any, listenPort);
}

void CollaborationClient::connectToPeer(const QHostAddress& ip, quint16 port)
{
    QTcpSocket* sock = new QTcpSocket(this);
    connect(sock, &QTcpSocket::readyRead, this, &CollaborationClient::onReadyRead);
    connect(sock, &QTcpSocket::disconnected, this, &CollaborationClient::onDisconnected);
    sock->connectToHost(ip, port);
    if (sock->waitForConnected(2000)) {
        m_peers << sock;
    } else {
        sock->deleteLater();
    }
}

void CollaborationClient::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* sock = m_server->nextPendingConnection();
        connect(sock, &QTcpSocket::readyRead, this, &CollaborationClient::onReadyRead);
        connect(sock, &QTcpSocket::disconnected, this, &CollaborationClient::onDisconnected);
        m_peers << sock;
    }
}

void CollaborationClient::onReadyRead()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = sock->readAll();
    auto doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) handleMessage(doc.object());
}

void CollaborationClient::onDisconnected()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    m_peers.removeAll(sock);
    sock->deleteLater();
}

void CollaborationClient::sendPoint(int x, int y)
{
    QJsonObject obj;
    obj["type"] = "point";
    obj["x"] = x;
    obj["y"] = y;
    broadcast(obj);
}

void CollaborationClient::broadcast(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);
    for (auto* sock : m_peers) sock->write(bytes);
}

void CollaborationClient::handleMessage(const QJsonObject& obj)
{
    if (obj.value("type").toString() == "point") {
        int x = obj.value("x").toInt();
        int y = obj.value("y").toInt();
        emit pointReceived(x, y);
    }
}
