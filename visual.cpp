#include "visual.h"
#include <iostream>

// Workspace corners in pixels
// Not finished - maybe shouldn't set as constant
const QVector2D Visual::TOP_LEFT = QVector2D(10,10);
const QVector2D Visual::BOTTOM_RIGHT = QVector2D(300,480);

Visual::Visual(QWidget* Parent) :
    QSfmlCanvas(Parent)
{
   // physics = nullptr;
    //Initialize the Physics pointer as a null pointer
    physics = 0;
    setMouseTracking(true);
    radius = 2;
    newWallThickness = 1;
    newObjectIsCircle = true;
    stopUpdate = false;
    graphicalThickness = newWallThickness*(this->size().width()/36);
    calibrationPoint = -1;
    calibrationTarget.setFillColor(sf::Color::Red);
    calibrationTarget.setRadius(5);
    calibrationTarget.setOrigin(sf::Vector2f(5,5));
    workspaceBounds = sf::VertexArray(sf::LinesStrip,5);
    defineTransform(size());
}

//Check if a body is being deleted
void Visual::stopUpdating(bool stop)
{
   stopUpdate = stop;
   if(stopUpdate) std::cout << "Stopped display" << std::endl;
   else std::cout << "Resume display" << std::endl;
   physics->displayStopped(stop);
   //emit displayStopped(stop);
}

void Visual::OnUpdate()
{
    //Don't update if a body is being created or deleted
    if(stopUpdate) return;
    // Erase previous frame
    clear();
    draw(workspaceBounds);
    // Draw the calibration target if in calibration mode
    if(calibrationPoint != -1)
    {
        draw(calibrationTarget);
    }
    // Draw the physical bodies' graphical representations
    // Test if simulation is already launched to avoid segmentation fault
    else if(physics != 0)
    {
        // Update graphical position to match Box2D position
        physics->updateBodies();
        // Draw the workspace walls
        size_t workspBodyCount = physics->getWorkspWallCount();
        for(size_t i(0);i<workspBodyCount;i++)
        {
            draw(physics->getWorkspWall(i));
        }
        // Draw the other objects
        size_t bodyCount = physics->getBodyCount();
        for(size_t i(0);i< bodyCount;i++)
        {
            draw(physics->getBody(i));
        }
        // Draw the object controlled by the user
        // Drawn last so it's layered on top of everything else
        draw(physics->getEffector());
        // If user is creating a new object, draw the preview
        // on cursor position (more options to be added later)
        if(creationMode)
        {
            if(newObjectIsCircle)
            {
                draw(newBall);
            }else if(position1Set)
            {
                draw(newWall);
            }

        }
    }
}

void Visual::OnInit()
{
    defineTransform(this->size());
    polygon = sf::VertexArray(sf::Points, 4);
}

// Redefines the coordinate transform from the physical simulation
// to the display screen each time the display is resized
// (not working properly because SFML's setSize() function is unresponsive
void Visual::defineTransform(QSize windowSize)
{
    topLeft = sf::Vector2f(10,10);
    bottomRight = sf::Vector2f(windowSize.width()-10,windowSize.height()-10);
    workspaceBounds[0].position = sf::Vector2f(topLeft.x,topLeft.y);
    workspaceBounds[1].position = sf::Vector2f(topLeft.x,bottomRight.y);
    workspaceBounds[2].position = sf::Vector2f(bottomRight.x,
                                               bottomRight.y);
    workspaceBounds[3].position = sf::Vector2f(bottomRight.x,topLeft.y);
    workspaceBounds[4].position = workspaceBounds[0].position;

    // Reset previous transform by setting it to the identity
    physics2graphics = physics2graphics.Identity;
    // Transform based on hardcoded workspace dimensions - to be improved later
    sf::Vector2f center(windowSize.width()/2,windowSize.height()/2);
    sf::Vector2f scale(windowSize.width()/36,-windowSize.height()/22);
    sf::Vector2f centerPhysics = sf::Vector2f((Physics::TOP_LEFT.x +
                                               Physics::BOTTOM_RIGHT.x)/2,
                                               (Physics::TOP_LEFT.y +
                                               Physics::BOTTOM_RIGHT.y)/2);
    sf::Vector2f centerVisual = sf::Vector2f((topLeft.x +
                                              bottomRight.x)/2,
                                             (topLeft.y +
                                              bottomRight.y)/2);
    center = centerVisual - centerPhysics;
    scale.x = (topLeft.x - centerVisual.x)/
              (Physics::TOP_LEFT.x - centerPhysics.x);
    scale.y = (topLeft.y - centerVisual.y)/
              (Physics::TOP_LEFT.y - centerPhysics.y);
    physics2graphics.translate(center);
    physics2graphics.scale(scale);

    // Uncomment the following lines for debugging if the transform is not correct
    //std::cout << "Translation : " << center.x << " " << center.y << std::endl;
    //std::cout << "Scale : " << scale.x << " " << scale.y << std::endl;
    //std::cout << "Center : " << centerPhysics.x << " " << centerPhysics.y << std::endl;
    //sf::Vector2f testPoint= physics2graphics.transformPoint(Physics::TOP_LEFT);
    //std::cout << "Transformed top left : " << testPoint.x << " " << testPoint.y << std::endl;
    //testPoint= physics2graphics.transformPoint(Physics::BOTTOM_RIGHT);
    //std::cout << "Transformed bottom right : " << testPoint.x << " " << testPoint.y << std::endl;

    // Assign the transform to the physical simulation to be transmitted to
    // each body. Check if simulation already launched to avoid segfault
    if(physics != 0) physics->setTransform(physics2graphics);

}

// Detect and react to a click of the mouse's left button on the SFML display
void Visual::mousePressEvent(QMouseEvent *event)
{
    // Get the coordinates of the clicked pixel (origin on upper left corner)
    sf::Vector2f clickedPos(event->pos().x(),event->pos().y());
    // Convert it to Box2D coordinates
    clickedPos = physics2graphics.getInverse().transformPoint(clickedPos);
    // If the creation dialog is open, create a new body at this location
    if(creationMode){
        if(newObjectIsCircle)
        {
           emit createNewCircle(b2Vec2(clickedPos.x,clickedPos.y),radius);
        }else
        {
            if(position1Set)
            {
                position1 = physics2graphics.getInverse().transformPoint(position1);
                position2 = physics2graphics.getInverse().transformPoint(position2);
                graphicalThickness = newWallThickness;
                updateNewWall();
                emit createNewBox(b2Vec2(newWall.getPosition().x,
                                         newWall.getPosition().y) ,
                                  newWall.getRotation()*pi/180,
                                  b2Vec2(newWall.getSize().x/2,
                                         newWall.getSize().y/2));
                position1Set = false;
                graphicalThickness = newWallThickness*(this->size().width()/36);
            }else
            {
                position1 = sf::Vector2f(event->pos().x(),event->pos().y());
                std::cout << "Visual: box pos " << position1.x << " " << position1.y << std::endl;
                position1Set = true;
            }
        }
        // Update the transform for the newly created body
        //defineTransform(size());
    }
    // Otherwise, check if a body was clicked and select it
    // unselect all bodies if the user clicked on empty space (index = -1)
    else
    {
        size_t bodyCount = physics->getBodyCount();
        std::cout << clickedPos.x << " " << clickedPos.y << std::endl;
        int bodyIndex(-1);
        for(size_t i(0);i< bodyCount;i++)
        {
            if(physics->getBody(i).contains(clickedPos))
            {
                bodyIndex = i;
            }
        }
        emit bodyClicked(bodyIndex);
    }

}

void Visual::mouseMoveEvent(QMouseEvent * event)
{
    if(creationMode)
    {
        if(newObjectIsCircle)
        {
            newBall.setPosition(sf::Vector2f(event->pos().x(),event->pos().y()));
        }else if(position1Set)
        {
            position2 = sf::Vector2f(event->pos().x(),event->pos().y());
            updateNewWall();
        }
    }
}

void Visual::newCircle()
{
    newObjectIsCircle = true;
    position1Set = false;
}

void Visual::newBox()
{
    newObjectIsCircle = false;
    position1Set = false;
}

void Visual::keyPressEvent(QKeyEvent * event)
{
    if(calibrationPoint != -1)
    {
        emit calibrationPointEntered(calibrationPoint);
        nextCalibrationPoint();
    }
    else if(event->key() == Qt::Key_Delete)
    {
        emit deletePressed();
    }

}

void Visual::resizeEvent(QResizeEvent *event)
{
    sf::RenderWindow::setSize(sf::Vector2u(event->size().width(),event->size().height()));
    setSize(sf::Vector2u(event->size().width(),event->size().height()));
    defineTransform(event->size());
}

void Visual::wheelEvent(QWheelEvent *event)
{
    //std::cout << event->delta() << std::endl;
    if(creationMode)
    {
        if(newObjectIsCircle)
        {
            radius +=((float)(event->delta())/1200);
            if(radius > 4) radius = 4;
            else if (radius < 1) radius = 1;
            newBall.setRadius(radius*(this->size().width()/36));
            newBall.setOrigin(sf::Vector2f(newBall.getRadius(),newBall.getRadius()));
        }else
        {
            newWallThickness +=((float)(event->delta())/1200);
            graphicalThickness = newWallThickness*(this->size().width()/36);
            if(newWallThickness > 4) newWallThickness = 4;
            else if (newWallThickness < 1) newWallThickness = 1;
            updateNewWall();
        }

    }
}

void Visual::updateNewWall()
{
    //position2 = b2Vec2(clickedPos.x,clickedPos.y);
    newWallPos.x = (position1.x + position2.x)/2;
    newWallPos.y = (position1.y + position2.y)/2;
    newWall.setPosition(newWallPos);
    newWall.setRotation((180/pi)*atan2(position2.y - position1.y,
                              position2.x - position1.x));
    newWallLength = sqrt(pow(position2.x - position1.x,2)+
                         pow(position2.y - position1.y,2));    
    newWall.setSize(sf::Vector2f(newWallLength,graphicalThickness));
    newWall.setOrigin(sf::Vector2f(newWallLength/2,
                                   graphicalThickness/2));
}

// Toggled if user opens the Add Object dialog window
void Visual::startCreationMode()
{
    creationMode = true;
    newBall.setRadius(radius*(this->size().width()/36));
    newBall.setOrigin(sf::Vector2f(newBall.getRadius(),newBall.getRadius()));
}

void Visual::endCreationMode()
{
    creationMode = false;
}

void Visual::startCalibrationMode()
{
    calibrationPoint = 0;
    physics->stopSim();
    calibrationTarget.setPosition(workspaceBounds[calibrationPoint].position);
}

void Visual::nextCalibrationPoint()
{
    calibrationPoint++;
    if(calibrationPoint < 4)
    {
        calibrationTarget.setPosition
                (workspaceBounds[calibrationPoint].position);
    }else endCalibrationMode();
}

void Visual::endCalibrationMode()
{
    calibrationPoint = -1;
    emit readyToCalibrate();
}


void Visual::setPhysics(Physics *newPhysics)
{
    physics = newPhysics;
    physics->setTransform(physics2graphics);
    physics->startSim();
}
