#ifndef PHYSICS_H
#define PHYSICS_H

#include<Box2D/Box2D.h>
#include<QVector2D>
#include <QTimer>
#include<body.h>
#include<QObject>
#include<effector.h>

const double pi = 3.1415926535897;

class Physics : public QObject
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
    void updateBodies();

public slots:
    void step();
    void run();
    void stop();
    void reset();

signals:
    void worldCreated();


private:
    double timeStep;
    int32 velocityIterations;
    int32 positionIterations;
    b2Vec2 gravity;
    b2World* world;
    QTimer *timer;
    QVector<Body> bodyList;
    Effector effector;
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
