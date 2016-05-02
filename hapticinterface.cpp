#include "hapticinterface.h"
#include <iostream>

HapticInterface::HapticInterface(QObject *parent) : QThread(parent)
{
    device = new QTcpSocket(this);
    a = 40;
    b = 100;
    c = 125;
    A=QVector2D(-a/2,10);B=QVector2D(0,0);C=QVector2D(0,0);
    D=QVector2D(0,0);E=QVector2D(a/2,10);
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
}

QVector2D HapticInterface::getPosition()
{
    return position;
}
QVector2D HapticInterface::getVelocity()
{
    return velocity;
}

bool HapticInterface::connectToHost(QString host)
{
    device->connectToHost(host, 53200);
    connect(device, SIGNAL(connected()), this, SLOT(reportState()));
    connect(device, SIGNAL(disconnected()), this, SLOT(reportState()));
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
    position = C;
}
/*
QVector2D force2torque();

void decodeData();
int receiveData();
*/
void HapticInterface::encodeData()
{
   //Encode each component of the force as a 10-bit number
   //float originalValue;
   int16_t encodedValue;
   data.clear();
   //originalValue = force.x();
  // force.setX(-200); force.setY(-600);
   encodedValue = (int16_t)(force.x());
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
   encodedValue = (int16_t)(force.y());
   data.append((uchar)((encodedValue>>8)&255));
   data.append((uchar)((encodedValue)&255));
   //

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

void HapticInterface::reportState()
{
    std::cout << "device connected" << std::endl;
}

void HapticInterface::readData()
 {
    //std::cout << "reading... " << " " << device->bytesAvailable() << std::endl;
    //device->disconnectFromHost();
    //data.clear();
     while(device->bytesAvailable() > 0)
     {
         //std::cout << "received"<<  device->bytesAvailable() << std::endl;
         //data.append(device->readAll());
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
int HapticInterface::~HapticInterface()
{
    exit(0);
    return 0;
}*/
