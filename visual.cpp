#include "visual.h"

Visual::Visual(QWidget* Parent, const QPoint& Position, const QSize& Size) :
    QSfmlCanvas(Parent, Position, Size)
{

}

void Visual::OnUpdate()
{
    for(int i(0);i<5;i++){
       polygon[i].position += sf::Vector2f(std::rand()%3-1,std::rand()%3-1);
    }
    draw(polygon);
}

void Visual::OnInit()
{
polygon = sf::VertexArray(sf::LinesStrip, 5);
for(int i(0);i<5;i++)
{
   polygon[i].position = sf::Vector2f(50,50)+sf::Vector2f(20*i,30*i);
}

}
