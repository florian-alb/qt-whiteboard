#include "whiteboardapp.h"
#include "presentation/mainwindow.h"

WhiteboardApp::WhiteboardApp(QApplication* app) 
    : m_app(app), m_mainWindow(nullptr) {
}

WhiteboardApp::~WhiteboardApp() {
}

void WhiteboardApp::run() {
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->show();
}
