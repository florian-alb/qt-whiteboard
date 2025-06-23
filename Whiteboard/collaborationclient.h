#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonObject>
#include <QVector>
#include <QPair>

class CollaborationClient : public QObject {
    Q_OBJECT
public:
    explicit CollaborationClient(const QHostAddress& serverIp,
                                 quint16 serverPort,
                                 QObject* parent = nullptr);
    Q_SLOT void sendPoint(int x, int y);

signals:
    void pointReceived(int x, int y);

private slots:
    void onReadyRead();
    void onServerDisconnected();

private:
    void connectToServer(const QHostAddress& ip, quint16 port);

    QTcpSocket* m_socket;
    quint16 m_serverPort;
    QVector<QPair<QHostAddress, quint16>> m_servers;
};
