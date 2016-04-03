#ifndef HAPTICINTERFACE_H
#define HAPTICINTERFACE_H

#include<QVector2D>
#include<QTcpSocket>
//#include<QHostAddress>
#include<vector>

class HapticInterface
{
public:
    HapticInterface();
    int connect(QHostAddress address);
    QVector2D getPosition();
    QVector2D getVelocity();
    void setForce(QVector2D force);
private:
    QVector2D position;
    QVector2D velocity;
    QVector2D force;
    QVector2D angle;
    QVector2D angularVelocity;
    QVector2D torque;
    QByteArray data;
   QTcpSocket device;

    QVector2D angle2position();
    QVector2D force2torque();
    void decodeData();
    int receiveData();
    int sendData();
};

#endif // HAPTICINTERFACE_H
