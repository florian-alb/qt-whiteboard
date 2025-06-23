#include <QApplication>
#include <QInputDialog>
#include <QHostAddress>
#include "collaborationClient.h"
#include "BoardWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool ok;
    int port = QInputDialog::getInt(nullptr, "Local Port", "Listen on port:", 12345, 1024, 65535, 1, &ok);
    CollaborationClient net(port);

    QString peer = QInputDialog::getText(nullptr, "Peer Address", "Enter peer IP:port:");
    auto parts = peer.split(':');
    if (parts.size() == 2) {
        net.connectToPeer(QHostAddress(parts[0]), parts[1].toUShort());
    }

    BoardWidget board;
    board.show();

    QObject::connect(&board, &BoardWidget::pointCreated,
                     &net,   &CollaborationClient::sendPoint);
    QObject::connect(&net,   &CollaborationClient::pointReceived,
                     &board, &BoardWidget::addPoint);

    return a.exec();
}
