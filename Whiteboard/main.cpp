#include <QApplication>
#include <QInputDialog>
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>
#include "collaborationserver.h"
#include "CollaborationClient.h"
#include "boardwidget.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    bool isServer = QMessageBox::question(nullptr,
                                          "Mode Selection",
                                          "Run as server?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
    quint16 port = QInputDialog::getInt(nullptr,
                                        isServer ? "Server Port" : "Server Port",
                                        "Port:", 12345, 1024, 65535, 1);

    if (isServer) {
        qDebug() << "Starting as server on port" << port;
        CollaborationServer server(port);

        CollaborationClient client(QHostAddress("0.0.0.0"), port);
        BoardWidget board;
        board.show();
        QObject::connect(&board, &BoardWidget::pointCreated,
                         &client, &CollaborationClient::sendPoint);
        QObject::connect(&client, &CollaborationClient::pointReceived,
                         &board, &BoardWidget::addPoint);

        return a.exec();
    } else {
        QString host = QInputDialog::getText(nullptr,
                                             "Server IP", "Enter server IP:", QLineEdit::Normal, "10.31.39.77");
        CollaborationClient client(QHostAddress(host), port);
        BoardWidget board;
        board.show();
        QObject::connect(&board, &BoardWidget::pointCreated,
                         &client, &CollaborationClient::sendPoint);
        QObject::connect(&client, &CollaborationClient::pointReceived,
                         &board, &BoardWidget::addPoint);
        return a.exec();
    }
}
