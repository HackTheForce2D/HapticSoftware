#ifndef VISUAL_H
#define VISUAL_H

#include<QVector2D>
#include<physics.h>

class Visual
{
public:
    Visual();
    void setPhysics(Physics physics);
    void startDisplay();
private:
    Physics physics_;
    void getObjectLocations();
    QVector2D convertCoordinates();
};

#endif // VISUAL_H
