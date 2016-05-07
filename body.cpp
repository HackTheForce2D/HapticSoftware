#include "body.h"
#include<iostream>

Body::Body()
{
    id = rand()*rand();
    name = "object";
}

long int Body::getId() const
{
    return id;
}

void Body::setType(int type)
{
    bodyType = type;
}

QString Body::getName() const
{
    return name;
}
void Body::setName(QString newName)
{
    name = newName;
}

void Body::setSelected(bool selected)
{
    isSelected = selected;
}

void Body::setNodeRadius(float newNodeRadius)
{
    nodeRadius = newNodeRadius;
}

void Body::setTransform(sf::Transform transform)
{
    physics2graphics = transform;
}

//define drawable object once all vertices are entered
void Body::finish()
{
    int nodeCount = nodeList.size();
    switch(bodyType)
    {
    case BALL:      vertices = sf::VertexArray(sf::TrianglesFan,nodeCount+1);//(sf::Points,nodeCount+1);
                    break;
    case PLANE:     vertices = sf::VertexArray(sf::TrianglesStrip,nodeCount);
                    break;
    case WALL:      vertices = sf::VertexArray(sf::Quads,4);
                    break;
    }
}

void Body::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //states.texture = &m_texture;
    states.transform *= physics2graphics;
    target.draw(vertices, states);
}

void Body::updatePosition()
{
    int nodeCount = nodeList.size();
    if(bodyType == BALL || bodyType == PLANE){
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        if(isSelected) vertices[0].color = sf::Color::Green;
        else vertices[0].color = sf::Color::White;
       // vertices[0].position = physics2graphics.transformPoint(p0);
        vertices[0].position = p0;
        for(int i(1);i<nodeCount;i++)
        {
            sf::Vector2f p = convertPosition(nodeList[i]->GetPosition());
            //float norm = sqrt(p.x*p0.x + p.y*p0.y); //account for node radius
            float norm = sqrt((p.x-p0.x)*(p.x-p0.x) + (p.y-p0.y)*(p.y-p0.y)); //account for node radius
            p = (p - p0)*(norm+nodeRadius)/norm + p0;
            //vertices[i].position = physics2graphics.transformPoint(p);
            vertices[i].position = p;
            vertices[i].color = sf::Color::Blue;
        }
        if(bodyType == BALL)
        {
            vertices[nodeCount].position = vertices[1].position;
            vertices[nodeCount].color = vertices[1].color;
        }
    } else if(bodyType == WALL)
    {
        for(int i(0);i<4;i++)
        {
            if(isSelected) vertices[i].color = sf::Color::Green;
            else vertices[i].color = sf::Color::White;
        }
    }
    //TODO : solid wall
}

void Body::setWallPosition(float rotation)
//function only needs to be called once, since the wall won't move
{
    if(bodyType == WALL)
    {
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        rotateWall = sf::Transform::Identity;
        rotateWall.rotate(rotation,p0);
        b2PolygonShape* wallShape = (b2PolygonShape*)nodeList[0]->GetFixtureList()[0].GetShape();
        for(int i(0);i<4;i++)
        {
            sf::Vector2f p = convertPosition( wallShape->GetVertex(i));
            //vertices[i].position = physics2graphics.transformPoint(p+p0);
            vertices[i].position = rotateWall.transformPoint(p+p0);
        }
    }
}

void Body::addNode(b2Body* node)
{
   nodeList.append(node);
}

b2Body* Body::getNode(int index)
{
    return nodeList[index];
}

sf::Vector2f Body::convertPosition(b2Vec2 v)
{
    return sf::Vector2f(v.x,v.y);
}

bool Body::operator==(const Body& rhs)
{
    return(getId() == rhs.getId());
}

void Body::destroyNodes()
{
        size_t numberOfNodes(nodeList.size());
        std::cout << numberOfNodes << std::endl;
        for(size_t i(0); i<numberOfNodes; i++)
        {
           nodeList[i]->GetWorld()->DestroyBody(nodeList[i]);
        }
}

bool Body::contains(sf::Vector2f position)
{
    return vertices.getBounds().contains(position);
}
