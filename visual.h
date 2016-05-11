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
    void startCreationMode();
    void endCreationMode();

signals:
    void bodyClicked(int index);
    void createNewBody(b2Vec2 position, float radius);

private:
    Physics *physics;
    sf::VertexArray polygon;
    sf::CircleShape newBall;
    sf::RectangleShape newWall;
    sf::Transform physics2graphics;
    bool creationMode;
    float radius;
    void defineTransform(QSize windowSize);
    void getObjectLocations();
    QVector2D convertCoordinates();
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void wheelEvent(QWheelEvent *event);
    void OnInit();
    void OnUpdate();
};

#endif // VISUAL_H
