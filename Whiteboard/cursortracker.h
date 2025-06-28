#pragma once
#include <QMap>
#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QTimer>
#include <QVector>

class CursorTracker : public QObject {
  Q_OBJECT

public:
  explicit CursorTracker(QObject *parent = nullptr);

  // Méthodes pour envoyer la position de la souris
  void sendMousePosition(const QPoint &position, bool isDrawing = false);
  void sendMousePress(const QPoint &position);
  void sendMouseRelease(const QPoint &position);

  // Méthode pour dessiner les curseurs des autres utilisateurs
  void drawCursors(QPainter &painter);

  // Méthode pour nettoyer les curseurs inactifs
  void cleanupInactiveCursors();

signals:
  void sendJson(const QJsonObject &json);

public slots:
  void onRemoteCursorUpdate(const QJsonObject &msg);
  void sendPendingCursorPosition();

private:
  struct UserCursor {
    QString userId;
    QPoint position;
    bool isDrawing;
    qint64 lastUpdateTime; // Pour détecter les curseurs inactifs
  };

  QMap<QString, UserCursor> m_userCursors;
  QString m_localUserId;
  QTimer *m_cursorTimer;
  QPoint m_pendingPosition;
  bool m_pendingIsDrawing;
  bool m_hasPendingPosition;

  static const int CURSOR_TIMEOUT_MS = 5000; // 5 secondes d'inactivité
};