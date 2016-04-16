#include "hapticinterface.h"

HapticInterface::HapticInterface(QObject *parent) : QObject(parent)
{
    device = new QTcpSocket(this);
    a = 40;
    b = 100;
    c = 125;
}

void HapticInterface::setForce(QVector2D force)
{
    force = force;
}

bool HapticInterface::connectToHost(QString host)
{
    device->connectToHost(host, 53000);
    return device->waitForConnected();
}

QVector2D HapticInterface::angle2position()
{
    float OAB((float)angle.x()),OED((float)angle.y());
    B = A+QVector2D(b*cos(OAB),b*sin(OAB));
    D = E+QVector2D(b*cos(OED),b*sin(OED));
    float BD((B-D).length());
    float BCD(acos(-(BD*BD-c*c - c*c)/(2*c*c)));
    float DBC(asin(c*sin(BCD)/BD));
    float thetaB(DBC + (float)atan((D.x()-B.y())/(D.x()-B.x())));
    C = B + QVector2D(c*cos(thetaB),c*sin(thetaB));
    position = C;
}
/*
QVector2D force2torque();

void decodeData();
int receiveData();
*/
void HapticInterface::encodeData()
{
   //data.
}
bool HapticInterface::sendData()
{
    if(device->state() == QAbstractSocket::ConnectedState)
    {
        encodeData();
        //device->write(IntToArray(data.size())); //write size of data
        device->write(data); //write the data itself
        return device->waitForBytesWritten();
    }
    else
        return false;
}

void HapticInterface::readyRead()
 {
     while(device->bytesAvailable() > 0)
     {
         data.append(device->readAll());
         decodeData();
     }
 }

void HapticInterface::decodeData()
 {
     int rawValue;
     rawValue = data[0];
     rawValue = rawValue << 8 | data[1];
     angle.setX(((qreal)rawValue)*2*pi/4096);
     rawValue = data[2];
     rawValue = rawValue << 8 | data[3];
     angle.setY(((qreal)rawValue)*2*pi/4096);
     rawValue = data[4];
     rawValue = rawValue << 8 | data[5];
     angularVelocity.setX(((qreal)rawValue)*2*pi/4096);
     rawValue = data[6];
     rawValue = rawValue << 8 | data[7];
     angularVelocity.setY(((qreal)rawValue)*2*pi/4096);
     data.clear();
 }
