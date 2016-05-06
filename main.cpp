#include "mainwindow.h"
#include <QApplication>
#include <physics.h>
#include <visual.h>
#include <hapticinterface.h>
#include <QPushButton>
#include <QListView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    ConnectionDialog c;
    //c.show();
    Visual * display =  w.findChild<Visual*>(QString("display"));
    QPushButton * buttonRun = w.findChild<QPushButton*>(QString("buttonRun"));
    QPushButton * buttonStop = w.findChild<QPushButton*>(QString("buttonStop"));
    QPushButton * buttonDelObject = w.findChild<QPushButton*>(QString("buttonDelObject"));
    QPushButton * buttonAddObject = w.findChild<QPushButton*>(QString("buttonAddObject"));
    QListView * objectList = w.findChild<QListView*>(QString("objectList"));

    //QPushButton *start =
    HapticInterface pantograph;
    Physics physics;
    physics.setHapticInterface(&pantograph);
    display->setPhysics(&physics);
    //objectList->setModel((QAbstractItemModel *)physics.getObjects());

    QObject::connect(&physics,SIGNAL(forceUpdated(QVector2D)),
                     &pantograph,SLOT(setForce(QVector2D)));

    QObject::connect(buttonRun,SIGNAL(clicked()),
                     &physics,SLOT(startSim()));
    QObject::connect(buttonStop,SIGNAL(clicked()),
                     &physics,SLOT(stopSim()));
    QObject::connect(&w, SIGNAL(findDevice()),&c,SLOT(show()));
    QObject::connect(&c,SIGNAL(connectToDevice(QString,int)), //FINISH THIS SIGNAL
                     &pantograph,SLOT(connectToHost(QString ,int)));
    QObject::connect(buttonDelObject,SIGNAL(clicked()),
                     &physics,SLOT(deleteBody()));
    QObject::connect(buttonAddObject,SIGNAL(clicked()),
                     &physics,SLOT(addBall()));


    //QTcpServer sender;
    //sender.listen("Localhost",53100);

    //pantograph.connectToHost("localhost",53200);
    //Launch communication thread
    pantograph.moveToThread(&pantograph);
    pantograph.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.start();

    return a.exec();
}
