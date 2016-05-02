#include "mainwindow.h"
#include <QApplication>
#include <physics.h>
#include <visual.h>
#include <hapticinterface.h>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Visual * display =  w.findChild<Visual*>(QString("display"));
    QPushButton * buttonRun = w.findChild<QPushButton*>(QString("buttonRun"));
    QPushButton * buttonStop = w.findChild<QPushButton*>(QString("buttonStop"));
    //QPushButton *start =
    HapticInterface pantograph;
    Physics physics;
    physics.setHapticInterface(&pantograph);
    display->setPhysics(&physics);
    QObject::connect(&physics,SIGNAL(forceUpdated(QVector2D)),
                     &pantograph,SLOT(setForce(QVector2D)));

    QObject::connect(buttonRun,SIGNAL(clicked()),
                     &physics,SLOT(startSim()));
    QObject::connect(buttonStop,SIGNAL(clicked()),
                     &physics,SLOT(stopSim()));


    //QTcpServer sender;
    //sender.listen("Localhost",53100);

    pantograph.connectToHost("localhost");
    //Launch communication thread
    pantograph.moveToThread(&pantograph);
    pantograph.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.start();

    return a.exec();
}
