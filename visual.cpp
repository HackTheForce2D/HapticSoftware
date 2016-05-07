#include "visual.h"
#include <iostream>

Visual::Visual(QWidget* Parent) :
    QSfmlCanvas(Parent)
{
    physics = nullptr;
    setMouseTracking(true);
}

void Visual::OnUpdate()
{
   // Body test = physics.getBody(0);
   // test.updatePosition();
    //polygon = test.getDrawable();
    //std::cout << "poly : " << polygon[10].position.x << " " << polygon[10].position.y << std::endl;

    clear();
    //draw(polygon);
   // TODO : get body positions using the pointers
    //physics.step();
    if(physics != nullptr)
    {
        physics->updateBodies();
        size_t workspBodyCount = physics->getWorkspWallCount();
        for(size_t i(0);i<workspBodyCount;i++)
        {
            draw(physics->getWorkspWall(i));
        }
        size_t bodyCount = physics->getBodyCount();
        for(size_t i(0);i< bodyCount;i++)
        {
            draw(physics->getBody(i));
        }
        draw(physics->getEffector());
    }
    //polygon[0].position = physics2graphics.transformPoint(sf::Vector2f(0,0));
    //polygon[1].position = physics2graphics.transformPoint(sf::Vector2f(-16,-10));
    //polygon[2].position = physics2graphics.transformPoint(sf::Vector2f(16,10));
    //polygon[3].position = sf::Vector2f(1,1);
   // draw(polygon);

}

void Visual::OnInit()
{
    defineTransform(this->size());
    polygon = sf::VertexArray(sf::Points, 4);
}

void Visual::defineTransform(QSize windowSize)
{
    physics2graphics = physics2graphics.Identity;
    std::cout << "window Size : " << windowSize.width() <<" "<< windowSize.height() <<std::endl;
    std::cout << "Window Position " << this->pos().x() << " "<< this->pos().y() << std::endl;
    std::cout << "Render size " << getSize().x << " "<< getSize().y << std::endl;
    sf::Vector2f center(windowSize.width()/2,windowSize.height()/2);
    sf::Vector2f scale(windowSize.width()/36,-windowSize.height()/22);
    physics2graphics.translate(center);
    physics2graphics.scale(scale);
    sf::Vector2f test = physics2graphics.transformPoint(sf::Vector2f(0,0));
    std::cout << "origin : " << test.x <<" "<< test.y <<std::endl;
    if(physics != nullptr) physics->setTransform(physics2graphics);

}

void Visual::mousePressEvent(QMouseEvent *event)
{
    sf::Vector2f clickedPos(event->localPos().x(),event->localPos().y());
    std::cout << clickedPos.x << " " << clickedPos.y << std::endl;
    clickedPos = physics2graphics.getInverse().transformPoint(clickedPos);
    size_t bodyCount = physics->getBodyCount();
    std::cout << clickedPos.x << " " << clickedPos.y << std::endl;
    for(size_t i(0);i< bodyCount;i++)
    {
        if(physics->getBody(i).contains(clickedPos))
        {
            emit bodyClicked(i);
        }
        std::cout << physics->getBody(i).contains(clickedPos) << std::endl;
    }


}

void Visual::mouseMoveEvent(QMouseEvent * event)
{
    //std::cout << event->screenPos().x() << event->screenPos().y()<< std::endl;
}

void Visual::resizeEvent(QResizeEvent *event)
{
    sf::RenderWindow::setSize(sf::Vector2u(event->size().width(),event->size().height()));
    setSize(sf::Vector2u(event->size().width(),event->size().height()));
    defineTransform(event->size());
}

void Visual::startCreationMode()
{
    creationMode = true;
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
