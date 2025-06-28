#include "sessioncontroller.h"

SessionController::SessionController(QObject* parent)
    : QObject(parent), m_role(SessionRole::Client), 
      m_connectionStatus(ConnectionStatus::Disconnected), m_syncManager(nullptr) {
}

SessionController::~SessionController() {
}

bool SessionController::createSession(const QString& sessionName, const QString& username) {
    // Create a new session
    m_session = std::make_unique<Session>(sessionName);
    
    // Create the host user
    m_currentUser = std::make_unique<User>(username, true); // Host is admin
    m_session->addUser(*m_currentUser);
    
    // Set role to host
    m_role = SessionRole::Host;
    
    // Update connection status
    setConnectionStatus(ConnectionStatus::Connected, "Session created successfully");
    
    // Notify the sync manager if available
    if (m_syncManager) {
        m_syncManager->startHosting(m_session.get(), m_currentUser.get());
    }
    
    return true;
}

bool SessionController::joinSession(const QString& sessionCode, const QString& username) {
    // Create the client user
    m_currentUser = std::make_unique<User>(username, false); // Client is not admin
    
    // Update connection status
    setConnectionStatus(ConnectionStatus::Connecting, "Connecting to session...");
    
    // Notify the sync manager if available
    if (m_syncManager) {
        return m_syncManager->joinSession(sessionCode, m_currentUser.get());
    }
    
    setConnectionStatus(ConnectionStatus::Error, "Sync manager not available");
    return false;
}

bool SessionController::leaveSession() {
    if (!m_session) {
        return false;
    }
    
    // Notify the sync manager if available
    if (m_syncManager) {
        m_syncManager->leaveSession();
    }
    
    // Clear session and user
    m_session.reset();
    m_currentUser.reset();
    
    // Update connection status
    setConnectionStatus(ConnectionStatus::Disconnected, "Disconnected from session");
    
    return true;
}

QString SessionController::getSessionCode() const {
    if (m_session) {
        return m_session->getSessionCode();
    }
    return "";
}

void SessionController::setConnectionStatus(ConnectionStatus status, const QString& message) {
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        m_lastErrorMessage = message;
        emit connectionStatusChanged(status, message);
    }
}
