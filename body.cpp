#include "body.h"
#include<iostream>

/* Class that represents a physical body in the Box2D simulation as well as
 * its graphical representation.
 * Needs to be refactored to include the creation of the object
 * (to lighten the Physics class and improve encapsulation) and also needs to
 * be split into a few subclasses for the various types of objects
*/
Body::Body()
{
    id = rand()*rand();
    name = "object";
    isSelected = false;
}

// Id allows two Body instances to be compared
// needed for the delete operation
long int Body::getId() const
{
    return id;
}

void Body::setType(int type)
{
    bodyType = type;
}

// Names of the objects are displayed in the objectList widget
QString Body::getName() const
{
    return name;
}

void Body::setName(QString newName)
{
    name = newName;
}

// If an object is selected, it's painted in a different color
void Body::setSelected(bool selected)
{
    isSelected = selected;
}

// Radius of the small rigid object that compose an elastic object
// We use it to correct the shape of the drawable so we don't have
// empty space between objects that are touching
void Body::setNodeRadius(float newNodeRadius)
{
    nodeRadius = newNodeRadius;
}

// Set the transform that converts Box2D coordinates to graphical
// display coordinates
void Body::setTransform(sf::Transform transform)
{
    physics2graphics = transform;
}

//define drawable object once all vertices are entered, based on object type
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

// function that gets called when the display draws the Body instance
void Body::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // No textures defined currently
    //states.texture = &m_texture;
    // Transform
    states.transform *= physics2graphics;
    target.draw(vertices, states);
}

// Update the position of the graphical object to match that of the physical
// object. Only gets called at the display rate (60Hz) rather than the Box2D
// simulation rate (1000Hz) to save resources
void Body::updatePosition()
{
    int nodeCount = nodeList.size();
    // If the object is deformable, we need to update each node
    if(bodyType == BALL || bodyType == PLANE){
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        // Change the color of the central node if the object is selected
        if(isSelected) vertices[0].color = sf::Color::Green;
        else vertices[0].color = sf::Color::White;
        vertices[0].position = p0;
        for(int i(1);i<nodeCount;i++)
        {
            sf::Vector2f p = convertPosition(nodeList[i]->GetPosition());
            float norm = sqrt((p.x-p0.x)*(p.x-p0.x) + (p.y-p0.y)*(p.y-p0.y)); //account for node radius
            p = (p - p0)*(norm+nodeRadius)/norm + p0;
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

// Turn aBox2D vector into an SFML vector
sf::Vector2f Body::convertPosition(b2Vec2 v)
{
    return sf::Vector2f(v.x,v.y);
}

// Overload the == operation  (needed for the delete operation)
bool Body::operator==(const Body& rhs)
{
    return(getId() == rhs.getId());
}

// Remove the Body's b2Body components from the b2World
// Called when the body is deleted
void Body::destroyNodes()
{
        size_t numberOfNodes(nodeList.size());
        std::cout << numberOfNodes << std::endl;
        for(size_t i(0); i<numberOfNodes; i++)
        {
           nodeList[i]->GetWorld()->DestroyBody(nodeList[i]);
        }
}

// Return true if given position is within the Body's bounding box
bool Body::contains(sf::Vector2f position)
{
    return vertices.getBounds().contains(position);
}
