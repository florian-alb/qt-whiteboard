#include "syncmanager.h"
#include "../data/textitem.h"
#include <QJsonDocument>

SyncManager::SyncManager(QObject* parent)
    : QObject(parent), m_networkService(new NetworkService(this)), m_session(nullptr), m_currentUser(nullptr) {
    
    // Connect network service signals
    connect(m_networkService, &NetworkService::messageReceived, this, &SyncManager::handleNetworkMessage);
    connect(m_networkService, &NetworkService::connectionStatusChanged, this, &SyncManager::connectionStatusChanged);
}

SyncManager::~SyncManager() {
    delete m_networkService;
}

bool SyncManager::startHosting(Session* session, User* hostUser) {
    if (!session || !hostUser) {
        return false;
    }
    
    m_session = session;
    m_currentUser = hostUser;
    
    // Start the network service in host mode
    return m_networkService->startServer();
}

bool SyncManager::joinSession(const QString& sessionCode, User* clientUser) {
    if (!clientUser) {
        return false;
    }
    
    m_currentUser = clientUser;
    
    // Start the network service in client mode
    return m_networkService->connectToServer(sessionCode);
}

bool SyncManager::leaveSession() {
    // Disconnect from the network
    m_networkService->disconnect();
    
    m_session = nullptr;
    m_currentUser = nullptr;
    
    return true;
}

void SyncManager::syncElementAdded(std::shared_ptr<WhiteboardElement> element) {
    if (!element) {
        return;
    }
    
    // Create a message for element addition
    QJsonObject message;
    message["type"] = "element_added";
    message["element"] = element->toJson();
    
    // Send the message
    sendMessage(message);
}

void SyncManager::syncElementRemoved(const QUuid& elementId) {
    // Create a message for element removal
    QJsonObject message;
    message["type"] = "element_removed";
    message["elementId"] = elementId.toString(QUuid::WithoutBraces);
    
    // Send the message
    sendMessage(message);
}

void SyncManager::syncElementModified(std::shared_ptr<WhiteboardElement> element) {
    if (!element) {
        return;
    }
    
    // Create a message for element modification
    QJsonObject message;
    message["type"] = "element_modified";
    message["element"] = element->toJson();
    
    // Send the message
    sendMessage(message);
}

void SyncManager::syncFullModel(const QUuid& clientId) {
    // This would be called when a new client joins to send them the full model
    // Implementation depends on how the model is accessed
    // For now, this is a placeholder
}

void SyncManager::handleNetworkMessage(const QJsonObject& message) {
    if (!message.contains("type")) {
        return;
    }
    
    QString type = message["type"].toString();
    
    if (type == "element_added" && message.contains("element")) {
        QJsonObject elementJson = message["element"].toObject();
        
        if (elementJson.contains("type")) {
            WhiteboardElement::ElementType elementType = static_cast<WhiteboardElement::ElementType>(elementJson["type"].toInt());
            
            std::shared_ptr<WhiteboardElement> element;
            switch (elementType) {
                case WhiteboardElement::Text: {
                    QPointF pos(elementJson["posX"].toDouble(), elementJson["posY"].toDouble());
                    element = std::make_shared<TextItem>(pos);
                    break;
                }
                // Add other element types as needed
                default:
                    return;
            }
            
            element->fromJson(elementJson);
            emit elementReceived(element);
        }
    }
    else if (type == "element_removed" && message.contains("elementId")) {
        QUuid elementId(message["elementId"].toString());
        emit elementRemovalReceived(elementId);
    }
    else if (type == "element_modified" && message.contains("element")) {
        QJsonObject elementJson = message["element"].toObject();
        
        if (elementJson.contains("type")) {
            WhiteboardElement::ElementType elementType = static_cast<WhiteboardElement::ElementType>(elementJson["type"].toInt());
            
            std::shared_ptr<WhiteboardElement> element;
            switch (elementType) {
                case WhiteboardElement::Text: {
                    QPointF pos(elementJson["posX"].toDouble(), elementJson["posY"].toDouble());
                    element = std::make_shared<TextItem>(pos);
                    break;
                }
                // Add other element types as needed
                default:
                    return;
            }
            
            element->fromJson(elementJson);
            emit elementModificationReceived(element);
        }
    }
    else if (type == "user_joined" && message.contains("user")) {
        QJsonObject userJson = message["user"].toObject();
        User user;
        user.fromJson(userJson);
        emit userJoined(user);
    }
    else if (type == "user_left" && message.contains("userId")) {
        QUuid userId(message["userId"].toString());
        emit userLeft(userId);
    }
}

void SyncManager::sendMessage(const QJsonObject& message) {
    m_networkService->sendMessage(message);
}
