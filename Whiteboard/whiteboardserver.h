#pragma once
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore/qobject.h>

class WhiteboardServer : public QObject {
  Q_OBJECT
public:
  explicit WhiteboardServer(quint16 port, QObject *parent = nullptr);

private slots:
  void onNewConnection();
  void onReadyRead();
  void onClientDisconnected();

private:
  void broadcastJson(const QJsonObject &obj, QTcpSocket *exclude = nullptr);

  QTcpServer *m_server;
  QList<QTcpSocket *> m_clients;
  quint16 m_listenPort;
};
