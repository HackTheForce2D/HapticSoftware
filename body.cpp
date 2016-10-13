#include "body.h"
#include<iostream>

/* Class that represents a physical body in the Box2D simulation as well as
 * its graphical representation.
 * Needs to be refactored to include the creation of the object
 * (to lighten the Physics class and improve encapsulation) and also needs to
 * be split into a few subclasses for the various types of objects
*/

const int  Body::FORCE_FACTOR = 20;
Body::Body()
{
    id = rand()*rand();
    name = "object";
    isSelected = false;
    defaultColor = sf::Color(255,255,255);
    nodeList = QVector<b2Body*> (0);
}

void Body::createSolidCircle(b2World *world, b2Vec2 position,float radius,
                             float density,bool isStatic=true)
{
    b2BodyDef rigidBallDef; //static body by default
    defaultColor = sf::Color(255-15*density,255-15*density,150-15*density);
    rigidBallDef.position.Set(position.x,position.y);
    if (!isStatic)
    {
        rigidBallDef.type = b2_dynamicBody;
    }
    b2Body* rigidBallBody = world->CreateBody(&rigidBallDef);
    b2CircleShape rigidBallShape;
    rigidBallShape.m_radius = radius;
    b2FixtureDef rigidBallFixtureDef;
    rigidBallFixtureDef.density = density;
    rigidBallFixtureDef.shape = &rigidBallShape;
    //fulcrum only supports the bar and doesn't collide with anything
    //if(canCollide == false)
    //{
        //rigidBallFixtureDef.filter.categoryBits = 0;
    //}
    rigidBallBody->CreateFixture(&rigidBallFixtureDef);
    setRadius(radius);
    addNode(rigidBallBody);
    finish();
    setType(RIGIDBALL);
    setName("Rigid Circle");

}
void Body::createSolidLine(b2World *world, b2Vec2 position,float rotation,
                           b2Vec2 size,float density, bool isStatic= true)
{
    setType(WALL);
    defaultColor = sf::Color(150-15*density,255-15*density,150-15*density);
    std::cout << "Body: creating wall: position: " << position.x << ","
              << position.y <<" Size:" << size.x << "," << size.y << std::endl;
    b2BodyDef wallDef; //static body by default
    wallDef.position.Set(position.x,position.y);
    wallDef.angle = rotation;
    if (!isStatic)
    {
        wallDef.type = b2_dynamicBody;
    }
    b2Body* wallBody = world->CreateBody(&wallDef);
    b2PolygonShape wallShape;
    wallShape.SetAsBox(size.x,size.y);
    b2FixtureDef wallFixtureDef;
    wallFixtureDef.shape = &wallShape;
    wallFixtureDef.density = density;
    wallBody->CreateFixture(&wallFixtureDef);
    addNode(wallBody);
    finish();
    setName("Rigid Line");
    //setWallPosition(rotation*180/M_PI);
    setWallPosition();
}

void Body::createElasticCircle(b2World *world,b2Vec2 position,float radius, float stiffness,
                               float damping, float  density, float maxSpacing,bool isStatic = true)
{
    setType(BALL);
    defaultColor = sf::Color(255-15*density,255-15*density,255-15*density);
    float nodeRadius = maxSpacing*.45;
    setNodeRadius(nodeRadius);
    float mass(density*radius*radius);
    int nodeCount = (2*M_PI*radius/maxSpacing); //not including center
    float nodeDensity(mass/(nodeRadius*nodeRadius*(nodeCount+1)));
    b2BodyDef ballNodeDef;
    ballNodeDef.linearDamping = 1;
    ballNodeDef.angularDamping = 1;
    b2CircleShape ballNodeShape;
    ballNodeShape.m_radius = nodeRadius;
    b2FixtureDef ballFixtureDef;
    ballFixtureDef.shape = &ballNodeShape;
    ballFixtureDef.density = nodeDensity;
    b2DistanceJointDef nodeLinkDef;
    b2MotorJointDef nodeLinkMotorDef;
    // Smaller stiffness in the link from the center to the edge
    // So that the effector can deform the ball without penetrating it
    nodeLinkDef.frequencyHz = sqrt((stiffness)*FORCE_FACTOR/mass);
    nodeLinkDef.dampingRatio = damping;
    nodeLinkMotorDef.correctionFactor = stiffness;
    nodeLinkMotorDef.maxForce = stiffness*radius;
    nodeLinkMotorDef.maxTorque = 0;

    //center
    ballNodeDef.position.Set(position.x,position.y);
    if (!isStatic)
    {
        ballNodeDef.type = b2_dynamicBody;
    }
    b2Body* ballCenter = world->CreateBody(&ballNodeDef);
    ballCenter->CreateFixture(&ballFixtureDef);
    addNode(ballCenter);
    ballNodeDef.type = b2_dynamicBody;
    //circle
    b2Vec2 nodePosition;
    //b2Body* firstNode =ballCenter,lastNode = ballCenter;
    for(int i(0);i<nodeCount;i++)
    {
        ballNodeDef.type = b2_dynamicBody;
        nodePosition = position + radius*b2Vec2(cos(2*M_PI*i/nodeCount),
                                                sin(2*M_PI*i/nodeCount));
        ballNodeDef.position.Set(nodePosition.x,nodePosition.y);
        b2Body* ballNode = world->CreateBody(&ballNodeDef);
        ballNode->CreateFixture(&ballFixtureDef);
        nodeLinkDef.Initialize(ballCenter,ballNode,
                               ballCenter->GetWorldCenter(),
                               ballNode->GetWorldCenter());
        world->CreateJoint(&nodeLinkDef);
        nodeLinkMotorDef.Initialize(ballCenter,ballNode);
        world->CreateJoint(&nodeLinkMotorDef);
        if(i>0)
        {
            nodeLinkDef.Initialize(ballNode,getNode(i-1),
                                   ballNode->GetWorldCenter(),
                                   getNode(i-1)->GetWorldCenter());
            world->CreateJoint(&nodeLinkDef);
            nodeLinkMotorDef.Initialize(ballNode,getNode(i-1));
            world->CreateJoint(&nodeLinkMotorDef);
        }
        addNode(ballNode);
    }
    nodeLinkDef.Initialize(getNode(nodeCount-1),getNode(1),
                           getNode(nodeCount-1)->GetWorldCenter(),
                           getNode(1)->GetWorldCenter());
    world->CreateJoint(&nodeLinkDef);
    nodeLinkMotorDef.Initialize(getNode(nodeCount-1),getNode(1));
    world->CreateJoint(&nodeLinkMotorDef);
    finish();
    setName("Elastic Circle");
}
void Body::createElasticLine()
{
    setType(PLANE);

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

void Body::setRadius(float radius_)
{
    radius = radius_;
    rigidBall.setRadius(radius);
    rigidBall.setOrigin(radius,radius);
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
    if(bodyType == RIGIDBALL)
    {
        target.draw(rigidBall, states);
    }else
    {
        target.draw(vertices, states);
    }

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
        else vertices[0].color = defaultColor;
        vertices[0].position = p0;
        for(int i(1);i<nodeCount;i++)
        {
            sf::Vector2f p = convertPosition(nodeList[i]->GetPosition());
            float norm = sqrt((p.x-p0.x)*(p.x-p0.x) + (p.y-p0.y)*(p.y-p0.y)); //account for node radius
            p = (p - p0)*(norm+nodeRadius)/norm + p0;
            vertices[i].position = p;
            vertices[i].color = sf::Color::Blue;
        }
        // For the elastic ball, the last node is the same as the first node
        // (after the center) to close the circle
        if(bodyType == BALL)
        {
            vertices[nodeCount].position = vertices[1].position;
            vertices[nodeCount].color = vertices[1].color;
        }
    } else if(bodyType == WALL)
    {
        setWallPosition();
        for(int i(0);i<4;i++)
        {
            if(isSelected) vertices[i].color = sf::Color::Green;
            else vertices[i].color = defaultColor;
        }
    }else if(bodyType == RIGIDBALL)
    {
        rigidBall.setPosition(convertPosition(nodeList[0]->GetPosition()));
        if(isSelected) rigidBall.setFillColor(sf::Color::Green);
        else rigidBall.setFillColor(defaultColor);
    }
    //TODO : solid wall
}

void Body::setWallPosition()
{
    if(bodyType == WALL)
    {
        float rotation = nodeList[0]->GetAngle();
        rotation *= 180.f/M_PI;
        sf::Vector2f p0 = convertPosition(nodeList[0]->GetPosition());
        rotateWall = sf::Transform::Identity;
        rotateWall.rotate(rotation,p0);
        b2PolygonShape* wallShape = (b2PolygonShape*)nodeList[0]->GetFixtureList()[0].GetShape();
        for(int i(0);i<4;i++)
        {
            sf::Vector2f p = convertPosition( wallShape->GetVertex(i));
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
        std::cout << "Body: destroying "<< numberOfNodes<< " nodes..." << std::flush;
        for(size_t i(0); i<numberOfNodes; i++)
        {
           nodeList[i]->GetWorld()->DestroyBody(nodeList[i]);
        }
     std::cout << "ok" << std::endl;
}

// Return true if given position is within the Body's bounding box
bool Body::contains(sf::Vector2f position)
{
    if(bodyType == RIGIDBALL)
    {
        sf::Vector2f bodyPos = rigidBall.getPosition();
        return (pow(bodyPos.x - position.x,2)+pow(bodyPos.y - position.y,2)
                < pow(rigidBall.getRadius(),2));
    }
    return vertices.getBounds().contains(position);

}
