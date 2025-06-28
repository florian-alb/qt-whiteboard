#include <QApplication>
#include "whiteboardapp.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Create and run the whiteboard application
    WhiteboardApp whiteboard(&app);
    whiteboard.run();
    
    return app.exec();
}
