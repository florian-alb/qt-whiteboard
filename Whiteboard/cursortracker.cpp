#include "cursortracker.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QRandomGenerator>

CursorTracker::CursorTracker(QObject *parent)
    : QObject(parent), m_cursorTimer(new QTimer(this)),
      m_hasPendingPosition(false) {
  // Générer un ID unique pour cet utilisateur
  m_localUserId = QString::number(QRandomGenerator::global()->generate());

  // Timer pour nettoyer les curseurs inactifs
  QTimer *cleanupTimer = new QTimer(this);
  connect(cleanupTimer, &QTimer::timeout, this,
          &CursorTracker::cleanupInactiveCursors);
  cleanupTimer->start(1000); // Vérifier toutes les secondes

  // Timer pour limiter l'envoi des positions du curseur
  connect(m_cursorTimer, &QTimer::timeout, this,
          &CursorTracker::sendPendingCursorPosition);
  m_cursorTimer->setInterval(100);
}

void CursorTracker::sendMousePosition(const QPoint &position, bool isDrawing) {
  // Stocker la position en attente
  m_pendingPosition = position;
  m_pendingIsDrawing = isDrawing;
  m_hasPendingPosition = true;

  // Démarrer le timer s'il n'est pas déjà actif
  if (!m_cursorTimer->isActive()) {
    m_cursorTimer->start();
  }
}

void CursorTracker::sendPendingCursorPosition() {
  if (!m_hasPendingPosition) {
    return;
  }

  QJsonObject msg;
  msg["id"] = m_localUserId;
  msg["type"] = "cursor_update";

  QJsonObject props;
  QJsonArray coords;
  coords.append(m_pendingPosition.x());
  coords.append(m_pendingPosition.y());
  props["position"] = coords;
  props["isDrawing"] = m_pendingIsDrawing;
  msg["props"] = props;

  emit sendJson(msg);

  // Réinitialiser l'état en attente
  m_hasPendingPosition = false;
  m_cursorTimer->stop();
}

void CursorTracker::sendMousePress(const QPoint &position) {
  QJsonObject msg;
  msg["id"] = m_localUserId;
  msg["type"] = "cursor_press";

  QJsonObject props;
  QJsonArray coords;
  coords.append(position.x());
  coords.append(position.y());
  props["position"] = coords;
  msg["props"] = props;

  emit sendJson(msg);
}

void CursorTracker::sendMouseRelease(const QPoint &position) {
  QJsonObject msg;
  msg["id"] = m_localUserId;
  msg["type"] = "cursor_release";

  QJsonObject props;
  QJsonArray coords;
  coords.append(position.x());
  coords.append(position.y());
  props["position"] = coords;
  msg["props"] = props;

  emit sendJson(msg);
}

void CursorTracker::onRemoteCursorUpdate(const QJsonObject &msg) {
  QString userId = msg["id"].toString();
  QString type = msg["type"].toString();
  auto props = msg["props"].toObject();

  // Ignorer nos propres messages
  if (userId == m_localUserId) {
    return;
  }

  qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

  if (type == "cursor_update") {
    QJsonArray coords = props["position"].toArray();
    QPoint position(coords[0].toInt(), coords[1].toInt());
    bool isDrawing = props["isDrawing"].toBool();

    // Mettre à jour ou créer le curseur
    UserCursor &cursor = m_userCursors[userId];
    cursor.userId = userId;
    cursor.position = position;
    cursor.isDrawing = isDrawing;
    cursor.lastUpdateTime = currentTime;

  } else if (type == "cursor_press") {
    QJsonArray coords = props["position"].toArray();
    QPoint position(coords[0].toInt(), coords[1].toInt());

    UserCursor &cursor = m_userCursors[userId];
    cursor.userId = userId;
    cursor.position = position;
    cursor.isDrawing = true;
    cursor.lastUpdateTime = currentTime;

  } else if (type == "cursor_release") {
    if (m_userCursors.contains(userId)) {
      UserCursor &cursor = m_userCursors[userId];
      cursor.isDrawing = false;
      cursor.lastUpdateTime = currentTime;
    }
  }
}

void CursorTracker::drawCursors(QPainter &painter) {
  qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

  for (auto it = m_userCursors.begin(); it != m_userCursors.end(); ++it) {
    const UserCursor &cursor = it.value();

    // Ignorer les curseurs trop anciens
    if (currentTime - cursor.lastUpdateTime > CURSOR_TIMEOUT_MS) {
      continue;
    }

    // Cercle pour le curseur
    painter.drawEllipse(cursor.position, 5, 5);

    // Nom de l'utilisateur avec couleur adaptée
    if (cursor.isDrawing) {
      painter.setPen(Qt::red);
    } else {
      painter.setPen(Qt::blue);
    }
    painter.drawText(cursor.position + QPoint(10, -10), cursor.userId);
  }
}

void CursorTracker::cleanupInactiveCursors() {
  qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

  for (auto it = m_userCursors.begin(); it != m_userCursors.end();) {
    if (currentTime - it.value().lastUpdateTime > CURSOR_TIMEOUT_MS) {
      it = m_userCursors.erase(it);
    } else {
      ++it;
    }
  }
}
