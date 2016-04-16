#ifndef EFFECTOR_H
#define EFFECTOR_H

#include<SFML/Graphics.hpp>
#include<Box2D/Box2D.h>
#include<QVector>
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
    void updatePosition();
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void resetGraphical();
    float radius;
    sf::Vector2f convertPosition(b2Vec2 v);
    b2Body* physical;
    sf::CircleShape graphical;
    sf::Transform physics2graphics,device2physics;
};

#endif // EFFECTOR_H
