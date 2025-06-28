#include "whiteboardclient.h"
#include <QDebug>
#include <QJsonDocument>
#include <QtCore/qlogging.h>

WhiteboardClient::WhiteboardClient(const QHostAddress &host, quint16 port,
                                   QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this)) {
  connect(m_socket, &QTcpSocket::connected, this,
          &WhiteboardClient::onConnected);
  connect(m_socket, &QTcpSocket::readyRead, this,
          &WhiteboardClient::onReadyRead);
  connect(m_socket, &QTcpSocket::disconnected, this,
          &WhiteboardClient::onDisconnectedSlot);

  m_socket->connectToHost(host, port);
}

void WhiteboardClient::onConnected() {
  qDebug() << __FUNCTION__ << __LINE__;
  qDebug() << "Connecté au serveur";
  emit connected();

  // Demander l'historique après la connexion
  requestHistory();
}

void WhiteboardClient::onDisconnectedSlot() {
  qDebug() << __FUNCTION__ << __LINE__;
  qDebug() << "Déconnecté du serveur";
  emit disconnected();
}

void WhiteboardClient::sendMessage(const QJsonObject &msg) {

  if (msg.contains("type") && msg["type"].toString() == "object") {
    qDebug() << "Envoi du message au serveur depuis le client:" << msg;
  }

  QJsonDocument doc(msg);
  QByteArray json = doc.toJson(QJsonDocument::Compact) + "\n";
  m_socket->write(json);
}

void WhiteboardClient::requestHistory() {
  qDebug() << "Demande d'historique au serveur";
  QJsonObject request;
  request["type"] = "request_history";
  sendMessage(request);
}

void WhiteboardClient::onReadyRead() {

  m_buffer += m_socket->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(m_buffer);
  qDebug() << "Doc reçu:" << doc;
  if (doc.isObject()) {
    emit messageReceived(doc.object());
  }

  while (true) {
    int idx = m_buffer.indexOf('\n');
    if (idx < 0)
      break;
    QByteArray line = m_buffer.left(idx);
    m_buffer.remove(0, idx + 1);
    QJsonDocument doc = QJsonDocument::fromJson(line);

    qDebug() << "Doc reçu:" << doc;
    if (doc.isObject()) {
      emit messageReceived(doc.object());
    }
  }
}
