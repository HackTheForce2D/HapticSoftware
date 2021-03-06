#ifndef HAPTICINTERFACE_H
#define HAPTICINTERFACE_H

#include <QVector2D>
#include <pantograph.h>
#include <QMatrix2x2>
#include <QtNetwork>
#include <QHostAddress>
#include <vector>
#include <cmath>

const double pi = 3.1415926535897;

class HapticInterface : public QThread
{
    Q_OBJECT
public:
    explicit HapticInterface(QObject *parent = 0);
    QVector2D getPosition();
    QVector2D getVelocity();
    void updateTorque();
    void run();

public slots:
    bool connectToHost(QString host,int port);
    void readData();
    void setForce(QVector2D newForce);
    void disconnect();
    void reportConnected();
    void reportDisconnected();
    void sendCalibrationAngle(int index);

signals:
    void connected();
    void disconnected();
    void calibrationAngle(int index, QVector2D angles);
    void calibrationFinished();

private:
    QVector2D encoderReading;
    QVector2D angle,angularVelocity,position,velocity;
    QVector2D force,torque;
    QVector2D A,B,C,D,E;
    QMatrix2x2 J;
    float a,b,c;
    Pantograph pantograph;

    QByteArray data;
    char dataIn[8],dataOut[4],buffer[8000];
    QTcpSocket *device;

    void angle2position();
    void force2torque();
    void encodeData();
    void decodeData();
    void updateJacobian();
    bool sendData();
};

#endif // HAPTICINTERFACE_H
