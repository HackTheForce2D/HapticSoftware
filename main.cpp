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
    a.addLibraryPath("Plugins");
    MainWindow w;
    w.show();
    ConnectionDialog connectionDialog;
    CreateObject createObjectDialog;
    Visual * display =  w.findChild<Visual*>(QString("display"));
    QPushButton * buttonRun = w.findChild<QPushButton*>(QString("buttonRun"));
    QPushButton * buttonStop = w.findChild<QPushButton*>(QString("buttonStop"));

    HapticInterface ethernetLink;
    Physics physics;
    physics.setHapticInterface(&ethernetLink);
    display->setPhysics(&physics);

    //Register the custom/foreign types so we can send them in signals
    qRegisterMetaType<Body>("Body");
    qRegisterMetaType<QList<Body> >("QList<Body>");
    qRegisterMetaType<b2Vec2>("b2Vec2");

    //Set up signals exchanged between the classes

    // Send force from the physics simulation to the Ethernet interface
    QObject::connect(&physics,SIGNAL(forceUpdated(QVector2D)),
                     &ethernetLink,SLOT(setForce(QVector2D)));

    // Pause and resume simulation with the main window buttons
    // (replace this by main window signals)
    QObject::connect(buttonRun,SIGNAL(clicked()),
                     &physics,SLOT(startSim()));
    QObject::connect(buttonStop,SIGNAL(clicked()),
                     &physics,SLOT(stopSim()));

    // Object creation signals
    QObject::connect(&w, SIGNAL(deleteAt(int)),
                     &physics,SLOT(deleteBody(int)));
    QObject::connect(&w, SIGNAL(deleteAll()),
                     &physics,SLOT(reset()));
    QObject::connect(&w, SIGNAL(createObject()),
                     &createObjectDialog,SLOT(show()));
    QObject::connect(&w, SIGNAL(createObject()),
                     display,SLOT(startCreationMode()));
    QObject::connect(&createObjectDialog,SIGNAL(densityChanged(float)),
                     &physics,SLOT(setDensity(float)));
    QObject::connect(&createObjectDialog,SIGNAL(stiffnessChanged(float)),
                     &physics,SLOT(setStiffness(float)));
    QObject::connect(&createObjectDialog,SIGNAL(dampingChanged(float)),
                     &physics,SLOT(setDamping(float)));
    QObject::connect(&createObjectDialog,SIGNAL(endCreationMode()),
                     display,SLOT(endCreationMode()));
    QObject::connect(display,SIGNAL(createNewBody(b2Vec2,float)),
                     &physics,SLOT(createBall(b2Vec2,float)));

    // Object selection signals
    QObject::connect(&w, SIGNAL(selectedObject(int)),
                     &physics,SLOT(selectBody(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                     &w,SLOT(bodyclicked(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                      &physics,SLOT(selectBody(int)));
    QObject::connect(&connectionDialog,SIGNAL(connectToDevice(QString,int)),
                     &ethernetLink,SLOT(connectToHost(QString ,int)));

    // Send the list of physical bodies when a new body is added
    QObject::connect(&w, SIGNAL(findDevice()),
                     &connectionDialog,SLOT(show()));
    QObject::connect(&physics,SIGNAL(objectListUpdated(QList<Body>)),
                      &w, SLOT(updateListView(QList<Body>)));

    // Calibration signals
    QObject::connect(&w,SIGNAL(calibrationStarted()),
                     display, SLOT(startCalibrationMode()));
    QObject::connect(display, SIGNAL(calibrationPointEntered(int)),
                     &ethernetLink, SLOT(sendCalibrationAngle(int)));
    QObject::connect(display,SIGNAL(calibrationFinished()),
                     &physics, SLOT(startSim()));

    //Launch communication thread
    ethernetLink.moveToThread(&ethernetLink);
    ethernetLink.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.start();

    int exitValue(a.exec());
    // Finish the threads before quiting the application to avoid errors
    // Not working as intended
    physics.exit();
    ethernetLink.exit();
    return exitValue;
}
