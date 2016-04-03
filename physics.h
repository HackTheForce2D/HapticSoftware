#ifndef PHYSICS_H
#define PHYSICS_H

#include<Box2D/Box2D.h>

class Physics
{
public:
    Physics();
    void setTimeStep(double timeStep);
    void createEntities();
    void step();
    void setStiffness(int bodyIndex, double frequence,double damping);
    Vector getPosition(int bodyIndex);
    double getRotation(int bodyIndex);

private:
    double timeStep;
    b2World world;

};

#endif // PHYSICS_H
