#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class WhiteboardController;
class SessionController;
class SyncManager;
class CanvasView;
class Toolbar;
class StylePanel;
class SessionDialog;
class UserListPanel;

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    MainWindow(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MainWindow();

private slots:
    void onCreateSessionClicked();
    void onJoinSessionClicked();
    void onSessionStatusChanged(int status, const QString& message);
    void onToolSelected(int toolId);
    void onTextAdded(const QPointF& position);
    void onElementSelected(const QUuid& elementId);

private:
    // Controllers
    std::unique_ptr<WhiteboardController> m_whiteboardController;
    std::unique_ptr<SessionController> m_sessionController;
    std::unique_ptr<SyncManager> m_syncManager;
    
    // UI Components
    CanvasView* m_canvasView;
    Toolbar* m_toolbar;
    StylePanel* m_stylePanel;
    SessionDialog* m_sessionDialog;
    UserListPanel* m_userListPanel;
    
    void setupUi();
    void setupControllers();
    void setupConnections();
    void showSessionDialog();
};

#endif // MAINWINDOW_H
