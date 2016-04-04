#ifndef VISUAL_H
#define VISUAL_H

#include<qsfmlcanvas.h>
#include<QVector2D>
#include<SFML/Graphics.hpp>
#include<physics.h>

class Visual : public QSfmlCanvas
{
public:
    Visual(QWidget* Parent, const QPoint& Position, const QSize& Size);
    void setPhysics(Physics physics);
    void startDisplay();
private:
    Physics physics_;
    sf::VertexArray polygon;
    void getObjectLocations();
    QVector2D convertCoordinates();
    void OnInit();
    void OnUpdate();
};

#endif // VISUAL_H
