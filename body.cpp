#include "body.h"
#include<iostream>

Body::Body()
{
    invTimeStep = 100.f;
}

void Body::setType(int type)
{
    bodyType = type;
}
void Body::setTimeStep(float timeStep)
{
    invTimeStep = 1/timeStep;
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
    target.draw(vertices, states);
           //states.transform *= physics2graphics;


}

void Body::updatePosition()
{
    int nodeCount = nodeList.size();
    if(bodyType == BALL || bodyType == PLANE){
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        vertices[0].position = physics2graphics.transformPoint(p0);
        for(int i(1);i<nodeCount;i++)
        {
            sf::Vector2f p = convertPosition(nodeList[i]->GetPosition());
            //float norm = sqrt(p.x*p0.x + p.y*p0.y); //account for node radius
            //p = (p - p0)*(norm+nodeRadius)/norm + p0;
            vertices[i].position = physics2graphics.transformPoint(p);
            nodeList[i]->GetJointList();//TODO
            float energy(0);
            if(i>0)
            {
                for (b2JointEdge *jointEdge = nodeList[i]->GetJointList();
                     jointEdge; jointEdge=jointEdge->next)
                {
                   //energy +=jointEdge->joint->GetReactionForce(invTimeStep).LengthSquared();
                }
            }
            energy *= 255.f/MAX_ENERGY;
            if(energy > 255) energy = 255;
            vertices[i].color = sf::Color(uint8(energy),0,255-uint8(energy));
        }
        if(bodyType == BALL)
        {
            vertices[nodeCount].position = vertices[1].position;
            vertices[nodeCount].color = vertices[1].color;
        }
    }
    //TODO : solid wall
    if(bodyType == WALL)
    {
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        b2PolygonShape* wallShape = (b2PolygonShape*)nodeList[0]->GetFixtureList()[0].GetShape();
        for(int i(0);i<4;i++)
        {
            sf::Vector2f p = convertPosition( wallShape->GetVertex(i));
            vertices[i].position = physics2graphics.transformPoint(p+p0);
        }
        wallShape->~b2Shape();
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
