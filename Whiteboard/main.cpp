// main.cpp
#include <QApplication>
#include <QDebug>
#include <QHostAddress>
#include <QInputDialog>
#include <QMessageBox>
#include <QtNetwork/qhostaddress.h>

#include "penciltool.h"
#include "whiteboardcanvas.h"
#include "whiteboardclient.h"
#include "whiteboardserver.h"
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 1) Choix du mode
  bool isServer = (QMessageBox::question(nullptr, "Mode de démarrage",
                                         "Exécuter en tant que serveur ?",
                                         QMessageBox::Yes | QMessageBox::No) ==
                   QMessageBox::Yes);

  // 2) Récupération du port
  quint16 serverPort = static_cast<quint16>(
      QInputDialog::getInt(nullptr, isServer ? "Port serveur" : "Port serveur",
                           "Numéro de port :", 12345, 1024, 65535, 1));

  // 3) Common setup du canvas et de l’outil
  WhiteboardCanvas *canvas = new WhiteboardCanvas;
  QString userId = QString::number(arc4random());
  PencilTool *pencil = new PencilTool(userId);
  canvas->setTool(pencil);

  WhiteboardServer *server = nullptr;
  QHostAddress serverAddress;

  if (isServer) {
    // --- Mode Serveur + Client local ---
    qDebug() << "Démarrage du serveur sur le port" << serverPort;
    server = new WhiteboardServer(serverPort);
    serverAddress = QHostAddress::LocalHost;
  } else {
    // --- Mode Client seul ---
    bool ok;
    QString host = QInputDialog::getText(
        nullptr, "IP du serveur",
        "Entrez l'adresse du serveur (ex: 192.168.1.10) :", QLineEdit::Normal,
        "127.0.0.1", &ok);
    if (!ok)
      return 0;

    serverAddress = QHostAddress(host);
  }

  WhiteboardClient client(QHostAddress(serverAddress), serverPort);

  QObject::connect(canvas, &WhiteboardCanvas::sendJson, &client,
                   &WhiteboardClient::sendMessage);

  QObject::connect(&client, &WhiteboardClient::messageReceived, canvas,
                   &WhiteboardCanvas::applyRemote);

  canvas->show();

  return app.exec();
}
