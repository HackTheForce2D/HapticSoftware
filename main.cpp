#include "mainwindow.h"
#include <QApplication>
#include <physics.h>
#include <visual.h>
#include <hapticinterface.h>
#include <QPushButton>
#include <QListView>

//Q_DECLARE_METATYPE(QList<Body>)

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
    //QPushButton * buttonDelObject = w.findChild<QPushButton*>(QString("buttonDelObject"));
    QPushButton * buttonAddObject = w.findChild<QPushButton*>(QString("buttonAddObject"));
    //QListView * objectList = w.findChild<QListView*>(QString("objectList"));

    //QPushButton *start =
    HapticInterface pantograph;
    Physics physics;
    physics.setHapticInterface(&pantograph);
    display->setPhysics(&physics);
    //objectList->setModel((QAbstractItemModel *)physics.getObjects());
    //QStringListModel *objectListSource = new QStringListModel;
   // objectList->setModel(objectListSource );
    //objectListSource->insertColumns(0,1);
    //objectListSource->insertRows(0,1);
    //objectListSource->setData(objectListSource->index(0),"Test");
       //     insertRows(rowCount(), 1);
    //setData(index(rowCount()-1), string);

    //Set up signals exchanged between the classes

    // Send force from the physics simulation to the Ethernet interface
    QObject::connect(&physics,SIGNAL(forceUpdated(QVector2D)),
                     &pantograph,SLOT(setForce(QVector2D)));

    // Pause and resume simulation with the main window buttons
    QObject::connect(buttonRun,SIGNAL(clicked()),
                     &physics,SLOT(startSim()));
    QObject::connect(buttonStop,SIGNAL(clicked()),
                     &physics,SLOT(stopSim()));

    //
    QObject::connect(&w, SIGNAL(deleteAt(int)),
                     &physics,SLOT(deleteBody(int)));
    QObject::connect(&w, SIGNAL(deleteAll()),
                     &physics,SLOT(reset()));
    QObject::connect(&w, SIGNAL(selectedObject(int)),
                     &physics,SLOT(selectBody(int)));
    QObject::connect(&w, SIGNAL(findDevice()),
                     &connectionDialog,SLOT(show()));
    QObject::connect(&w, SIGNAL(createObject()),
                     &createObjectDialog,SLOT(show()));
    QObject::connect(&w, SIGNAL(createObject()),
                     display,SLOT(startCreationMode()));
    QObject::connect(&w,SIGNAL(deleteAll()), //replace this signal
                     display, SLOT(endCreationMode()));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                     &w,SLOT(bodyclicked(int)));
    QObject::connect(display,SIGNAL(bodyClicked(int)),
                      &physics,SLOT(selectBody(int)));
    QObject::connect(&connectionDialog,SIGNAL(connectToDevice(QString,int)),
                     &pantograph,SLOT(connectToHost(QString ,int)));
    //QObject::connect(buttonDelObject,SIGNAL(clicked()),
     //                &physics,SLOT(deleteBody()));
    //Register the custom classes so we can send them in the signal
    qRegisterMetaType<Body>("Body");
    qRegisterMetaType<QList<Body>>("QList<Body>");
    QObject::connect(&physics,SIGNAL(objectListUpdated(QList<Body>)),
                      &w, SLOT(updateListView(QList<Body>)));
    //QObject::connect(buttonAddObject,SIGNAL(clicked()),
     //                &physics,SLOT(addBall()));
    qRegisterMetaType<b2Vec2>("b2Vec2");
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


    //Launch communication thread
    pantograph.moveToThread(&pantograph);
    pantograph.start();
    //Launch simulation thread
    physics.moveToThread(&physics);
    physics.start();

    int exitValue(a.exec());
    physics.exit(); // Fix this
    pantograph.exit();
    return exitValue;
}
