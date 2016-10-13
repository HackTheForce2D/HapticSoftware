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
    QObject::connect(&w, SIGNAL(createObject()),
                     &createObjectDialog,SLOT(show()));
    QObject::connect(&w, SIGNAL(createObject()),
                     display,SLOT(startCreationMode()));
    //Set object properties
    QObject::connect(&createObjectDialog,SIGNAL(toggleRigid(bool)),
                     &physics,SLOT(setRigid(bool)));
    QObject::connect(&createObjectDialog,SIGNAL(toggleStatic(bool)),
                     &physics,SLOT(setStatic(bool)));
    QObject::connect(&createObjectDialog,SIGNAL(updateProperties(float,float,float)),
                    &physics,SLOT(setObjectProperties(float,float,float)));
    QObject::connect(&createObjectDialog,SIGNAL(newCircle()),
                    display,SLOT(newCircle()));
    QObject::connect(&createObjectDialog,SIGNAL(newBox()),
                    display,SLOT(newBox()));
    // Create objects
    QObject::connect(display,SIGNAL(createNewCircle(b2Vec2,float)),
                     &physics,SLOT(createNewCircle(b2Vec2,float)));
    QObject::connect(display,SIGNAL(createNewBox(b2Vec2,float,b2Vec2)),
                     &physics,SLOT(createNewBox(b2Vec2,float,b2Vec2)));

    QObject::connect(&createObjectDialog,SIGNAL(endCreationMode()),
                     display,SLOT(endCreationMode()));


    // Object selection signals
    QObject::connect(&w, SIGNAL(selectedObject(int)),
                     &physics,SLOT(selectBody(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                     &w,SLOT(bodyclicked(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                      &physics,SLOT(selectBody(int)));

    // Object deletion signals
    QObject::connect(&w, SIGNAL(deleteAt(int)),
                     &physics,SLOT(deleteBody(int)));
    QObject::connect(&w, SIGNAL(deleteAll()),
                     &physics,SLOT(reset()));
    QObject::connect(display,SIGNAL(deletePressed()),
                      &w,SLOT(onDeleteKeyPressed()));
    QObject::connect(&physics, SIGNAL(stopDisplay(bool)),
                     display, SLOT(stopUpdating(bool)));
    // Replaced by a function call from Visual to Physics
    //QObject::connect(display, SIGNAL(displayStopped(bool)),
    //                 &physics, SLOT(displayStopped(bool)));

    QObject::connect(&connectionDialog,SIGNAL(connectToDevice(QString,int)),
                     &ethernetLink,SLOT(connectToHost(QString ,int)));

    // Send the list of physical bodies when a new body is added   
    QObject::connect(&physics,SIGNAL(objectListUpdated(QList<Body>)),
                      &w, SLOT(updateListView(QList<Body>)));

    // Device connection signals
    QObject::connect(&w, SIGNAL(findDevice()),
                     &connectionDialog,SLOT(show()));
    QObject::connect(&ethernetLink,SIGNAL(connected()),
                     &w, SLOT(onDeviceConnected()));
    QObject::connect(&ethernetLink,SIGNAL(disconnected()),
                     &w, SLOT(onDeviceDisconnected()));
    QObject::connect(&w,SIGNAL(disconnectDevice()),
                     &ethernetLink, SLOT(disconnect()));

    // Calibration signals
    QObject::connect(&w,SIGNAL(calibrationDemanded()),
                     display, SLOT(startCalibrationMode()));
    QObject::connect(display, SIGNAL(calibrationPointEntered(int)),
                     &ethernetLink, SLOT(sendCalibrationAngle(int)));
    QObject::connect(display,SIGNAL(readyToCalibrate()),
                     &physics, SLOT(startSim()));
    QObject::connect(display,SIGNAL(readyToCalibrate()),
                     &w, SLOT(onCalibrationLaunched()));
    QObject::connect(&ethernetLink, SIGNAL(calibrationFinished()),
                     &w, SLOT(onCalibrationFinished()));

    //Launch communication thread
    ethernetLink.moveToThread(&ethernetLink);
    ethernetLink.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.createEntities();
    physics.start();

    int exitValue(a.exec());
    // Finish the threads before quiting the application to avoid errors
    // Not working as intended
    physics.exit();
    ethernetLink.exit();
    return exitValue;
}
