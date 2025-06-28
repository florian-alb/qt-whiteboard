#include "toolbar.h"
#include <QActionGroup>

Toolbar::Toolbar(QWidget* parent)
    : QToolBar(parent) {
    
    setupActions();
}

Toolbar::~Toolbar() {
}

void Toolbar::setupActions() {
    // Create action group for exclusive tool selection
    QActionGroup* toolGroup = new QActionGroup(this);
    toolGroup->setExclusive(true);
    
    // Create select tool action
    m_selectAction = new QAction("Select", this);
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true); // Default tool
    m_selectAction->setData(Select);
    toolGroup->addAction(m_selectAction);
    addAction(m_selectAction);
    
    // Create text tool action
    m_textAction = new QAction("Text", this);
    m_textAction->setCheckable(true);
    m_textAction->setData(Text);
    toolGroup->addAction(m_textAction);
    addAction(m_textAction);
    
    // Connect action triggered signal
    connect(toolGroup, &QActionGroup::triggered, this, &Toolbar::onToolActionTriggered);
}

void Toolbar::onToolActionTriggered(QAction* action) {
    if (action) {
        // Emit tool selected signal with tool ID
        emit toolSelected(action->data().toInt());
    }
}
