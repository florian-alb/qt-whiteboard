#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QAction>

/**
 * @brief Toolbar for whiteboard tools
 */
class Toolbar : public QToolBar {
    Q_OBJECT

public:
    enum Tool {
        Select = 0,
        Text = 1
    };

    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    Toolbar(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~Toolbar();

signals:
    /**
     * @brief Signal emitted when a tool is selected
     * @param toolId ID of the selected tool
     */
    void toolSelected(int toolId);

private slots:
    void onToolActionTriggered(QAction* action);

private:
    QAction* m_selectAction;
    QAction* m_textAction;
    
    void setupActions();
};

#endif // TOOLBAR_H
