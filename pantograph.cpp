#include "pantograph.h"

const sf::Vector2f Pantograph::TOP_LEFT = sf::Vector2f(-177/2,70);
const sf::Vector2f Pantograph::BOTTOM_RIGHT = sf::Vector2f(177/2,170);
const int Pantograph::ENCODER_MAX_VAL = 4096;
const float Pantograph::ENCODER_TO_RAD = 2*(3.1415926)/ENCODER_MAX_VAL;
const float Pantograph::MAX_TORQUE = 500;

Pantograph::Pantograph()
{
    a = 50;
    b = 80;
    c = 125;
    //b1 = 100;
    //b2 = 100;
    //c1 = 135;
    //c2 = 135;
    //Nelder-Mead can't handle the encoder offsets
    offset1 = 0;
    offset2 = 0;

    A=QVector2D(-a/2,10);
    B=QVector2D(0,0);
    C=QVector2D(0,0);
    D=QVector2D(0,0);
    E=QVector2D(a/2,10);

    calibrationAngles = QVector<QVector2D>(4,QVector2D(0,0));
    calibrationCoordinates = QVector<QVector2D>(4,QVector2D(0,0));
    calibrationCoordinates[0] = QVector2D(TOP_LEFT.x,TOP_LEFT.y);
    calibrationCoordinates[1] = QVector2D(TOP_LEFT.x,BOTTOM_RIGHT.y);
    calibrationCoordinates[2] = QVector2D(BOTTOM_RIGHT.x,BOTTOM_RIGHT.y);
    calibrationCoordinates[3] = QVector2D(BOTTOM_RIGHT.x,TOP_LEFT.y);
}

QVector2D Pantograph::geometricModel(QVector2D sensorReading)
{
    QVector2D angle;
    angle.setX((((int)(sensorReading.x() + offset1))
                %ENCODER_MAX_VAL)*ENCODER_TO_RAD);
    angle.setY((((int)(sensorReading.y() + offset2))
                %ENCODER_MAX_VAL)*ENCODER_TO_RAD);
    A=QVector2D(-a/2,10);
    E=QVector2D(a/2,10);
    float OAB((float)angle.x()),OED((float)angle.y());
    B = A+QVector2D(b*cos(OAB),b*sin(OAB));
    D = E+QVector2D(b*cos(OED),b*sin(OED));
    float BD((B-D).length());
    float BCD(acos(-(BD*BD-c*c - c*c)/(2*c*c)));
    float DBC(asin(c*sin(BCD)/BD));
    float thetaB(DBC + (float)atan((D.y()-B.y())/(D.x()-B.x())));
    C = B + QVector2D(c*cos(thetaB),c*sin(thetaB));
    return C;
}

QMatrix2x2 Pantograph::kinematicModel()
{
    QMatrix2x2 J;
    float denominator((C.y()-D.y())*(C.x()-B.x())-(C.x()-D.x())*(C.y()-B.y()));
    J(0,0) = -(C.y()-D.y())*((C.x()-B.x())*
              (C.y()-A.y())-(C.y()-B.y())*(C.x()-A.x()))/denominator;
    J(0,1) = -(C.y()-E.y())-(C.y()-D.y())*((C.x()-E.x())*
              (C.y()-B.y())-(C.y()-E.y())*(C.x()-B.x()))/denominator;
    J(1,0) = (C.x()-D.x())*((C.x()-B.x())*
             (C.y()-A.y())-(C.y()-B.y())*(C.x()-A.x()))/denominator;
    J(1,1) =  (C.x()-E.x())+(C.x()- D.x())*((C.x()-E.x())*
             (C.y()-B.y())-(C.y()-E.y())*(C.x()-B.x()))/denominator;
    return J;
}

QVector2D Pantograph::calculateTorque(QVector2D force)
{
    QMatrix2x2 J(kinematicModel());
    QVector2D torque;
    torque.setX(-J(0,0)*force.x() + J(1,0)*force.y());
    torque.setY(-J(0,1)*force.x() + J(1,1)*force.y());
    if(abs(torque.x()) > MAX_TORQUE || abs(torque.y()) > MAX_TORQUE)
    {
        torque = MAX_TORQUE*(torque/
                             (std::max(abs(torque.x()),abs(torque.y()))));
    }
    return torque;
}

void Pantograph::setCalibAngle(int index, QVector2D angles)
{
    //if(index >=0 && index < 4)
    calibrationAngles[index] = angles;
}

float Pantograph::geometricModelError(QVector<float> parameters)
{
    a = parameters[0];
    b = parameters[1];
    c = parameters[2];
    offset1 = parameters[3];
    offset2 = parameters[4];
    float error(0), errorPoint(0);
    for(int i(0); i<4; i++)
    {
        QVector2D sensorReading = calibrationAngles[i];
        QVector2D position(geometricModel(sensorReading));
        errorPoint += (calibrationCoordinates[i] - position).length();
        error += errorPoint*errorPoint;
    }
    error = sqrt(error);
    return error;
}

float Pantograph::f(QVector<float> parameters)
{
    return geometricModelError(parameters);
}

void Pantograph::calibrate()
{
    std::cout << "calibrate() called" << std::endl;
    QVector<float> parameters(5,0);
    parameters[0] = a;
    parameters[1] = b;
    parameters[2] = c;
    parameters[3] = offset1;
    parameters[4] = offset2;
    nelderMead(parameters);
    /*parameters[0] = a;
    parameters[1] = b1;
    parameters[2] = b2;
    parameters[3] = c1;
    parameters[4] = c2;
    parameters[5] = offset1;
    parameters[6] = offset2;*/
}

void Pantograph::nelderMead(QVector<float> xStart)
{
    float alpha(1),beta(0.5),gamma(2.0);
    // Define the Nelder Mead polygon
    int totalIterations = 0;
    int nbDimensions(xStart.size());
    int nbVertices(nbDimensions+1);
    QVector<QVector<float>> X(nbVertices,QVector<float>(nbDimensions,0));
    QVector<float> xBest(nbDimensions,0);
    float error2 = 1;
    int maxIterations2 = 5000;
    int k2(0);
    while(k2 < maxIterations2 && error2 > .0001 )
    {
        // First vertex is the initial guess itself
        X[0] = xStart;
        // For each of the other vertices, change one dimension
        // of the initial guess
        for(int i(1); i<nbVertices; i++)
        {
            X[i] = xStart;
            // if x0 is 0 at that dimension, set the vertex to 0.10
            // mainly for the encoder offset values (in radians
            //if(xStart[i-1] == 0) X[i][i-1] = 0.10;
            // otherwise increase the value by 10% (lengths of the robot)
            //else X[i][i-1] *= 1.1;
            X[i][i-1] += 0.50;
        }
        // Stop criteria - value of the function
        // (should be zero for a perfect match)
        // And number of iterations (both hardcoded for the moment)
        float error = 1;
        int maxIterations = 50;
        int k(0);
        while (k< maxIterations && error > .0001)
        {
            QVector<float> y(nbVertices,0);
            QVector<int> index(nbVertices,0);
            // Evaluate the function at the vertices
            for(int i(0); i<nbVertices; i++)
            {
                y[i] = f(X[i]);
                // the algorithm also needs the indices of the sorted array
                index[i] = i;
            }
            // Sorting function taken from Stack Overflow
            // http://stackoverflow.com/questions/17554242/
            std::sort(index.begin(), index.end(),
                [&](const int& a, const int& b) {
                    return (y[a] < y[b]);
                }
            );
            // index of the highest value of the function
            int h = index[index.size()-1];
            float yh(y[h]);
            // index of the second highest value of the function
            int s = index[index.size()-2];
            float ys(y[s]);
            // index of the lowest value of the function
            int l = index[0];
            float yl(y[l]);
            // barycenter of the points excluding that with the highest value

            QVector<float> x0(nbDimensions,0);
            for(int i(0); i<nbVertices; i++)
            {
                if(i != h)
                {
                    for(int j(0); j<nbDimensions;j++)
                        x0[j] += X[i][j]/(nbVertices-1);
                }
            }
            // Mirror X[h] with respect to the barycenter to find the new point xr
            QVector<float> xr(nbDimensions,0);
            for(int i(0); i<nbDimensions;i++)
                xr[i] = (1+alpha)*x0[i] - alpha*X[h][i];
            // Evaluate function at the new point
            float yr(f(xr));
            // Test if new point is lower than the lowest of the vertices
            if(yr < yl)
            {
                // Do an expansion in the same direction
                QVector<float> xe(nbDimensions,0);
                for(int i(0); i<nbDimensions;i++)
                    xe[i] = gamma*xr[i] + (1-gamma)*x0[i];
                float ye(f(xe));
                // If expansion finds an even lower value, keep it in place of xh
                if(ye < yr)
                {
                    X[h] = xe;
                }
                // Otherwise, keep the original reflected value
                else
                {
                    X[h] = xr;
                }
            }
            // If reflected value is not lower than the lowest value
            else if(yr < ys)
            {
                X[h] = xr;
            }
            else
            {
                if(yr < yh)
                {
                    X[h] = xr;
                }
                // Do a contraction
                QVector<float> xc(nbDimensions,0);
                for(int i(0); i<nbDimensions;i++)
                    xc[i] = beta*X[h][i] + (1-beta)*x0[i];
                float yc(f(xc));
                if(yc < yh)
                {
                    X[h] = xc;
                }
                else
                {
                    for(int i(0); i<nbVertices; i++)
                    {

                        for(int j(0); j<nbDimensions;j++)
                            X[i][j] = (X[i][j] + X[l][j])/2;
                    }
                }
            }
            k++;
            totalIterations++;
            xBest = X[l];
            error = (yh-yl)*(yh-yl);
            error2 = yl;
        }
        // Change the initial guess to the best point found and restart
        xStart = xBest;
        k2++;
    }
    std::cout << "Solution : ";
    for(int j(0); j<nbDimensions;j++) std::cout << xBest[j] << ", ";
    std::cout << std::endl;
    std::cout << "Total iterations : " << totalIterations << std::endl;
    std::cout << "Error: " << f(xBest) << std::endl;
}
