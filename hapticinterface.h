#ifndef HAPTICINTERFACE_H
#define HAPTICINTERFACE_H

#include<QVector2D>
#include<QTcpSocket>
#include<QHostAddress>
#include<vector>
#include <cmath>

const double pi = 3.1415926535897;

class HapticInterface : public QObject
{
    Q_OBJECT
public:
    explicit HapticInterface(QObject *parent = 0);
    bool connectToHost(QString host);
    QVector2D getPosition();
    QVector2D getVelocity();
    void setForce(QVector2D force);

private slots:
    void readyRead();

private:
    QVector2D angle,angularVelocity,position,velocity;
    QVector2D force,torque;
    QVector2D A,B,C,D,E;
    float a,b,c;

    QByteArray data;
    QTcpSocket *device;

    QVector2D angle2position();
    QVector2D force2torque();
    void encodeData();
    void decodeData();
    bool receiveData();
    bool sendData();
};

#endif // HAPTICINTERFACE_H
