#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>
#include <QVector>
#include <QVector2D>
#include <QTimer>
#include <body.h>
#include <cmath>
#include <QObject>
#include <effector.h>
#include <hapticinterface.h>

//const double pi = 3.1415926535897;

class Physics : public QThread
{
    Q_OBJECT
public:
    Physics();
    static const sf::Vector2f TOP_LEFT, BOTTOM_RIGHT;
    static const int FORCE_FACTOR;
    void setTimeStep(double timeStep);
    void createEntities();
    void setStiffness(int bodyIndex, double frequence,double damping);
    int getTotalBodyCount();
    int getBodyCount();
    int getWorkspWallCount();
    Body getBody(int index);
    Body getWorkspWall(int index);
    QList<Body> * getObjects();
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
    void selectBody(int);
    void deleteBody(int index);
    void setDensity(float newDensity);
    void setStiffness(float newStiffness);
    void setDamping(float newDamping);
    void createBall(b2Vec2 position, float radius);


signals:
    void worldCreated();
    void forceUpdated(QVector2D force);
    void objectListUpdated(QList<Body> list);


private:
    double timeStep;
    int32 velocityIterations;
    int32 positionIterations;
    b2Vec2 gravity;
    b2World* world;
    QTimer *timer;
    float density,stiffness,damping;
    HapticInterface *hapticDevice;
    sf::Transform physics2graphics,device2physics;
    QList<Body> workspaceWalls;
    QList<Body> bodyList; //Model for the ListView
    Effector effector;
    QVector2D endEffectorRealPosition;
    void createSolidWall(b2Vec2 position, float rotation,
                         b2Vec2 size, bool isWorkspace);
    void createWorkspace(float left, float right,
                         float bottom, float top, float thickness);
    void createBall(b2Vec2 position,float radius, float stiffness,
                    float damping, float density, float maxSpacing);
    void createEffector(float radius);
    void createPlane();
    void createEffector();
};

#endif // PHYSICS_H
