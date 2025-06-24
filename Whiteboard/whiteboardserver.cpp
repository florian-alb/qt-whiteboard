#include "whiteboardserver.h"
#include <QDebug>
#include <QHostAddress>
#include <QObject>
#include <QtCore/qjsondocument.h>

WhiteboardServer::WhiteboardServer(quint16 port, QObject *parent)
    : QObject(parent), m_listenPort(port), m_server(new QTcpServer(this)) {

  connect(m_server, &QTcpServer::newConnection, this,
          &WhiteboardServer::onNewConnection);
  m_server->listen(QHostAddress::Any, port);
  qDebug() << "Serveur TCP écoutant sur le port" << port;
}

void WhiteboardServer::onNewConnection() {
  qDebug() << __FUNCTION__ << __LINE__;
  QTcpSocket *socket = new QTcpSocket(this);
  while (m_server->hasPendingConnections()) {
    QTcpSocket *client = m_server->nextPendingConnection();
    connect(client, &QTcpSocket::readyRead, this,
            &WhiteboardServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this,
            &WhiteboardServer::onClientDisconnected);
    m_clients << client;
  }
}

void WhiteboardServer::onReadyRead() {
  qDebug() << __FUNCTION__ << __LINE__;
  QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
  QByteArray data = senderSocket->readAll();
  // Broadcast à tous (y compris l’émetteur si souhaité)
  for (QTcpSocket *sock : std::as_const(m_clients)) {
    if (sock->state() == QTcpSocket::ConnectedState) {
      sock->write(data);
    }
  }
}

void WhiteboardServer::onClientDisconnected() {
  qDebug() << __FUNCTION__ << __LINE__;
  QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
  m_clients.removeAll(socket);
  socket->deleteLater();
}

void WhiteboardServer::broadcastJson(const QJsonObject &obj,
                                     QTcpSocket *exclude) {
  qDebug() << __FUNCTION__ << __LINE__;
  QJsonDocument doc(obj);
  QByteArray data = doc.toJson(QJsonDocument::Compact);

  for (QTcpSocket *client : m_clients) {
    if (client != exclude) {
      client->write(data);
    }
  }
}