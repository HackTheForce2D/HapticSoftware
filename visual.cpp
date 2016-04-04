#include "visual.h"

Visual::Visual(QWidget* Parent) :
    QSfmlCanvas(Parent)
{

}

void Visual::OnUpdate()
{
    for(int i(0);i<19;i++){
       polygon[i].position += sf::Vector2f(std::rand()%3-1,std::rand()%3-1);
    }
    polygon[19].position = polygon[0].position;

    clear();
    draw(polygon);
}

void Visual::OnInit()
{
polygon = sf::VertexArray(sf::Triangles, 20);
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

}
