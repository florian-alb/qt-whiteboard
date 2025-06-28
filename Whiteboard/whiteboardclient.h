#pragma once
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>

class WhiteboardClient : public QObject {
  Q_OBJECT
public:
  explicit WhiteboardClient(const QHostAddress &host, quint16 port,
                            QObject *parent = nullptr);
  void sendMessage(const QJsonObject &msg);
  void requestHistory();

signals:
  void messageReceived(const QJsonObject &msg);
  void connected();
  void disconnected();

private slots:
  void onConnected();
  void onReadyRead();
  void onDisconnectedSlot();

private:
  QTcpSocket *m_socket;
  QByteArray m_buffer;
};
