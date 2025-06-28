#pragma once
#include <QJsonArray>
#include <QJsonObject>
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
  void sendHistoryToClient(QTcpSocket *client);
  void storeDrawing(const QJsonObject &drawing);
  void processMessage(const QJsonObject &msg, QTcpSocket *senderSocket);

  QTcpServer *m_server;
  QList<QTcpSocket *> m_clients;
  quint16 m_listenPort;
  QJsonArray m_drawingHistory;
  QMap<QTcpSocket *, QByteArray> m_clientBuffers;
};
