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
}

void HapticInterface::run()
    {
        connect(device, SIGNAL(readyRead()), this, SLOT(readData()));
        exec();
    }

void HapticInterface::setForce(QVector2D newForce)
{
    force = newForce;
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
    bool connected;
    std::cout << "Connecting" << std::endl;
    device->connectToHost(host,port);
    connect(device, SIGNAL(connected()), this, SLOT(reportConnected()));
    connect(device, SIGNAL(disconnected()), this, SLOT(reportDisconnected()));
    connected = device->waitForConnected(1000);
    std::cout << "Connected: " << connected << std::endl;
    return connected;
}

void HapticInterface::sendCalibrationAngle(int index)
{
    //emit calibrationAngle(index, encoderReading);
    pantograph.setCalibAngle(index,encoderReading);
    if(index == 3)
    {
        pantograph.calibrate();
        emit calibrationFinished();
    }
}

void HapticInterface::encodeData()
{
   int16_t encodedValue;
   data.clear();
   // Call the kinematic model to calculate the torque
   torque = pantograph.calculateTorque(force);
   encodedValue = (int16_t)(torque.x()+500);
   //Encode each component of the force as a 10-bit number
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
   encodedValue = (int16_t)(torque.y()+500);
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
}

bool HapticInterface::sendData()
{
    if(device->state() == QAbstractSocket::ConnectedState)
    {
        encodeData();
        //no need to write the size of the data because it's fixed (4 bytes)
        //so we just write the data itself to save time
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
         // If more than 8 bytes are available, store them in the buffer
         // and then take only the last 8 bytes to dataIn
         // to ignore outdated information
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

// Recover the angle measurements from the bytes array received
void HapticInterface::decodeData()
 {
     uint rawValue;
     qreal decodedValue;
     // Left bar angle is in <data[0] | data[1]> (12 out of 16 bits used)
     rawValue = (uchar)dataIn[0];
     rawValue = rawValue << 8 | (uchar)dataIn[1];
     encoderReading.setX(rawValue);
     decodedValue = ((qreal)rawValue)*2*pi/4096;
     if(decodedValue > pi) decodedValue -= 2*pi;
     angle.setX(decodedValue);
     // Right bar angle is in <data[2] | data[3]> (12 out of 16 bits used)
     rawValue = (uchar)dataIn[2];
     rawValue = rawValue << 8 | (uchar)dataIn[3];
     encoderReading.setY(rawValue);
     decodedValue = ((qreal)rawValue)*2*pi/4096;
     if(decodedValue > pi) decodedValue -= 2*pi;
     angle.setY(decodedValue);
     // Angular velocity is currently not measured nor taken into account
     // We should received 0 all the time but we're keeping the code to
     // evaluate the delay in the communication
     rawValue = dataIn[4];
     rawValue = rawValue << 8 | dataIn[5];
     angularVelocity.setX(((qreal)rawValue)*2*pi/4096);
     rawValue = data[6];
     rawValue = rawValue << 8 | dataIn[7];
     angularVelocity.setY(((qreal)rawValue)*2*pi/4096);
     position = pantograph.geometricModel(encoderReading);
 }

