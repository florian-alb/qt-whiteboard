#pragma once
#include <QWebSocketServer>
#include <QWebSocket>

class WhiteboardServer : public QObject
{
    Q_OBJECT
public:
    WhiteboardServer(quint16 port, QObject *parent = nullptr);
private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &msg);
    void socketDisconnected();

private:
    QWebSocketServer m_server;
    QList<QWebSocket *> m_clients;
};
