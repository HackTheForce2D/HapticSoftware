#include "mainwindow.h"
#include <QApplication>
#include <physics.h>
#include <visual.h>
#include <hapticinterface.h>
#include <QPushButton>
#include <QListView>

//Q_DECLARE_METATYPE(QList<Body>)

float testOptim(QVector<float> x)
{
    //quadratic function to test the optimizer used for calibration
    return (x[0]-60)*(x[0]-60)+
           (x[1]-100)*(x[1]-100)+
           (x[2]-135)*(x[2]-135)+
           (x[3]-2300)*(x[3]-2300)+
           (x[4]+2300)*(x[4]+2300);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    ConnectionDialog connectionDialog;
    //c.show();
    CreateObject createObjectDialog;
    //createObjectDialog.show();
    Visual * display =  w.findChild<Visual*>(QString("display"));
    QPushButton * buttonRun = w.findChild<QPushButton*>(QString("buttonRun"));
    QPushButton * buttonStop = w.findChild<QPushButton*>(QString("buttonStop"));

    HapticInterface ethernetLink;
    Physics physics;
    physics.setHapticInterface(&ethernetLink);
    display->setPhysics(&physics);

    //Register the custom/foreign types so we can send them in signals
    qRegisterMetaType<Body>("Body");
    qRegisterMetaType<QList<Body>>("QList<Body>");
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

    // Add, delete and deleteAll buttons (object editor)
    QObject::connect(&w, SIGNAL(deleteAt(int)),
                     &physics,SLOT(deleteBody(int)));
    QObject::connect(&w, SIGNAL(deleteAll()),
                     &physics,SLOT(reset()));
    QObject::connect(&w, SIGNAL(createObject()),
                     &createObjectDialog,SLOT(show()));
    QObject::connect(&w, SIGNAL(createObject()),
                     display,SLOT(startCreationMode()));

    // Object selection signals
    QObject::connect(&w, SIGNAL(selectedObject(int)),
                     &physics,SLOT(selectBody(int)));
    QObject::connect(&w, SIGNAL(findDevice()),
                     &connectionDialog,SLOT(show()));


    QObject::connect(&w,SIGNAL(deleteAll()), //replace this signal
                     display, SLOT(endCreationMode()));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                     &w,SLOT(bodyclicked(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                      &physics,SLOT(selectBody(int)));
    QObject::connect(&connectionDialog,SIGNAL(connectToDevice(QString,int)),
                     &ethernetLink,SLOT(connectToHost(QString ,int)));

    // Send the list of physical bodies when a new body is added

    QObject::connect(&physics,SIGNAL(objectListUpdated(QList<Body>)),
                      &w, SLOT(updateListView(QList<Body>)));

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
    QObject::connect(&w,SIGNAL(calibrationStarted()),
                     display, SLOT(startCalibrationMode()));

    // Calibration signals
    Pantograph pTest;
    QObject::connect(display, SIGNAL(calibrationPointEntered(int)),
                     &ethernetLink, SLOT(sendCalibrationAngle(int)));
    QObject::connect(&ethernetLink, SIGNAL(calibrationAngle(int,QVector2D)),
                     &pTest, SLOT(setCalibAngle(int,QVector2D)));
    QObject::connect(display,SIGNAL(calibrationFinished()),
                     &physics, SLOT(startSim()));
    //QObject::connect(display,SIGNAL(calibrationFinished()),
      //               &pTest, SLOT(calibrate()));

    //Testing the optimization function
    //
    //QVector<float> xStartTest(5,0);
   // xStartTest[0] = 1;
   // xStartTest[1] = 4;
    //xStartTest[2] = 5;
    //pTest.nelderMead(testOptim,xStartTest);


    //Launch communication thread
    ethernetLink.moveToThread(&ethernetLink);
    ethernetLink.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.start();

    int exitValue(a.exec());
    physics.exit(); // Fix this
    ethernetLink.exit();
    return exitValue;
}
