// main.cpp
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNetwork/qhostaddress.h>

#include "circletool.h"
#include "penciltool.h"
#include "rectangletool.h"
#include "texttool.h"
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

  // 3) Création de la fenêtre principale
  QWidget *mainWindow = new QWidget;
  mainWindow->setWindowTitle("Tableau Blanc Collaboratif");
  mainWindow->resize(1000, 700);

  // 4) Création du layout principal
  QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);
  mainLayout->setSpacing(5);
  mainLayout->setContentsMargins(10, 10, 10, 10);

  // 5) Création de la barre d'outils
  QHBoxLayout *toolbarLayout = new QHBoxLayout;
  toolbarLayout->setSpacing(10);

  QPushButton *pencilButton = new QPushButton("✏️ Crayon");
  QPushButton *rectangleButton = new QPushButton("⬜ Rectangle");
  QPushButton *circleButton = new QPushButton("⭕ Cercle");
  QPushButton *textButton = new QPushButton("📝 Texte");
  QPushButton *saveButton = new QPushButton("💾 Sauvegarder");
  QPushButton *loadButton = new QPushButton("📂 Charger");

  // Style des boutons
  pencilButton->setCheckable(true);
  rectangleButton->setCheckable(true);
  circleButton->setCheckable(true);
  textButton->setCheckable(true);
  pencilButton->setChecked(true); // Crayon par défaut

  // Taille des boutons
  pencilButton->setMinimumHeight(40);
  rectangleButton->setMinimumHeight(40);
  circleButton->setMinimumHeight(40);
  textButton->setMinimumHeight(40);
  saveButton->setMinimumHeight(40);
  loadButton->setMinimumHeight(40);

  // Style des boutons
  QString buttonStyle = "QPushButton {"
                        "  background-color: #f0f0f0;"
                        "  border: 2px solid #c0c0c0;"
                        "  border-radius: 5px;"
                        "  padding: 8px 16px;"
                        "  font-size: 14px;"
                        "  font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "  background-color: #e0e0e0;"
                        "  border-color: #a0a0a0;"
                        "}"
                        "QPushButton:checked {"
                        "  background-color: #0078d4;"
                        "  color: white;"
                        "  border-color: #005a9e;"
                        "}"
                        "QPushButton:pressed {"
                        "  background-color: #005a9e;"
                        "}";

  pencilButton->setStyleSheet(buttonStyle);
  rectangleButton->setStyleSheet(buttonStyle);
  circleButton->setStyleSheet(buttonStyle);
  textButton->setStyleSheet(buttonStyle);
  saveButton->setStyleSheet(buttonStyle);
  loadButton->setStyleSheet(buttonStyle);

  toolbarLayout->addWidget(pencilButton);
  toolbarLayout->addWidget(rectangleButton);
  toolbarLayout->addWidget(circleButton);
  toolbarLayout->addWidget(textButton);
  toolbarLayout->addWidget(saveButton);
  toolbarLayout->addWidget(loadButton);
  toolbarLayout->addStretch(); // Espace flexible à droite

  mainLayout->addLayout(toolbarLayout);

  // 6) Création du canvas et des outils
  WhiteboardCanvas *canvas = new WhiteboardCanvas;
  QString userId = QString::number(arc4random());

  PencilTool *pencilTool = new PencilTool(userId);
  RectangleTool *rectangleTool = new RectangleTool(userId);
  CircleTool *circleTool = new CircleTool(userId);
  TextTool *textTool = new TextTool(userId, canvas);

  // Ajouter les quatre outils au canvas
  canvas->addTool(pencilTool);
  canvas->addTool(rectangleTool);
  canvas->addTool(circleTool);
  canvas->addTool(textTool);

  // Outil par défaut
  canvas->setActiveTool(pencilTool);

  // 7) Connexion des boutons pour changer d'outil actif
  QObject::connect(pencilButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(pencilTool);
    pencilButton->setChecked(true);
    rectangleButton->setChecked(false);
    circleButton->setChecked(false);
    textButton->setChecked(false);
  });

  QObject::connect(rectangleButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(rectangleTool);
    rectangleButton->setChecked(true);
    pencilButton->setChecked(false);
    circleButton->setChecked(false);
    textButton->setChecked(false);
  });

  QObject::connect(circleButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(circleTool);
    circleButton->setChecked(true);
    pencilButton->setChecked(false);
    rectangleButton->setChecked(false);
    textButton->setChecked(false);
  });

  QObject::connect(textButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(textTool);
    textButton->setChecked(true);
    pencilButton->setChecked(false);
    rectangleButton->setChecked(false);
    circleButton->setChecked(false);
  });

  // 8) Connexion des boutons de sauvegarde/chargement
  QObject::connect(saveButton, &QPushButton::clicked, [=]() {
    QString filename = QFileDialog::getSaveFileName(
        mainWindow, "Sauvegarder les dessins",
        QDir::homePath() + "/dessins.json", "Fichiers JSON (*.json)");

    if (!filename.isEmpty()) {
      canvas->saveDrawingsToFile(filename);
      QMessageBox::information(mainWindow, "Sauvegarde",
                               "Dessins sauvegardés avec succès !");
    }
  });

  QObject::connect(loadButton, &QPushButton::clicked, [=]() {
    QString filename = QFileDialog::getOpenFileName(
        mainWindow, "Charger des dessins", QDir::homePath(),
        "Fichiers JSON (*.json)");

    if (!filename.isEmpty()) {
      canvas->loadDrawingsFromFile(filename);
      QMessageBox::information(mainWindow, "Chargement",
                               "Dessins chargés avec succès !");
    }
  });

  mainLayout->addWidget(canvas);

  // 9) Configuration réseau
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

  mainWindow->show();

  return app.exec();
}
