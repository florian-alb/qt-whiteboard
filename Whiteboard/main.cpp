// main.cpp
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFrame>
#include <QScreen>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNetwork/qhostaddress.h>
#include <QRandomGenerator>

#include "circletool.h"
#include "erasertool.h"
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
  mainWindow->resize(1200, 800);
  mainWindow->setMinimumSize(800, 600);
  
  // Style de la fenêtre principale
  mainWindow->setStyleSheet("background-color: #f8f9fa;");
  
  // Centrer la fenêtre sur l'écran
  QScreen *screen = QApplication::primaryScreen();
  if (screen) {
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - mainWindow->width()) / 2;
    int y = (screenGeometry.height() - mainWindow->height()) / 2;
    mainWindow->move(x, y);
  }

  // 4) Création du layout principal
  QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(20, 20, 20, 20);

  QWidget *toolbarContainer = new QWidget;
  toolbarContainer->setFixedHeight(70);
  toolbarContainer->setMaximumWidth(1000);
  toolbarContainer->setStyleSheet(
    "background-color: white; "
    "border: 1px solid #e1e5e9; "
    "border-radius: 20px; "
    "margin: 0px 20px;");
  
  QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbarContainer);
  toolbarLayout->setSpacing(12);
  toolbarLayout->setContentsMargins(20, 15, 20, 15);
  toolbarLayout->setAlignment(Qt::AlignCenter);

  QPushButton *pencilButton = new QPushButton("✏️ Crayon");
  QPushButton *rectangleButton = new QPushButton("⬜ Rectangle");
  QPushButton *circleButton = new QPushButton("⭕ Cercle");
  QPushButton *textButton = new QPushButton("📝 Texte");
  QPushButton *eraserButton = new QPushButton("🧱 Gomme");
  
  // Séparateur visuel
  QFrame *separator = new QFrame;
  separator->setFrameShape(QFrame::VLine);
  separator->setFrameShadow(QFrame::Sunken);
  separator->setStyleSheet("color: #dee2e6; margin: 5px 0;");
  
  QPushButton *saveButton = new QPushButton("💾 Sauvegarder");
  QPushButton *loadButton = new QPushButton("📂 Charger");

  // Style des boutons
  pencilButton->setCheckable(true);
  rectangleButton->setCheckable(true);
  circleButton->setCheckable(true);
  textButton->setCheckable(true);
  eraserButton->setCheckable(true);
  pencilButton->setChecked(true); // Crayon par défaut

  // Taille des boutons
  pencilButton->setMinimumSize(140, 45);
  rectangleButton->setMinimumSize(140, 45);
  circleButton->setMinimumSize(140, 45);
  textButton->setMinimumSize(140, 45);
  eraserButton->setMinimumSize(140, 45);
  saveButton->setMinimumSize(140, 45);
  loadButton->setMinimumSize(140, 45);
  
  pencilButton->setMaximumHeight(45);
  rectangleButton->setMaximumHeight(45);
  circleButton->setMaximumHeight(45);
  textButton->setMaximumHeight(45);
  eraserButton->setMaximumHeight(45);
  saveButton->setMaximumHeight(45);
  loadButton->setMaximumHeight(45);
  
  pencilButton->setCursor(Qt::PointingHandCursor);
  rectangleButton->setCursor(Qt::PointingHandCursor);
  circleButton->setCursor(Qt::PointingHandCursor);
  textButton->setCursor(Qt::PointingHandCursor);
  eraserButton->setCursor(Qt::PointingHandCursor);
  saveButton->setCursor(Qt::PointingHandCursor);
  loadButton->setCursor(Qt::PointingHandCursor);

  // Style moderne des boutons
  QString toolButtonStyle = 
    "QPushButton {"
    "  background-color: #f8f9fa;"
    "  border: 2px solid #dee2e6;"
    "  border-radius: 6px;"
    "  padding: 6px 12px;"
    "  font-weight: bold;"
    "  color: #495057;"
    "}"
    "QPushButton:hover {"
    "  background-color: #e9ecef;"
    "  border-color: #adb5bd;"
    "}"
    "QPushButton:checked {"
    "  background-color: #007bff;"
    "  color: white;"
    "  border-color: #0056b3;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #0056b3;"
    "}";
    
  QString actionButtonStyle = 
    "QPushButton {"
    "  background-color: #28a745;"
    "  border: 2px solid #1e7e34;"
    "  border-radius: 6px;"
    "  padding: 6px 12px;"
    "  font-weight: bold;"
    "  color: white;"
    "}"
    "QPushButton:hover {"
    "  background-color: #218838;"
    "  border-color: #1c7430;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #1c7430;"
    "}";

  // Application des styles
  pencilButton->setStyleSheet(toolButtonStyle);
  rectangleButton->setStyleSheet(toolButtonStyle);
  circleButton->setStyleSheet(toolButtonStyle);
  textButton->setStyleSheet(toolButtonStyle);
  eraserButton->setStyleSheet(toolButtonStyle);
  saveButton->setStyleSheet(actionButtonStyle);
  loadButton->setStyleSheet(actionButtonStyle);

  // Ajout des widgets à la toolbar avec espacement centré
  toolbarLayout->addStretch();
  toolbarLayout->addWidget(pencilButton);
  toolbarLayout->addWidget(rectangleButton);
  toolbarLayout->addWidget(circleButton);
  toolbarLayout->addWidget(textButton);
  toolbarLayout->addWidget(eraserButton);
  toolbarLayout->addWidget(separator);
  toolbarLayout->addWidget(saveButton);
  toolbarLayout->addWidget(loadButton);
  toolbarLayout->addStretch();
  
  // Création d'un conteneur pour centrer la barre d'outils
  QHBoxLayout *toolbarCenterLayout = new QHBoxLayout();
  toolbarCenterLayout->addStretch();
  toolbarCenterLayout->addWidget(toolbarContainer);
  toolbarCenterLayout->addStretch();
  toolbarCenterLayout->setContentsMargins(0, 0, 0, -20); // Chevauchement négatif

  mainLayout->addLayout(toolbarCenterLayout);

  // 6) Création du canvas et des outils
  WhiteboardCanvas *canvas = new WhiteboardCanvas;
  QString userId = QString::number(QRandomGenerator::global()->generate());

  PencilTool *pencilTool = new PencilTool(userId);
  RectangleTool *rectangleTool = new RectangleTool(userId);
  CircleTool *circleTool = new CircleTool(userId);
  TextTool *textTool = new TextTool(userId, canvas);
  EraserTool *eraserTool = new EraserTool(userId);

  // Ajouter les cinq outils au canvas
  canvas->addTool(pencilTool);
  canvas->addTool(rectangleTool);
  canvas->addTool(circleTool);
  canvas->addTool(textTool);
  canvas->addTool(eraserTool);

  // Outil par défaut
  canvas->setActiveTool(pencilTool);

  // 7) Connexion des boutons pour changer d'outil actif
  QObject::connect(pencilButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(pencilTool);
    pencilButton->setChecked(true);
    rectangleButton->setChecked(false);
    circleButton->setChecked(false);
    textButton->setChecked(false);
    eraserButton->setChecked(false);
  });

  QObject::connect(rectangleButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(rectangleTool);
    rectangleButton->setChecked(true);
    pencilButton->setChecked(false);
    circleButton->setChecked(false);
    textButton->setChecked(false);
    eraserButton->setChecked(false);
  });

  QObject::connect(circleButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(circleTool);
    circleButton->setChecked(true);
    pencilButton->setChecked(false);
    rectangleButton->setChecked(false);
    textButton->setChecked(false);
    eraserButton->setChecked(false);
  });

  QObject::connect(textButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(textTool);
    textButton->setChecked(true);
    pencilButton->setChecked(false);
    rectangleButton->setChecked(false);
    circleButton->setChecked(false);
    eraserButton->setChecked(false);
  });
  
  QObject::connect(eraserButton, &QPushButton::clicked, [=]() {
    canvas->setActiveTool(eraserTool);
    eraserButton->setChecked(true);
    pencilButton->setChecked(false);
    rectangleButton->setChecked(false);
    circleButton->setChecked(false);
    textButton->setChecked(false);
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
      QMessageBox messageBox;
      messageBox.setStyleSheet("color: black;");
      messageBox.information(mainWindow, "Chargement",
                             "Dessins chargés avec succès !");
    }
  });

  // Conteneur pour le canvas avec style et z-index inférieur à la toolbar
  QWidget *canvasContainer = new QWidget;
  canvasContainer->setStyleSheet(
    "background-color: white; "
    "border: 1px solid #e1e5e9; "
    "border-radius: 12px; "
    "margin-top: 10px;");
  
  QVBoxLayout *canvasLayout = new QVBoxLayout(canvasContainer);
  canvasLayout->setContentsMargins(15, 35, 15, 15); // Marge supérieure plus grande pour la toolbar
  canvasLayout->addWidget(canvas);
  
  mainLayout->addWidget(canvasContainer);

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
