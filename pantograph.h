#ifndef PANTOGRAPH_H
#define PANTOGRAPH_H

#include <QVector>
#include <QVector2D>
#include <QMatrix2x2>
#include <algorithm> //sort
#include <cmath>
#include<effector.h>
#include <iostream>

/* This class contains the methods needed to properly drive
 * the parallel robot that provides the force feedback. It's also
 * responsible for holding the robot's parameters, calibrating said
 * parameters and saving/loading the parameters from an XML file
 * (to be implemented)
 * */

class Pantograph : public QObject
{
    Q_OBJECT
public:
    static const sf::Vector2f TOP_LEFT, BOTTOM_RIGHT;
    static const int ENCODER_MAX_VAL;
    static const float ENCODER_TO_RAD,MAX_TORQUE;
    Pantograph();
    QVector2D geometricModel(QVector2D encoderReading);
    QMatrix2x2 kinematicModel();
    QVector2D calculateTorque(QVector2D force);
    void nelderMead(QVector<float> xStart);
    void geneticAlgorithm(QVector<float> xStart);

public slots:
    void setCalibAngle(int index, QVector2D angles);
    void calibrate();

private:
    // Pantograph vertices to calculate the jacobian matrix
    QVector2D A,B,C,D,E;
    QVector<QVector2D> calibrationAngles, calibrationCoordinates;
    // Pantograph's physical parameters
    float a,b,c,offset1,offset2;
    //functions for calibration
    float geometricModelError(QVector<float> parameters);
    float f(QVector<float> parameters);
};

#endif // PANTOGRAPH_H
