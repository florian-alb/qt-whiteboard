// main.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScreen>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNetwork/qhostaddress.h>

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
  mainWindow->setWindowTitle("QT Whiteboard");
  mainWindow->resize(1200, 800);
  mainWindow->setMinimumSize(800, 600);
  
  // Style de la fenêtre principale avec un thème sombre
  mainWindow->setStyleSheet(
    "background-color: #1e1e1e;"
  );
  
  // Centrer la fenêtre sur l'écran
  QScreen *screen = QApplication::primaryScreen();
  if (screen) {
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - mainWindow->width());
    int y = (screenGeometry.height() - mainWindow->height());
    mainWindow->move(x, y);
  }

  // 4) Création du layout principal avec sidebar et zone de dessin
  QHBoxLayout *mainLayout = new QHBoxLayout(mainWindow);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // Création de la sidebar gauche pour les outils
  QWidget *sidebar = new QWidget;
  sidebar->setFixedWidth(250);
  sidebar->setStyleSheet(
    "background-color: #252526; "
    "border-right: 1px solid #333333;");
  
  QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setSpacing(10);
  sidebarLayout->setContentsMargins(10, 10, 10, 10);

  // Groupe d'outils de dessin
  QGroupBox *toolsGroup = new QGroupBox("Select");
  toolsGroup->setStyleSheet(
    "QGroupBox { "
    "  color: #ffffff; "
    "  font-weight: bold; "
    "  border: none; "
    "  margin-top: 15px; "
    "}"
    "QGroupBox::title { "
    "  subcontrol-origin: margin; "
    "  subcontrol-position: top left; "
    "  padding: 5px; "
    "  color: #ffffff; "
    "}");
  
  QVBoxLayout *toolsLayout = new QVBoxLayout(toolsGroup);
  toolsLayout->setSpacing(8);
  toolsLayout->setContentsMargins(5, 20, 5, 5);

  // Boutons d'outils avec style sombre
  QPushButton *pencilButton = new QPushButton("Pencil");
  QPushButton *rectangleButton = new QPushButton("Rectangle");
  QPushButton *circleButton = new QPushButton("Circle");
  QPushButton *textButton = new QPushButton("Text");
  QPushButton *eraserButton = new QPushButton("Eraser");
  QPushButton *saveButton = new QPushButton("Save");
  QPushButton *loadButton = new QPushButton("Load");
  
  // Groupe pour les options de style de texte
  QGroupBox *styleGroup = new QGroupBox("Style");
  styleGroup->setStyleSheet(
    "QGroupBox { "
    "  color: #ffffff; "
    "  font-weight: bold; "
    "  border: none; "
    "  margin-top: 25px; "
    "}"
    "QGroupBox::title { "
    "  subcontrol-origin: margin; "
    "  subcontrol-position: top left; "
    "  padding: 5px; "
    "  color: #ffffff; "
    "}");
  
  QVBoxLayout *styleLayout = new QVBoxLayout(styleGroup);
  styleLayout->setSpacing(10);
  styleLayout->setContentsMargins(5, 25, 5, 5);
  
  // Options de style de texte
  QLabel *fontLabel = new QLabel("Font:");
  fontLabel->setStyleSheet("color: #ffffff;");
  QComboBox *fontComboBox = new QComboBox();
  fontComboBox->addItem("Arial");
  fontComboBox->addItem("Times New Roman");
  fontComboBox->addItem("Courier New");
  fontComboBox->setStyleSheet(
    "QComboBox { "
    "  background-color: #333333; "
    "  color: #ffffff; "
    "  border: 1px solid #555555; "
    "  padding: 5px; "
    "  border-radius: 3px; "
    "}"
    "QComboBox::drop-down { "
    "  subcontrol-origin: padding; "
    "  subcontrol-position: top right; "
    "  width: 15px; "
    "  border-left: 1px solid #555555; "
    "}");
  
  QLabel *sizeLabel = new QLabel("Size:");
  sizeLabel->setStyleSheet("color: #ffffff;");
  QComboBox *sizeComboBox = new QComboBox();
  sizeComboBox->addItem("12");
  sizeComboBox->addItem("14");
  sizeComboBox->addItem("16");
  sizeComboBox->addItem("18");
  sizeComboBox->addItem("24");
  sizeComboBox->setStyleSheet(
    "QComboBox { "
    "  background-color: #333333; "
    "  color: #ffffff; "
    "  border: 1px solid #555555; "
    "  padding: 5px; "
    "  border-radius: 3px; "
    "}"
    "QComboBox::drop-down { "
    "  subcontrol-origin: padding; "
    "  subcontrol-position: top right; "
    "  width: 15px; "
    "  border-left: 1px solid #555555; "
    "}");
  
  QHBoxLayout *textStyleLayout = new QHBoxLayout();
  QCheckBox *boldCheckBox = new QCheckBox("Bold");
  boldCheckBox->setStyleSheet("color: #ffffff;");
  QCheckBox *italicCheckBox = new QCheckBox("Italic");
  italicCheckBox->setStyleSheet("color: #ffffff;");
  textStyleLayout->addWidget(boldCheckBox);
  textStyleLayout->addWidget(italicCheckBox);
  
  QLabel *colorLabel = new QLabel("Color:");
  colorLabel->setStyleSheet("color: #ffffff;");
  QPushButton *colorButton = new QPushButton();
  colorButton->setFixedSize(30, 30);
  colorButton->setStyleSheet(
    "background-color: #000000; "
    "border: 1px solid #555555; "
    "border-radius: 3px;");
  
  // Groupe pour les utilisateurs connectés
  QGroupBox *usersGroup = new QGroupBox("Users");
  usersGroup->setStyleSheet(
    "QGroupBox { "
    "  color: #ffffff; "
    "  font-weight: bold; "
    "  border: none; "
    "  margin-top: 25px; "
    "}"
    "QGroupBox::title { "
    "  subcontrol-origin: margin; "
    "  subcontrol-position: top left; "
    "  padding: 5px; "
    "  color: #ffffff; "
    "}");
  
  QVBoxLayout *usersLayout = new QVBoxLayout(usersGroup);
  usersLayout->setSpacing(10);
  usersLayout->setContentsMargins(5, 25, 5, 5);
  
  // Barre d'état en bas
  QLabel *statusLabel = new QLabel("Ready");
  statusLabel->setStyleSheet(
    "color: #ffffff; "
    "background-color: #007acc; "
    "padding: 3px 10px;");
  statusLabel->setFixedHeight(25);

  // Style des boutons d'outils
  QString toolButtonStyle = 
    "QPushButton { "
    "  background-color: #333333; "
    "  color: #ffffff; "
    "  border: 1px solid #555555; "
    "  border-radius: 3px; "
    "  padding: 8px; "
    "  text-align: left; "
    "  width: 100%; "
    "} "
    "QPushButton:hover { "
    "  background-color: #3e3e3e; "
    "} "
    "QPushButton:pressed, QPushButton:checked { "
    "  background-color: #8800ff; "
    "  color: #ffffff; "
    "} ";

  pencilButton->setCheckable(true);
  rectangleButton->setCheckable(true);
  circleButton->setCheckable(true);
  textButton->setCheckable(true);
  eraserButton->setCheckable(true);
  pencilButton->setChecked(true); // Outil par défaut
  
  pencilButton->setStyleSheet(toolButtonStyle);
  rectangleButton->setStyleSheet(toolButtonStyle);
  circleButton->setStyleSheet(toolButtonStyle);
  textButton->setStyleSheet(toolButtonStyle);
  eraserButton->setStyleSheet(toolButtonStyle);
  saveButton->setStyleSheet(toolButtonStyle);
  loadButton->setStyleSheet(toolButtonStyle);

  // Taille des boutons - légèrement plus compacts
  pencilButton->setMinimumSize(130, 42);
  rectangleButton->setMinimumSize(130, 42);
  circleButton->setMinimumSize(130, 42);
  textButton->setMinimumSize(130, 42);
  eraserButton->setMinimumSize(130, 42);
  saveButton->setMinimumSize(130, 42);
  loadButton->setMinimumSize(130, 42);
  
  pencilButton->setMaximumHeight(42);
  rectangleButton->setMaximumHeight(42);
  circleButton->setMaximumHeight(42);
  textButton->setMaximumHeight(42);
  eraserButton->setMaximumHeight(42);
  saveButton->setMaximumHeight(42);
  loadButton->setMaximumHeight(42);
  
  // Polices pour tous les boutons
  pencilButton->setFont(QFont("Arial", 10));
  rectangleButton->setFont(QFont("Arial", 10));
  circleButton->setFont(QFont("Arial", 10));
  textButton->setFont(QFont("Arial", 10));
  eraserButton->setFont(QFont("Arial", 10));
  saveButton->setFont(QFont("Arial", 10));
  loadButton->setFont(QFont("Arial", 10));
  
  // Curseur pointeur pour tous les boutons
  pencilButton->setCursor(Qt::PointingHandCursor);
  rectangleButton->setCursor(Qt::PointingHandCursor);
  circleButton->setCursor(Qt::PointingHandCursor);
  textButton->setCursor(Qt::PointingHandCursor);
  eraserButton->setCursor(Qt::PointingHandCursor);
  saveButton->setCursor(Qt::PointingHandCursor);
  loadButton->setCursor(Qt::PointingHandCursor);

  // Taille des boutons
  pencilButton->setFixedHeight(30);
  rectangleButton->setFixedHeight(30);
  circleButton->setFixedHeight(30);
  textButton->setFixedHeight(30);
  eraserButton->setFixedHeight(30);
  saveButton->setFixedHeight(30);
  loadButton->setFixedHeight(30);

  // Ajout des boutons au layout des outils
  toolsLayout->addWidget(pencilButton);
  toolsLayout->addWidget(rectangleButton);
  toolsLayout->addWidget(circleButton);
  toolsLayout->addWidget(textButton);
  toolsLayout->addWidget(eraserButton);
  toolsLayout->addWidget(saveButton);
  toolsLayout->addWidget(loadButton);

  // Ajout des éléments de style au layout de style
  styleLayout->addWidget(fontLabel);
  styleLayout->addWidget(fontComboBox);
  styleLayout->addWidget(sizeLabel);
  styleLayout->addWidget(sizeComboBox);
  styleLayout->addLayout(textStyleLayout);
  styleLayout->addWidget(colorLabel);
  
  QHBoxLayout *colorLayout = new QHBoxLayout();
  colorLayout->addWidget(colorButton);
  colorLayout->addStretch();
  styleLayout->addLayout(colorLayout);

  // Ajout des groupes au layout de la sidebar
  sidebarLayout->addWidget(toolsGroup);
  sidebarLayout->addWidget(styleGroup);
  sidebarLayout->addStretch();

  // 5) Création du canvas et de la zone principale
  QWidget *mainArea = new QWidget;
  QVBoxLayout *mainAreaLayout = new QVBoxLayout(mainArea);
  mainAreaLayout->setContentsMargins(0, 0, 0, 0);
  mainAreaLayout->setSpacing(0);

  // 6) Création des outils et du canvas
  QString userId = QString::number(QRandomGenerator::global()->generate());
  
  // Canvas avec fond blanc
  WhiteboardCanvas *canvas = new WhiteboardCanvas;
  canvas->setStyleSheet("background-color: white;");
  
  // Création des outils
  PencilTool *pencilTool = new PencilTool(userId);
  RectangleTool *rectangleTool = new RectangleTool(userId);
  CircleTool *circleTool = new CircleTool(userId);
  TextTool *textTool = new TextTool(userId, canvas);
  EraserTool *eraserTool = new EraserTool(userId);
  
  // Ajout du canvas et de la barre d'état au layout principal
  mainAreaLayout->addWidget(canvas);
  mainAreaLayout->addWidget(statusLabel);

  // Ajout de la sidebar et de la zone principale au layout principal
  mainLayout->addWidget(sidebar);
  mainLayout->addWidget(mainArea);

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
        mainWindow, "Save drawings",
        QDir::homePath() + "/drawings.json", "JSON Files (*.json)");

    if (!filename.isEmpty()) {
      canvas->saveDrawingsToFile(filename);
      QMessageBox messageBox;
      messageBox.setStyleSheet(
        "QMessageBox { background-color: #252526; color: #ffffff; }"
        "QLabel { color: #ffffff; font-size: 12px; }"
        "QPushButton { background-color: #8800ff; color: white; "
        "border-radius: 6px; padding: 6px 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #9900ff; }"
      );
      messageBox.information(mainWindow, "Save",
                             "Drawings saved successfully!");
    }
  });

  QObject::connect(loadButton, &QPushButton::clicked, [=]() {
    QString filename = QFileDialog::getOpenFileName(
        mainWindow, "Load drawings", QDir::homePath(),
        "JSON Files (*.json)");

    if (!filename.isEmpty()) {
      canvas->loadDrawingsFromFile(filename);
      QMessageBox messageBox;
      messageBox.setStyleSheet(
        "QMessageBox { background-color: #252526; color: #ffffff; }"
        "QLabel { color: #ffffff; font-size: 12px; }"
        "QPushButton { background-color: #8800ff; color: white; "
        "border-radius: 6px; padding: 6px 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #9900ff; }"
      );
      messageBox.information(mainWindow, "Load",
                             "Drawings loaded successfully!");
    }
  });

  // Mise à jour des styles de message box pour le thème sombre
  QMessageBox::StandardButton defaultButton = QMessageBox::NoButton;

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
