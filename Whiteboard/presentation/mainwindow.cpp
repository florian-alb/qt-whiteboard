#include "mainwindow.h"
#include "canvasview.h"
#include "toolbar.h"
#include "stylepanel.h"
#include "sessiondialog.h"
#include "userlistpanel.h"
#include "../business/whiteboardcontroller.h"
#include "../business/sessioncontroller.h"
#include "../business/syncmanager.h"
#include "../data/textitem.h"
#include "../data/whiteboardelement.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QStatusBar>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    
    setupControllers();
    setupUi();
    setupConnections();
    
    // Show session dialog on startup
    QTimer::singleShot(100, this, &MainWindow::showSessionDialog);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupControllers() {
    // Create controllers
    m_syncManager = std::make_unique<SyncManager>();
    m_whiteboardController = std::make_unique<WhiteboardController>();
    m_sessionController = std::make_unique<SessionController>();
    
    // Connect controllers
    m_whiteboardController->setSyncManager(m_syncManager.get());
    m_sessionController->setSyncManager(m_syncManager.get());
}

void MainWindow::setupUi() {
    // Set window properties
    setWindowTitle("QT Whiteboard");
    resize(1024, 768);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create toolbar
    m_toolbar = new Toolbar(this);
    mainLayout->addWidget(m_toolbar);
    
    // Create canvas view
    m_canvasView = new CanvasView(m_whiteboardController.get(), this);
    mainLayout->addWidget(m_canvasView);
    
    // Create style panel as a dock widget
    QDockWidget* styleDock = new QDockWidget("Style", this);
    styleDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_stylePanel = new StylePanel(this);
    styleDock->setWidget(m_stylePanel);
    addDockWidget(Qt::RightDockWidgetArea, styleDock);
    
    // Create user list panel as a dock widget
    QDockWidget* userListDock = new QDockWidget("Users", this);
    userListDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_userListPanel = new UserListPanel(this);
    userListDock->setWidget(m_userListPanel);
    addDockWidget(Qt::RightDockWidgetArea, userListDock);
    
    // Create session dialog
    m_sessionDialog = new SessionDialog(this);
    
    // Set up status bar
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections() {
    // Connect toolbar signals
    connect(m_toolbar, &Toolbar::toolSelected, this, &MainWindow::onToolSelected);
    
    // Connect canvas view signals
    connect(m_canvasView, &CanvasView::textAdded, this, &MainWindow::onTextAdded);
    connect(m_canvasView, &CanvasView::elementSelected, this, &MainWindow::onElementSelected);
    
    // Connect session dialog signals
    connect(m_sessionDialog, &SessionDialog::createSessionClicked, this, &MainWindow::onCreateSessionClicked);
    connect(m_sessionDialog, &SessionDialog::joinSessionClicked, this, &MainWindow::onJoinSessionClicked);
    
    // Connect session controller signals
    connect(m_sessionController.get(), &SessionController::connectionStatusChanged, 
            this, &MainWindow::onSessionStatusChanged);
}

void MainWindow::showSessionDialog() {
    m_sessionDialog->exec();
}

void MainWindow::onCreateSessionClicked() {
    QString sessionName = m_sessionDialog->getSessionName();
    QString username = m_sessionDialog->getUsername();
    
    if (sessionName.isEmpty() || username.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter both session name and username.");
        return;
    }
    
    if (m_sessionController->createSession(sessionName, username)) {
        // Get the session code to share with others
        QString sessionCode = m_sessionController->getSession()->getSessionCode();
        
        // Show session code in a message box
        QMessageBox::information(this, "Session Created",
                              "Your session has been created!\n\n"
                              "Share this code with others to join your session:\n"
                              + sessionCode);
        
        // Update UI to show we're hosting
        statusBar()->showMessage("Hosting session: " + sessionName + " (Code: " + sessionCode + ")");
        
        // Set the current user ID in the whiteboard controller
        m_whiteboardController->setCurrentUserId(m_sessionController->getCurrentUser()->getId().toString());
        
        // Update user list
        m_userListPanel->addUser(username, true); // Add self as admin
    }
}

void MainWindow::onJoinSessionClicked() {
    QString sessionCode = m_sessionDialog->getSessionCode();
    QString username = m_sessionDialog->getUsername();
    
    if (sessionCode.isEmpty() || username.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter both session code and username.");
        return;
    }
    
    if (m_sessionController->joinSession(sessionCode, username)) {
        // Update UI to show we're connecting
        statusBar()->showMessage("Connecting to session: " + sessionCode);
        
        // Set the current user ID in the whiteboard controller
        m_whiteboardController->setCurrentUserId(m_sessionController->getCurrentUser()->getId().toString());
    }
}

void MainWindow::onSessionStatusChanged(int status, const QString& message) {
    // Update status bar with connection status
    statusBar()->showMessage(message);
    
    // Handle different status cases
    switch (status) {
        case SessionController::Connected:
            // Enable UI elements for connected state
            m_toolbar->setEnabled(true);
            m_canvasView->setEnabled(true);
            m_stylePanel->setEnabled(true);
            break;
            
        case SessionController::Disconnected:
        case SessionController::Error:
            // Disable UI elements for disconnected state
            m_toolbar->setEnabled(false);
            m_canvasView->setEnabled(false);
            m_stylePanel->setEnabled(false);
            break;
            
        default:
            break;
    }
}

void MainWindow::onToolSelected(int toolId) {
    // Set the active tool in the canvas view
    m_canvasView->setActiveTool(toolId);
}

void MainWindow::onTextAdded(const QPointF& position) {
    // Add a text element at the specified position
    m_whiteboardController->addTextElement(position, "");
}

void MainWindow::onElementSelected(const QUuid& elementId) {
    // Update the style panel with the selected element's properties
    auto element = m_whiteboardController->getModel()->getElement(elementId);
    if (element) {
        if (element->getType() == WhiteboardElement::Text) {
            auto textItem = std::dynamic_pointer_cast<TextItem>(element);
            if (textItem) {
                m_stylePanel->setTextProperties(textItem->getFont(), textItem->getColor());
            }
        }
    }
}
