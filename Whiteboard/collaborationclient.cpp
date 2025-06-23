#include "CollaborationClient.h"
#include "collaborationserver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

CollaborationClient::CollaborationClient(const QHostAddress& serverIp,
                                         quint16 serverPort,
                                         QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_serverPort(serverPort)
{
    connect(m_socket, &QTcpSocket::readyRead,
            this, &CollaborationClient::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &CollaborationClient::onServerDisconnected);
    connectToServer(serverIp, serverPort);
}

void CollaborationClient::connectToServer(const QHostAddress& ip, quint16 port)
{
    qDebug() << "Connecting to server" << ip.toString() << port;
    m_socket->abort();
    m_socket->connectToHost(ip, port);
}

void CollaborationClient::sendPoint(int x, int y)
{
    QJsonObject msg;
    msg["type"] = "point";
    msg["x"] = x;
    msg["y"] = y;
    m_socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
}

void CollaborationClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    auto doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    QString type = obj.value("type").toString();
    if (type == "point") {
       emit pointReceived(obj.value("x").toInt(), obj.value("y").toInt());
    } else if (type == "clientList") {
        m_servers.clear();
        for (const QJsonValue& v : obj.value("clients").toArray()) {
            auto o = v.toObject();
            m_servers.append({QHostAddress(o.value("ip").toString()), (quint16)o.value("port").toInt()});
        }
    }
}

void CollaborationClient::onServerDisconnected()
{
    qDebug() << "Server disconnected, electing new server";
    if (!m_servers.isEmpty()) {
        auto candidate = m_servers.first();
        // start backup server locally
        CollaborationServer* backup = new CollaborationServer(m_serverPort, this);
        // remove self from server list
        m_servers.removeFirst();
        // reconnect client socket to new server
        connectToServer(candidate.first, candidate.second);
    }
}
