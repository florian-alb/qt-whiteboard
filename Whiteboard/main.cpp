#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QInputDialog>
#include "CollaborationClient.h"
#include "MyGraphicsView.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Setup réseau P2P ---
    quint16 localPort = QInputDialog::getInt(nullptr, "Port local", "Sur quel port voulez-vous écouter ?", 5555);
    CollaborationClient net(localPort);

    // Demander l'IP:port du peer sous la forme "192.168.1.42:5555"
    QString peer = QInputDialog::getText(nullptr, "Peer",
                                         "Adresse du pair (ip:port) :");
    auto parts = peer.split(':');
    net.connectToPeer(QHostAddress(parts[0]), parts[1].toUShort());

    // --- Setup graphique minimal ---
    QGraphicsScene scene;
    MyGraphicsView view(&scene);
    view.setSceneRect(0,0,800,600);
    view.show();

    // Que faire quand on reçoit un createText
    QObject::connect(&net, &CollaborationClient::createTextReceived,
                     [&](const QString& id, int x, int y, const QString& text){
                         auto item = scene.addText(text);
                         item->setPos(x,y);
                         item->setFlag(QGraphicsItem::ItemIsSelectable);
                         item->setFlag(QGraphicsItem::ItemIsFocusable);
                         item->setTextInteractionFlags(Qt::TextEditorInteraction);
                         item->setData(0, id);
                     });

    // updateTextReceived
    QObject::connect(&net, &CollaborationClient::updateTextReceived,
                     [&](const QString& id, const QString& text){
                         for (auto *it : scene.items()) {
                             if (auto *ti = qgraphicsitem_cast<QGraphicsTextItem*>(it)) {
                                 if (ti->data(0).toString() == id)
                                     ti->setPlainText(text);
                             }
                         }
                     });

    // deleteTextReceived
    QObject::connect(&net, &CollaborationClient::deleteTextReceived,
                     [&](const QString& id){
                         for (auto *it : scene.items()) {
                             if (auto *ti = qgraphicsitem_cast<QGraphicsTextItem*>(it)) {
                                 if (ti->data(0).toString() == id)
                                     scene.removeItem(ti);
                             }
                         }
                     });

    // Pour tester localement : créer du texte au clic
    view.setMouseTracking(true);
    QObject::connect(&view, &MyGraphicsView::sceneClicked, [&](const QPointF& pos) {
        QString id = QUuid::createUuid().toString();

        auto item = scene.addText("Éditez…");
        item->setPos(pos);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setFlag(QGraphicsItem::ItemIsFocusable);
        item->setTextInteractionFlags(Qt::TextEditorInteraction);
        item->setData(0, id);

        net.sendCreateText(id, pos.x(), pos.y(), "Éditez…");

        QObject::connect(item->document(), &QTextDocument::contentsChanged, [item, id, &net]() {
            net.sendUpdateText(id, item->toPlainText());
        });
    });

    return a.exec();
}
