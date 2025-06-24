#pragma once
#include <QWebSocket>
#include <QJsonObject>

class WhiteboardClient : public QObject
{
    Q_OBJECT
public:
    WhiteboardClient(const QUrl &serverUrl, QObject *parent = nullptr);
    void sendMessage(const QJsonObject &msg);
signals:
    void messageReceived(const QJsonObject &msg);
    void connected();
    void disconnected();
private slots:
    void onTextMessage(const QString &msg);

private:
    QWebSocket m_socket;
};
