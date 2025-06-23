#include "CollaborationClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>

CollaborationClient::CollaborationClient(quint16 listenPort, QObject* parent)
    : QObject(parent),
    m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection,
            this, &CollaborationClient::onNewConnection);

    if (!m_server->listen(QHostAddress::Any, listenPort)) {
        qFatal("Impossible d'écouter sur le port %d", listenPort);
    }
    qInfo("En écoute P2P sur le port %d", listenPort);
}

void CollaborationClient::connectToPeer(const QHostAddress& peerIp, quint16 peerPort)
{
    QTcpSocket* socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &CollaborationClient::onSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &CollaborationClient::onPeerDisconnected);

    socket->connectToHost(peerIp, peerPort);
    if (socket->waitForConnected(3000)) {
        qInfo("Connecté à %s:%d", qPrintable(peerIp.toString()), peerPort);
        m_peers << socket;
    } else {
        qWarning("Échec connexion à %s:%d", qPrintable(peerIp.toString()), peerPort);
        socket->deleteLater();
    }
}

void CollaborationClient::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* peer = m_server->nextPendingConnection();
        connect(peer, &QTcpSocket::readyRead, this, &CollaborationClient::onSocketReadyRead);
        connect(peer, &QTcpSocket::disconnected, this, &CollaborationClient::onPeerDisconnected);
        m_peers << peer;
        qInfo("Nouveau peer connecté depuis %s:%d",
              qPrintable(peer->peerAddress().toString()),
              peer->peerPort());
    }
}

void CollaborationClient::onSocketReadyRead()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    while (sock->bytesAvailable()) {
        auto doc = QJsonDocument::fromJson(sock->readAll());
        if (doc.isObject())
            handleIncomingJson(doc.object());
    }
}

void CollaborationClient::onPeerDisconnected()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    m_peers.removeAll(sock);
    sock->deleteLater();
    qInfo("Peer déconnecté");
}

void CollaborationClient::broadcastJson(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    for (QTcpSocket* peer : m_peers) {
        peer->write(data);
    }
}

// Envoi
void CollaborationClient::sendCreateText(const QString& id, int x, int y, const QString& text)
{
    QJsonObject obj{
        {"type", "createText"},
        {"id", id},
        {"x", x},
        {"y", y},
        {"text", text}
    };
    broadcastJson(obj);
}

void CollaborationClient::sendUpdateText(const QString& id, const QString& text)
{
    QJsonObject obj{
        {"type", "updateText"},
        {"id", id},
        {"text", text}
    };
    broadcastJson(obj);
}

void CollaborationClient::sendDeleteText(const QString& id)
{
    QJsonObject obj{
        {"type", "deleteText"},
        {"id", id}
    };
    broadcastJson(obj);
}

// Réception
void CollaborationClient::handleIncomingJson(const QJsonObject& obj)
{
    QString type = obj.value("type").toString();
    if (type == "createText") {
        emit createTextReceived(
            obj.value("id").toString(),
            obj.value("x").toInt(),
            obj.value("y").toInt(),
            obj.value("text").toString()
            );
    }
    else if (type == "updateText") {
        emit updateTextReceived(
            obj.value("id").toString(),
            obj.value("text").toString()
            );
    }
    else if (type == "deleteText") {
        emit deleteTextReceived(obj.value("id").toString());
    }
}
