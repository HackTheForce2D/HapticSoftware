#include "hapticinterface.h"
#include <iostream>

HapticInterface::HapticInterface(QObject *parent) : QThread(parent)
{
    device = new QTcpSocket(this);
    a = 60;
    b = 100;
    c = 135;
    A=QVector2D(-a/2,10);
    B=QVector2D(0,0);
    C=QVector2D(0,0);
    D=QVector2D(0,0);
    E=QVector2D(a/2,10);
    //encoderOffset = 0,0;
}

//void run();
    void HapticInterface::run(){
    connect(device, &QIODevice::readyRead, this, &HapticInterface::readData);
    exec();
  }

void HapticInterface::setForce(QVector2D newForce)
{
    //std::cout << "updating force" << std::endl;
    force = newForce;
    updateTorque();
}

QVector2D HapticInterface::getPosition()
{
    return position;
}
QVector2D HapticInterface::getVelocity()
{
    return velocity;
}

bool HapticInterface::connectToHost(QString host,int port)
{
    device->connectToHost(host,port);
    connect(device, SIGNAL(connected()), this, SLOT(connected()));
    connect(device, SIGNAL(disconnected()), this, SLOT(disconnected()));
    return device->waitForConnected();
}

void HapticInterface::angle2position()
{
    float OAB((float)angle.x()),OED((float)angle.y());
    //std::cout << (OAB*180/pi) << " " << (OED*180/pi) << std::endl;
    B = A+QVector2D(b*cos(OAB),b*sin(OAB));
    D = E+QVector2D(b*cos(OED),b*sin(OED));
    //std::cout <<"D : "<< D.x() << " " << D.y() << std::endl;
    float BD((B-D).length());
    //std::cout <<"BD : "<< BD << std::endl;
    float BCD(acos(-(BD*BD-c*c - c*c)/(2*c*c)));
    float DBC(asin(c*sin(BCD)/BD));
    float thetaB(DBC + (float)atan((D.y()-B.y())/(D.x()-B.x())));
    C = B + QVector2D(c*cos(thetaB),c*sin(thetaB));
    //std::cout <<"C : "<< C.x() << " " << C.y() << std::endl;
    if(position != C) updateJacobian();
    position = C;
}
/*
QVector2D force2torque();

void decodeData();
int receiveData();
*/
void HapticInterface::updateJacobian()
{
    float denominator((C.y()-D.y())*(C.x()-B.x())-(C.x()-D.x())*(C.y()-B.y()));
    J(0,0) = -(C.y()-D.y())*((C.x()-B.x())*
              (C.y()-A.y())-(C.y()-B.y())*(C.x()-A.x()))/denominator;
    J(0,1) = -(C.y()-E.y())-(C.y()-D.y())*((C.x()-E.x())*
              (C.y()-B.y())-(C.y()-E.y())*(C.x()-B.x()))/denominator;
    J(1,0) = (C.x()-D.x())*((C.x()-B.x())*
             (C.y()-A.y())-(C.y()-B.y())*(C.x()-A.x()))/denominator;
    J(1,1) =  (C.x()-E.x())+(C.x()- D.x())*((C.x()-E.x())*
             (C.y()-B.y())-(C.y()-E.y())*(C.x()-B.x()))/denominator;
    //updateTorque();
}

void HapticInterface::updateTorque()
{
     torque.setX(-J(0,0)*force.x() + J(1,0)*force.y());
     torque.setY(-J(0,1)*force.x() + J(1,1)*force.y());
     if(abs(torque.x()) > 500 || abs(torque.y()) > 500)
     {
         torque = 500*(torque/(std::max(abs(torque.x()),abs(torque.y()))));
     }
     //temporary
     if(abs(force.x()) > 500 || abs(force.y()) > 500)
     {
         force = 500*(force/(std::max(abs(force.x()),abs(force.y()))));
     }
}

void HapticInterface::encodeData()
{
   //Encode each component of the force as a 10-bit number
   //float originalValue;
   int16_t encodedValue;
   data.clear();
   //originalValue = force.x();
   encodedValue = (int16_t)(torque.x()+500);
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
   encodedValue = (int16_t)(torque.y()+500);
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
   //

}
bool HapticInterface::sendData()
{
    if(device->state() == QAbstractSocket::ConnectedState)
    {
        encodeData();
        //no need to write the size of the data because it's fixed (4 bytes)
        //so we just write the data itself
        device->write(data);
        return device->waitForBytesWritten();
    }
    else
        return false;
}

void HapticInterface::reportConnected()
{
    emit connected();
    std::cout << "device connected" << std::endl;
}
void HapticInterface::reportDisconnected()
{
    emit disconnected();
    std::cout << "device disconnected" << std::endl;
}

void HapticInterface::disconnect()
{
    device->disconnectFromHost();
}

void HapticInterface::readData()
 {
     while(device->bytesAvailable() > 0)
     {
         size_t bytesAvailable(device->bytesAvailable());
         // if more than 8 bytes are available, store them in buffer
         // and then take only the last 8 bytes to dataIn
         // to ignore old information
         if(bytesAvailable!= 8){
             device->read(buffer,device->bytesAvailable());
             for(int i(0);i<8;i++)
             {
                 dataIn[i] = buffer[bytesAvailable-8+i];
             }
         }
         else
         {
             device->read(dataIn,8);
         }

       decodeData();
       sendData();
     }
 }


void HapticInterface::decodeData()
 {
     uint rawValue;
     qreal decodedValue;
     rawValue = (uchar)dataIn[0];
     rawValue = rawValue << 8 | (uchar)dataIn[1];
     decodedValue = ((qreal)rawValue)*2*pi/4096;
     if(decodedValue > pi) decodedValue -= 2*pi;
     angle.setX(decodedValue);
     rawValue = (uchar)dataIn[2];
     rawValue = rawValue << 8 | (uchar)dataIn[3];
     decodedValue = ((qreal)rawValue)*2*pi/4096;
     if(decodedValue > pi) decodedValue -= 2*pi;
     angle.setY(decodedValue);
     rawValue = dataIn[4];
     rawValue = rawValue << 8 | dataIn[5];
     angularVelocity.setX(((qreal)rawValue)*2*pi/4096);
     rawValue = data[6];
     rawValue = rawValue << 8 | dataIn[7];
     angularVelocity.setY(((qreal)rawValue)*2*pi/4096);
     angle2position();
     //std::cout << (angle.x()*180/pi) << " " << (angle.y()*180/pi) << std::endl;
     //std::cout << position.x() << " " << position.y()<< std::endl;
 }
/*
HapticInterface::~HapticInterface()
{
    exit(0);
    return 0;
}*/
