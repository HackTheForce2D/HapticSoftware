#include "visual.h"
#include <iostream>

Visual::Visual(QWidget* Parent) :
    QSfmlCanvas(Parent)
{

}

void Visual::OnUpdate()
{
    //test
    for(int i(0);i<19;i++){
       polygon[i].position += sf::Vector2f(std::rand()%3-1,std::rand()%3-1);
    }
    polygon[19].position = polygon[1].position;
   // Body test = physics.getBody(0);
   // test.updatePosition();
    //polygon = test.getDrawable();
    //std::cout << "poly : " << polygon[10].position.x << " " << polygon[10].position.y << std::endl;

    clear();
    //draw(polygon);
   // TODO : get body positions using the pointers
    //physics.step();
    physics.updateBodies();
    for(int i(0);i<physics.getBodyCount();i++)
    {
        draw(physics.getBody(i));
    }
    draw(physics.getEffector());
}

void Visual::OnInit()
{
    //test
polygon = sf::VertexArray(sf::TrianglesFan, 20);
for(int i(0);i<19;i++)
{
   polygon[i].position = sf::Vector2f(50,50)+sf::Vector2f(rand()%8*i,rand()%8*i);
   int colorChosen = rand()%3;
   if(colorChosen  == 0){
       polygon[i].color = sf::Color::Red;
   }else if(colorChosen  == 1){
     polygon[i].color = sf::Color::Green;
   }else{
     polygon[i].color = sf::Color::Blue;
   }
}
polygon[19].position = polygon[0].position;
polygon[19].color = polygon[0].color;
physics.createEntities();
defineTransform();
physics.run();
}

void Visual::defineTransform()
{
    QSize windowSize = this->size();
    physics2graphics = physics2graphics.Identity;
    physics2graphics.translate(windowSize.width()/2,windowSize.height()/2);
    physics2graphics.scale(windowSize.width()/33,-windowSize.height()/21);
    physics.setTransform(physics2graphics);
}

void Visual::resizeEvent(QResizeEvent * event)
{
    defineTransform();
}
