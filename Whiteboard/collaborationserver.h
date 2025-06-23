#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QJsonObject>

class CollaborationServer : public QObject {
    Q_OBJECT
public:
    explicit CollaborationServer(quint16 listenPort, QObject* parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void broadcast(const QJsonObject& msg, QTcpSocket* exclude = nullptr);
    void broadcastClientList();

    QTcpServer* m_server;
    QList<QTcpSocket*> m_clients;
    quint16 m_listenPort;
};
