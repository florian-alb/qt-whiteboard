#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>

class CollaborationClient : public QObject {
    Q_OBJECT
public:
    explicit CollaborationClient(quint16 listenPort, QObject* parent = nullptr);
    Q_SLOT void connectToPeer(const QHostAddress& ip, quint16 port);
    Q_SLOT void sendPoint(int x, int y);
signals:
    void pointReceived(int x, int y);
private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
private:
    void broadcast(const QJsonObject& obj);
    void handleMessage(const QJsonObject& obj);
    QTcpServer* m_server;
    QList<QTcpSocket*> m_peers;
};
