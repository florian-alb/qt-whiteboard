#include "session.h"
#include <QJsonArray>
#include <QCryptographicHash>

Session::Session(const QString& name)
    : m_id(QUuid::createUuid()), m_name(name), m_creationTime(QDateTime::currentDateTime()) {
}

void Session::addUser(const User& user) {
    m_users.append(user);
}

bool Session::removeUser(const QUuid& userId) {
    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].getId() == userId) {
            m_users.removeAt(i);
            return true;
        }
    }
    return false;
}

User* Session::findUser(const QUuid& userId) {
    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].getId() == userId) {
            return &m_users[i];
        }
    }
    return nullptr;
}

QJsonObject Session::toJson() const {
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["name"] = m_name;
    json["creationTime"] = m_creationTime.toString(Qt::ISODate);
    
    QJsonArray usersArray;
    for (const User& user : m_users) {
        usersArray.append(user.toJson());
    }
    json["users"] = usersArray;
    
    return json;
}

void Session::fromJson(const QJsonObject& json) {
    if (json.contains("id")) {
        m_id = QUuid(json["id"].toString());
    }
    
    if (json.contains("name")) {
        m_name = json["name"].toString();
    }
    
    if (json.contains("creationTime")) {
        m_creationTime = QDateTime::fromString(json["creationTime"].toString(), Qt::ISODate);
    }
    
    if (json.contains("users") && json["users"].isArray()) {
        m_users.clear();
        QJsonArray usersArray = json["users"].toArray();
        for (const QJsonValue& value : usersArray) {
            if (value.isObject()) {
                User user;
                user.fromJson(value.toObject());
                m_users.append(user);
            }
        }
    }
}

QString Session::getSessionCode() const {
    // Create a unique but shareable session code based on the session ID
    QString idStr = m_id.toString(QUuid::WithoutBraces);
    QByteArray hash = QCryptographicHash::hash(idStr.toUtf8(), QCryptographicHash::Md5);
    
    // Use first 8 characters of the hash for a shorter code
    return QString(hash.toHex()).left(8).toUpper();
}
