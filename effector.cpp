#include "effector.h"
#include <iostream>

Effector::Effector()
{
    graphical.setFillColor(sf::Color::Green);
    device2physics = device2physics.Identity;
    device2physics.rotate(180,sf::Vector2f(0,0));
    device2physics.translate(sf::Vector2f(0,-20));
    device2physics.scale(sf::Vector2f(-.2,.2),sf::Vector2f(0,0));

}

void Effector::setTransformHaptic(sf::Transform transform)
{
    device2physics = transform;
}

void Effector::setTransform(sf::Transform transform)
{
    physics2graphics = transform;
}

void Effector::setPhysical(b2Body * effectorPhysical)
{
    physical = effectorPhysical;
}
void Effector::setRadius(float newRadius)
{
    radius = newRadius;
}

void Effector::updateGraphic()
{
    sf::Vector2f p = convertPosition(physical->GetPosition());
    resetGraphical();
    graphical.setPosition(p);
}

void Effector::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform = physics2graphics;
    target.draw(graphical, states);
}

void Effector::resetGraphical()
{
    graphical.setPosition(sf::Vector2f(0,0));
    graphical.setRadius(radius);
    graphical.setOrigin(sf::Vector2f(radius,radius));
}

QVector2D Effector::updateForce(QVector2D position)
{
    sf::Vector2f positionSf(convertPosition(position));
    positionSf = device2physics.transformPoint(positionSf);
    b2Vec2 positionb2(convertPosition(positionSf));
    //std::cout << positionb2.x << " " << positionb2.y << std::endl;
    //Proportional control
    kp = 300;
    force = kp*(positionb2 - physical->GetPosition());
    physical->ApplyForce(force,physical->GetWorldCenter(),1);
    //return the reaction to the force as a QVector2D
    //to be used by the interface with the device
    return (QVector2D(-force.x,-force.y));

}

sf::Vector2f Effector::convertPosition(b2Vec2 v)
{
    return sf::Vector2f(v.x,v.y);
}

sf::Vector2f Effector::convertPosition(QVector2D v)
{
    return sf::Vector2f(v.x(),v.y());
}

b2Vec2 Effector::convertPosition(sf::Vector2f v)
{
    return b2Vec2(v.x,v.y);
}
