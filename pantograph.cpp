#include "pantograph.h"

Pantograph::Pantograph()
{

}

void Pantograph::geometricModel()
{
    float OAB((float)angle.x()),OED((float)angle.y());
    B = A+QVector2D(b*cos(OAB),b*sin(OAB));
    D = E+QVector2D(b*cos(OED),b*sin(OED));
    float BD((B-D).length());
    float BCD(acos(-(BD*BD-c*c - c*c)/(2*c*c)));
    float DBC(asin(c*sin(BCD)/BD));
    float thetaB(DBC + (float)atan((D.y()-B.y())/(D.x()-B.x())));
    C = B + QVector2D(c*cos(thetaB),c*sin(thetaB));
    if(position != C) kinematicModel();
    position = C;
}

void Pantograph::kinematicModel()
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
}

void Pantograph::nelderMead(float (*f)(QVector<float>),QVector<float> xStart)
{
    float alpha(0.4),beta(0.4),gamma(1.5);
    // Define the Nelder Mead polygon
    int nbDimensions(xStart.size());
    int nbVertices(nbDimensions+1);
    QVector<QVector<float>> X(nbVertices,QVector<float>(nbDimensions,0));
    // First vertex is the initial guess itself
    X[0] = xStart;
    // For each of the other vertices, change one dimension
    // of the initial guess
    for(int i(1); i<nbVertices; i++)
    {
        X[i] = xStart;
        // if x0 is 0 at that dimension, set the vertex to 0.10
        // mainly for the encoder offset values (in radians
        if(xStart[i-1] == 0) X[i][i-1] = 5;//0.10;
        // otherwise increase the value by 10% (lengths of the robot)
        else X[i][i-1] *= 1.1;
    }
    // Stop criteria - value of the function
    // (should be zero for a perfect match)
    // And number of iterations (both hardcoded for the moment)
    float error = 1;
    int maxIterations = 100;
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
        std::cout << "sorted " << yl << " " << ys << " " << yh << std::endl;
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
        // Evaluate function for the new point
        float yr(f(xr));
        // Test if new point is lower than the lowest of the vertices
        if(yr < yl)
        {
            std::cout << "Expansion" << std::endl;
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
            std::cout << "Contration" << std::endl;
            // Do a contraction
            QVector<float> xc(nbDimensions,0);
            for(int i(0); i<nbDimensions;i++)
                xc[i] = beta*X[h][i] + (1-beta)*x0[i];
            float yc(f(xc));
            if(yc < yh)
                X[h] = xc;
            else
                for(int i(0); i<nbVertices; i++)
                {
                    for(int j(0); j<nbDimensions;j++)
                        X[i][j] = (X[i][j] + X[l][j])/2;
                }
        }
        k++;
        std::cout << "Iteration " << k << " : " << std::endl;
        std::cout << "yl " <<  yl << std::endl;
        std::cout << "xl " <<  X[l][0]<<","<<  X[l][1]<<","<<  X[l][2] << std::endl;
        error = (yh-yl)*(yh-yl);

    }
}
