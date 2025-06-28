#ifndef WHITEBOARDAPP_H
#define WHITEBOARDAPP_H

#include <QApplication>
#include <memory>

class MainWindow;

/**
 * @brief Main application class that initializes and runs the whiteboard application
 */
class WhiteboardApp {
public:
    /**
     * @brief Constructor
     * @param app Pointer to the QApplication instance
     */
    WhiteboardApp(QApplication* app);
    
    /**
     * @brief Destructor
     */
    ~WhiteboardApp();
    
    /**
     * @brief Initializes and shows the main application window
     */
    void run();

private:
    QApplication* m_app;
    std::unique_ptr<MainWindow> m_mainWindow;
};

#endif // WHITEBOARDAPP_H
