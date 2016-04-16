#include "effector.h"

Effector::Effector()
{
    graphical.setFillColor(sf::Color::Green);

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

void Effector::updatePosition()
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

sf::Vector2f Effector::convertPosition(b2Vec2 v)
{
    return sf::Vector2f(v.x,v.y);
}

void Effector::resetGraphical()
{
    graphical.setPosition(sf::Vector2f(0,0));
    graphical.setRadius(radius);
    graphical.setOrigin(sf::Vector2f(radius,radius));
}
