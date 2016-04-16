#ifndef BODY_H
#define BODY_H

#include<SFML/Graphics.hpp>
#include<Box2D/Box2D.h>
#include<QVector>

const int WORKSPACE(0),BALL(1),PLANE(2),WALL(3);
const float MAX_ENERGY(2000);

class Body : public sf::Drawable
{
public:
    Body();
    void setType(int type);
    void setTimeStep(float timeStep);
    void setNodeRadius(float newNodeRadius);
    void addNode(b2Body* node);
    b2Body* getNode(int index);
    void finish();
    void setTransform(sf::Transform transform);
    void updatePosition();

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    int bodyType;
    float invTimeStep;
    float nodeRadius;
    sf::Vector2f convertPosition(b2Vec2 v);
    sf::VertexArray vertices;
    //sf::Texture m_texture;
    QVector<b2Body*> nodeList;
    sf::Transform physics2graphics;
};

#endif // BODY_H
