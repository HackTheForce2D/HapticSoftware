#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>
#include <QVector>
#include <QVector2D>
#include <QTimer>
#include <body.h>
#include <QObject>
#include <effector.h>
#include <hapticinterface.h>

//const double pi = 3.1415926535897;

class Physics : public QThread
{
    Q_OBJECT
public:
    Physics();
    void setTimeStep(double timeStep);
    void createEntities();
    void setStiffness(int bodyIndex, double frequence,double damping);
    int getBodyCount();
    Body getBody(int index);
    Effector getEffector();
    void setTransform(sf::Transform t);
    void setHapticInterface(HapticInterface *i);
    void updateBodies();
    void run();

public slots:
    void step();
    void startSim();
    void stopSim();
    void reset();

signals:
    void worldCreated();
    void forceUpdated(QVector2D force);


private:
    double timeStep;
    int32 velocityIterations;
    int32 positionIterations;
    b2Vec2 gravity;
    b2World* world;
    QTimer *timer;
    HapticInterface *hapticDevice;
    QVector<Body> bodyList;
    Effector effector;
    QVector2D endEffectorRealPosition;
    void createSolidWall(b2Vec2 position, float rotation, b2Vec2 size);
    void createWorkspace(float left, float right,
                         float bottom, float top, float thickness);
    void createBall(b2Vec2 position,float radius, float stiffness,
                    float damping, float density, float maxSpacing);
    void createEffector(float radius);
    void createPlane();
    void createEffector();
};

#endif // PHYSICS_H
