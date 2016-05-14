#include "visual.h"
#include <iostream>

Visual::Visual(QWidget* Parent) :
    QSfmlCanvas(Parent)
{
    physics = nullptr;
    setMouseTracking(true);
    radius = 2;
}

void Visual::OnUpdate()
{
    // Erase previous frame
    clear();
    // Draw the physical bodies' graphical representations
    // Test if simulation is already launched to avoid segmentation fault
    if(physics != nullptr)
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
            draw(newBall);
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
    // Reset previous transform by setting it to the identity
    physics2graphics = physics2graphics.Identity;
    // Transform based on hardcoded workspace dimensions - to be improved later
    sf::Vector2f center(windowSize.width()/2,windowSize.height()/2);
    sf::Vector2f scale(windowSize.width()/36,-windowSize.height()/22);
    physics2graphics.translate(center);
    physics2graphics.scale(scale);
    // Assign the transform to the physical simulation to be transmitted to
    // each body. Check if simulation already launched to avoid segfault
    if(physics != nullptr) physics->setTransform(physics2graphics);

}

// Detect and react to a click of the mouse's left button on the SFML display
void Visual::mousePressEvent(QMouseEvent *event)
{
    // Get the coordinates of the clicked pixel (origin on upper left corner)
    sf::Vector2f clickedPos(event->localPos().x(),event->localPos().y());
    // Convert it to Box2D coordinates
    clickedPos = physics2graphics.getInverse().transformPoint(clickedPos);
    if(creationMode){
        emit createNewBody(b2Vec2(clickedPos.x,clickedPos.y),radius);
    }
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
           // std::cout << physics->getBody(i).contains(clickedPos) << std::endl;
        }
        emit bodyClicked(bodyIndex);
    }

}

void Visual::mouseMoveEvent(QMouseEvent * event)
{
    if(creationMode)
    {
        newBall.setPosition(sf::Vector2f(event->localPos().x(),event->localPos().y()));
    }
}

void Visual::keyPressEvent(QKeyEvent * event)
{
    //event->key();
}

void Visual::resizeEvent(QResizeEvent *event)
{
    sf::RenderWindow::setSize(sf::Vector2u(event->size().width(),event->size().height()));
    setSize(sf::Vector2u(event->size().width(),event->size().height()));
    defineTransform(event->size());
}

void Visual::wheelEvent(QWheelEvent *event)
{
    std::cout << event->delta() << std::endl;
    if(creationMode)
    {
        radius +=((float)(event->delta())/1200);
        if(radius > 4) radius = 4;
        else if (radius < 1) radius = 1;
        newBall.setRadius(radius*(this->size().width()/36));
        newBall.setOrigin(sf::Vector2f(newBall.getRadius(),newBall.getRadius()));
    }
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

void Visual::setPhysics(Physics *newPhysics)
{
    physics = newPhysics;
    physics->createEntities();
    physics->setTransform(physics2graphics);
    physics->startSim();
}
