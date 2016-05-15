#ifndef PANTOGRAPH_H
#define PANTOGRAPH_H

#include <QVector>
#include <QVector2D>
#include <QMatrix2x2>
#include <algorithm> //sort
#include <iostream>


class Pantograph
{
public:
    Pantograph();
    void geometricModel();
    void kinematicModel();
    void nelderMead(float (*f)(QVector<float>),QVector<float> xStart);

private:
    QVector2D angle,angularVelocity,position,velocity;
    QVector2D force,torque;
    QVector2D A,B,C,D,E;
    QVector<QVector2D> calibrationAngles, calibrationCoordinates;
    QMatrix2x2 J;
    float a,b,c;
};

#endif // PANTOGRAPH_H
