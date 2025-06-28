#include "user.h"
#include <QJsonObject>

User::User(const QString& username, bool isAdmin)
    : m_id(QUuid::createUuid()), m_username(username), m_isAdmin(isAdmin) {
}

QJsonObject User::toJson() const {
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["username"] = m_username;
    json["isAdmin"] = m_isAdmin;
    return json;
}

void User::fromJson(const QJsonObject& json) {
    if (json.contains("id")) {
        m_id = QUuid(json["id"].toString());
    }
    
    if (json.contains("username")) {
        m_username = json["username"].toString();
    }
    
    if (json.contains("isAdmin")) {
        m_isAdmin = json["isAdmin"].toBool();
    }
}
