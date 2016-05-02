#ifndef EFFECTOR_H
#define EFFECTOR_H

#include<SFML/Graphics.hpp>
#include<Box2D/Box2D.h>
#include<QVector>
#include<QVector2D>
#include<body.h>


class Effector : public sf::Drawable
{
public:
    Effector();
    void setTransform(sf::Transform transform);
    void setTransformHaptic(sf::Transform transform);
    b2Vec2 getPosition();
    void setPhysical(b2Body * effectorPhysical);
    void setRadius(float newRadius);
    void updateGraphic();
    QVector2D updateForce(QVector2D position);
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void resetGraphical();
    float radius,kp,forceFactor;
    b2Vec2 force;
    sf::Vector2f convertPosition(b2Vec2 v);
    sf::Vector2f convertPosition(QVector2D v);
    b2Vec2 convertPosition(sf::Vector2f v);
    b2Body* physical;
    sf::CircleShape graphical;
    sf::Transform physics2graphics,device2physics;
};

#endif // EFFECTOR_H
