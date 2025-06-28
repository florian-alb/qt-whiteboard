#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QRadioButton;
class QPushButton;
class QLabel;

/**
 * @brief Dialog for creating or joining a session
 */
class SessionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    SessionDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~SessionDialog();
    
    /**
     * @brief Gets the entered username
     * @return Username
     */
    QString getUsername() const;
    
    /**
     * @brief Gets the entered session name
     * @return Session name
     */
    QString getSessionName() const;
    
    /**
     * @brief Gets the entered session code
     * @return Session code
     */
    QString getSessionCode() const;

signals:
    /**
     * @brief Signal emitted when the create session button is clicked
     */
    void createSessionClicked();
    
    /**
     * @brief Signal emitted when the join session button is clicked
     */
    void joinSessionClicked();

private slots:
    void onCreateRadioToggled(bool checked);
    void onJoinRadioToggled(bool checked);
    void onCreateButtonClicked();
    void onJoinButtonClicked();
    void onCancelButtonClicked();

private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_sessionNameEdit;
    QLineEdit* m_sessionCodeEdit;
    QRadioButton* m_createRadio;
    QRadioButton* m_joinRadio;
    QPushButton* m_createButton;
    QPushButton* m_joinButton;
    QPushButton* m_cancelButton;
    QLabel* m_sessionNameLabel;
    QLabel* m_sessionCodeLabel;
    
    void setupUi();
    void updateUi();
};

#endif // SESSIONDIALOG_H
