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
}

void WhiteboardClient::onDisconnectedSlot() {
  qDebug() << __FUNCTION__ << __LINE__;
  qDebug() << "Déconnecté du serveur";
  emit disconnected();
}

void WhiteboardClient::sendMessage(const QJsonObject &msg) {
  qDebug() << __FUNCTION__ << __LINE__;
  QJsonDocument doc(msg);
  QByteArray json = doc.toJson(QJsonDocument::Compact) + "\n";
  m_socket->write(json);
}

void WhiteboardClient::onReadyRead() {
  qDebug() << __FUNCTION__ << __LINE__;
  m_buffer += m_socket->readAll();
  // On découpe sur '\n'
  while (true) {
    int idx = m_buffer.indexOf('\n');
    if (idx < 0)
      break;
    QByteArray line = m_buffer.left(idx);
    m_buffer.remove(0, idx + 1);
    QJsonDocument doc = QJsonDocument::fromJson(line);
    if (doc.isObject()) {
      emit messageReceived(doc.object());
    }
  }
}
