#include "pantograph.h"

const sf::Vector2f Pantograph::TOP_LEFT = sf::Vector2f(-177/2,70);
const sf::Vector2f Pantograph::BOTTOM_RIGHT = sf::Vector2f(177/2,170);
const int Pantograph::ENCODER_MAX_VAL = 4096;
// Constant to convert the encoder reading to an angle in radians
const float Pantograph::ENCODER_TO_RAD = 2*(3.1415926)/ENCODER_MAX_VAL;
// Maximum torque of the motor with reductor in mNm
const float Pantograph::MAX_TORQUE = 500;

Pantograph::Pantograph()
{
    // Initialize the pantograph's parameters at default values
    a = 50;
    b = 80;
    c = 125;
    //b1 = 100;
    //b2 = 100;
    //c1 = 135;
    //c2 = 135;
    //Nelder-Mead can't handle the encoder offsets
    //offset1 = 3090;
    //offset2 = 1530;
    offset1 = 0;
    offset2 = 0;

    A=QVector2D(-a/2,10);
    B=QVector2D(0,0);
    C=QVector2D(0,0);
    D=QVector2D(0,0);
    E=QVector2D(a/2,10);

    // Vector to store the angles read when the user places the pantograph
    // in each of the four corners of the workspace for calibration
    calibrationAngles = QVector<QVector2D>(4,QVector2D(0,0));
    // Vector to store the coordinates of the four corners of the workspace
    calibrationCoordinates = QVector<QVector2D>(4,QVector2D(0,0));
    calibrationCoordinates[0] = QVector2D(TOP_LEFT.x,TOP_LEFT.y);
    calibrationCoordinates[1] = QVector2D(TOP_LEFT.x,BOTTOM_RIGHT.y);
    calibrationCoordinates[2] = QVector2D(BOTTOM_RIGHT.x,BOTTOM_RIGHT.y);
    calibrationCoordinates[3] = QVector2D(BOTTOM_RIGHT.x,TOP_LEFT.y);
}

// Converts the signal sent by the encoders to a position in the workspace
// based on the geometry of the pantograph
QVector2D Pantograph::geometricModel(QVector2D encoderReading)
{
    // Convert the encoder reading in ticks to radians per second
    QVector2D angle;
    angle.setX((((int)(encoderReading.x() + offset1))
                %ENCODER_MAX_VAL)*ENCODER_TO_RAD);
    angle.setY((((int)(encoderReading.y() + offset2))
                %ENCODER_MAX_VAL)*ENCODER_TO_RAD);
    // Place the static nodes (motors or reductors)
    A=QVector2D(-a/2,10);
    E=QVector2D(a/2,10);
    float OAB((float)angle.x()),OED((float)angle.y());
    B = A+QVector2D(b*cos(OAB),b*sin(OAB));
    D = E+QVector2D(b*cos(OED),b*sin(OED));
    float BD((B-D).length());
    float BCD(acos(-(BD*BD-c*c - c*c)/(2*c*c)));
    if(std::isnan(BCD)) return QVector2D(0,0);
    float DBC(asin(c*sin(BCD)/BD));
    if(std::isnan(DBC)) return QVector2D(0,0);
    float thetaB(DBC + (float)atan((D.y()-B.y())/(D.x()-B.x())));
    if(std::isnan(thetaB)) return QVector2D(0,0);
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
    geneticAlgorithm(parameters);
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

void Pantograph::geneticAlgorithm(QVector<float> xStart)
{
    std::cout << "Start" << std::endl;
    float d(0.25), p(1.25), mutationRate(0.10), mutationDelta(2);
    int k(0), stallGenCount(0), stallGenLimit(15),N(30);
    int nbDimensions(xStart.size());
    const int maxIterations(50000);
    //const int maxIterations(1);
    QVector<QVector<float>> P(N,QVector<float>(nbDimensions,0));
    QVector<float> xBest(nbDimensions,0);
    QVector<int> R(N,0); // rank
    QVector<float> F(N,0); // adaptive force
    float Fsum(0);
    for(int i(0); i< N; i++)
    {
        R[i] = i;
        float fi = 2-p + 2*(p-1)*((float)(R[i]-1)/(N-1));
        F[i] = Fsum + fi;
        Fsum += fi;
        //std::cout << "Rank " << R[i] <<" adaptive force : " << fi << std::endl;
        for(int j(0); j<nbDimensions; j++)
        {
            if(j < 3) P[i][j] = (float)(rand()%100) + 40;
            else P[i][j] = (float)(rand()%4096);
        }
    }
    while(stallGenCount < stallGenLimit && k < maxIterations)
    {
        // Selection
        //Evaluate the function at the population
        //std::cout << "Selection" << std::endl;
        QVector<float> y(N,0);
        QVector<int> index(N,0);
        for(int i(0); i< N; i++)
        {
            y[i] = f(P[i]);
            index[i] = i;
        }
        // Sort the costs in ascending order
        std::sort(index.begin(), index.end(),
                  [&](const int& a, const int& b) {
                  return (y[a] > y[b]);
        });
        // Generate intermediate population M by weighed random selection
        QVector<QVector<float>> M(N,QVector<float>(nbDimensions,0));
        for(int i(0); i< N; i++)
        {
            int dice(rand()%((int)Fsum*10000));
            int j(0);
            //std::cout << dice << " " << F[j]*10000 << std::endl;
            while(dice > F[j]*10000) j++;
            //std::cout << " New element is " << j << std::endl;
            M[i] = P[index[j]];
        }
        // Crossover
        //std::cout << "crossover" << std::endl;
        QVector<QVector<float>> Mx(N,QVector<float>(nbDimensions,0));
        for(int i(0); i< N; i++)
        {
            int k1(rand()%N),k2(rand()%N);
            //std::cout << k1 << " " << k2 << std::endl;
            for(int j(0); j<nbDimensions; j++)
            {
                float alpha = (rand()%10000 - 5000)*d/10000;
                Mx[i][j] = (1-alpha)*M[k1][j] + alpha*M[k2][j];
            }
        }
        // Mutation
        for(int i(0); i< N; i++)
        {
            for(int j(0); j<nbDimensions; j++)
            {
                // Add a random value between -mutationDelta and mutationDelta
                // to a few of the individuals chosen at random
                bool mutate((10000*mutationRate) > (rand()%10000));
                if (mutate) Mx[i][j] +=(rand()%10000 - 5000)*
                                        mutationDelta/10000;
            }
        }
        // Make new population
        // keep only the best individual of the previous population
        // (last index) to avoid backtracking
        for(int i(0); i < N-1; i++)
        {
            P[index[i]] = Mx[i];
        }
        xBest = P[index[0]];
        k++;
    }
    std::cout << "Solution : ";
    for(int j(0); j<nbDimensions;j++) std::cout << xBest[j] << ", ";
    std::cout << std::endl;
    std::cout << "Total iterations : " << k << std::endl;
    // When we evaluate the function, the value of xBest is copied to the
    // parameters of the pantograph
    std::cout << "Error: " << f(xBest) << std::endl;
}


//----------------------Nelder Mead-----------------------------
void Pantograph::nelderMead(QVector<float> xStart)
{
    float alpha(1),beta(0.5),gamma(2.0);
    // Define the Nelder Mead simplex
    int totalIterations = 0;
    int nbDimensions(xStart.size());
    int nbVertices(nbDimensions+1);
    QVector<QVector<float>> X(nbVertices,QVector<float>(nbDimensions,0));
    QVector<float> xBest(nbDimensions,0);
    float error2 = 1;
    // We restart the algorithm after 50 iterations, up to 5000 times
    // to avoid converging to a non-minimal point
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
