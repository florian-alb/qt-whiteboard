#include "whiteboardserver.h"
#include <QDateTime>
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
  QTcpSocket *socket = new QTcpSocket(this);
  while (m_server->hasPendingConnections()) {
    QTcpSocket *client = m_server->nextPendingConnection();
    connect(client, &QTcpSocket::readyRead, this,
            &WhiteboardServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this,
            &WhiteboardServer::onClientDisconnected);
    m_clients << client;

    qDebug() << "Nouveau client connecté. Total clients:" << m_clients.size();
  }
}

void WhiteboardServer::onReadyRead() {
  QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
  QByteArray data = senderSocket->readAll();

  // Ajouter les nouvelles données au buffer du client
  m_clientBuffers[senderSocket] += data;

  // Traiter les messages ligne par ligne (découpage sur '\n')
  QByteArray &buffer = m_clientBuffers[senderSocket];
  while (true) {
    int idx = buffer.indexOf('\n');
    if (idx < 0)
      break;

    QByteArray line = buffer.left(idx);
    buffer.remove(0, idx + 1);

    // Parser le JSON
    QJsonDocument doc = QJsonDocument::fromJson(line);

    if (!doc.isNull() && doc.isObject()) {
      QJsonObject obj = doc.object();
      processMessage(obj, senderSocket);
    } else {
      qDebug() << "Erreur parsing JSON pour la ligne:" << line;
    }
  }
}

void WhiteboardServer::processMessage(const QJsonObject &msg,
                                      QTcpSocket *senderSocket) {
  // Vérifier si c'est une demande d'historique
  if (msg.contains("type") && msg["type"].toString() == "request_history") {
    qDebug() << "Demande d'historique reçue";
    sendHistoryToClient(senderSocket);
    return;
  }

  // Traiter les messages de curseur
  if (msg.contains("type") && msg["type"].toString() == "cursor_update") {
    qDebug() << "Position du curseur reçue:"
             << msg["props"].toObject()["position"].toArray();
    // Diffuser la position du curseur à tous les autres clients
    QJsonDocument doc(msg);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    for (QTcpSocket *sock : std::as_const(m_clients)) {
      if (sock != senderSocket && sock->state() == QTcpSocket::ConnectedState) {
        sock->write(data);
      }
    }
    return;
  }

  // Si c'est un message de dessin, le stocker
  if (msg.contains("type") && msg["type"].toString() == "object") {
    storeDrawing(msg);
  }

  // Broadcast à tous les autres clients
  QJsonDocument doc(msg);
  QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
  for (QTcpSocket *sock : std::as_const(m_clients)) {
    if (sock != senderSocket && sock->state() == QTcpSocket::ConnectedState) {
      sock->write(data);
    }
  }
}

void WhiteboardServer::onClientDisconnected() {
  QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
  m_clients.removeAll(socket);
  m_clientBuffers.remove(socket);
  socket->deleteLater();
  qDebug() << "Client déconnecté. Total clients:" << m_clients.size();
}

void WhiteboardServer::broadcastJson(const QJsonObject &obj,
                                     QTcpSocket *exclude) {
  QJsonDocument doc(obj);
  QByteArray data = doc.toJson(QJsonDocument::Compact);

  for (QTcpSocket *client : m_clients) {
    if (client != exclude) {
      client->write(data);
    }
  }
}

void WhiteboardServer::storeDrawing(const QJsonObject &drawing) {
  qDebug() << "Tentative de stockage d'un dessin:" << drawing;

  // Stocker tous les messages de dessin
  QJsonObject drawingWithTimestamp = drawing;
  if (!drawingWithTimestamp.contains("timestamp")) {
    drawingWithTimestamp["timestamp"] =
        QDateTime::currentDateTime().toString(Qt::ISODate);
  }

  m_drawingHistory.append(drawingWithTimestamp);
  qDebug() << "Dessin stocké dans l'historique. Total:"
           << m_drawingHistory.size();
}

void WhiteboardServer::sendHistoryToClient(QTcpSocket *client) {
  if (m_drawingHistory.isEmpty()) {
    qDebug() << "Aucun historique à envoyer au nouveau client";
    return;
  }

  // Créer un message spécial pour l'historique
  QJsonObject historyMsg;
  historyMsg["type"] = "history";
  historyMsg["drawings"] = m_drawingHistory;
  historyMsg["total_drawings"] = m_drawingHistory.size();

  QJsonDocument doc(historyMsg);
  QByteArray data = doc.toJson(QJsonDocument::Compact);

  client->write(data);
  qDebug() << "Historique envoyé au nouveau client:" << data;
}