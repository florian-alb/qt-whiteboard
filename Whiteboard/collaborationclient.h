#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>

class CollaborationClient : public QObject {
    Q_OBJECT
public:
    explicit CollaborationClient(quint16 listenPort, QObject* parent = nullptr);

    /// Se connecter à un pair donné (IP + port décodés du "code unique")
    Q_SLOT void connectToPeer(const QHostAddress& peerIp, quint16 peerPort);

    /// Émettre un message JSON vers tous les pairs connectés
    Q_SLOT void sendCreateText(const QString& id, int x, int y, const QString& text);
    Q_SLOT void sendUpdateText(const QString& id, const QString& text);
    Q_SLOT void sendDeleteText(const QString& id);

signals:
    // reçus du réseau
    void createTextReceived(const QString& id, int x, int y, const QString& text);
    void updateTextReceived(const QString& id, const QString& text);
    void deleteTextReceived(const QString& id);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onPeerDisconnected();

private:
    void broadcastJson(const QJsonObject& obj);
    void handleIncomingJson(const QJsonObject& obj);

    QTcpServer*   m_server;
    QList<QTcpSocket*> m_peers;
};
