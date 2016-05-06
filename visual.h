#ifndef VISUAL_H
#define VISUAL_H

#include<qsfmlcanvas.h>
#include<QVector2D>
#include<SFML/Graphics.hpp>
#include<physics.h>
#include<QResizeEvent>

class Visual : public QSfmlCanvas
{
    Q_OBJECT
public:
    Visual(QWidget* Parent);
    void setPhysics(Physics *newPhysics);
    void startDisplay();

public slots:

private:
    Physics *physics;
    sf::VertexArray polygon;
    sf::Transform physics2graphics;
    void defineTransform(QSize windowSize);
    void getObjectLocations();
    QVector2D convertCoordinates();
    void resizeEvent(QResizeEvent * event);
    void OnInit();
    void OnUpdate();
};

#endif // VISUAL_H
