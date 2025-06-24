#include <QApplication>
#include "WhiteboardServer.h"
#include "WhiteboardClient.h"
#include "WhiteboardCanvas.h"
#include "PencilTool.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    // 1) Start server (in a real app, run separate process)
    WhiteboardServer server(12345);

    // 2) Create client UI
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    WhiteboardCanvas *canvas = new WhiteboardCanvas;
    layout->addWidget(canvas);

    WhiteboardClient *client = new WhiteboardClient(QUrl("ws://localhost:12345"));
    QObject::connect(client, &WhiteboardClient::connected, [&]()
                     { qDebug() << "Connected"; });
    QObject::connect(client, &WhiteboardClient::messageReceived,
                     canvas, &WhiteboardCanvas::applyRemote);
    QObject::connect(canvas, &WhiteboardCanvas::sendJson,
                     client, &WhiteboardClient::sendMessage);

    // 3) Install pencil tool
    static const QString userId = QString::number(qrand());
    PencilTool *pencil = new PencilTool(userId, &window);
    canvas->setTool(pencil);

    window.show();
    return a.exec();
}
