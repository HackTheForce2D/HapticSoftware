#include "physics.h"
#include<iostream>

const sf::Vector2f Physics::TOP_LEFT = sf::Vector2f(-17.7,10);
const sf::Vector2f Physics::BOTTOM_RIGHT = sf::Vector2f(17.7,-10);

Physics::Physics()
{
gravity = b2Vec2(0.0f, 0.0f);
world = new b2World(gravity);
velocityIterations = 6;
positionIterations = 2;
timeStep = 0.001f;
density = 1;
damping = .5;
stiffness = 10;
timer = new QTimer(this);

device2physics = device2physics.Identity;
//device2physics.rotate(180,sf::Vector2f(0,0));
//device2physics.translate(sf::Vector2f(0,-20));
//device2physics.scale(sf::Vector2f(-.2,.2),sf::Vector2f(0,0));
sf::Vector2f centerPhysics = sf::Vector2f((Physics::TOP_LEFT.x +
                                           Physics::BOTTOM_RIGHT.x)/2,
                                           (Physics::TOP_LEFT.y +
                                           Physics::BOTTOM_RIGHT.y)/2);
sf::Vector2f centerPantograph = sf::Vector2f((Pantograph::TOP_LEFT.x +
                                              Pantograph::BOTTOM_RIGHT.x)/2,
                                              (Pantograph::TOP_LEFT.y +
                                              Pantograph::BOTTOM_RIGHT.y)/2);
device2physics.translate(centerPhysics - centerPantograph);
float xScale = (Physics::TOP_LEFT.x - centerPhysics.x)/
        (Pantograph::TOP_LEFT.x - centerPantograph.x);
float yScale = (Physics::TOP_LEFT.y - centerPhysics.y)/
        (Pantograph::TOP_LEFT.y - centerPantograph.y);
std::cout << "Translation : " << (centerPhysics - centerPantograph).x << " " << (centerPhysics - centerPantograph).y << std::endl;
std::cout << "Scale : " << xScale << " " << yScale << std::endl;
std::cout << "Center : " << centerPhysics.x << " " << centerPhysics.y << std::endl;
device2physics.scale(sf::Vector2f(xScale,yScale), -(centerPhysics - centerPantograph));
sf::Vector2f testPoint= device2physics.transformPoint(Pantograph::TOP_LEFT);
std::cout << "Transformed top left : " << testPoint.x << " " << testPoint.y << std::endl;
testPoint= device2physics.transformPoint(Pantograph::BOTTOM_RIGHT);
std::cout << "Transformed bottom right : " << testPoint.x << " " << testPoint.y << std::endl;
}


void Physics::createSolidWall(b2Vec2 position, float rotation,
                              b2Vec2 size, bool isWorkspace=false)
{
    Body wall;
    wall.setType(WALL);
    b2BodyDef wallDef; //static body by default
    wallDef.position.Set(position.x,position.y);
    wallDef.angle = rotation;
    b2Body* wallBody = world->CreateBody(&wallDef);
    b2PolygonShape wallShape;
    wallShape.SetAsBox(size.x,size.y);
    b2FixtureDef wallFixtureDef;
    wallFixtureDef.shape = &wallShape;
    wallBody->CreateFixture(&wallFixtureDef);
    wall.addNode(wallBody);
    wall.finish();
    wall.setName("Wall");
    wall.setWallPosition(rotation*180/pi);
    if(isWorkspace)
    {
        workspaceWalls.append(wall);
    }else
    {
        bodyList.append(wall);
        emit objectListUpdated(bodyList);
    }
}

void Physics::createWorkspace(float left, float right, float bottom,
                              float top, float thickness)
{
    createSolidWall(b2Vec2(left-thickness/2,(top+bottom)/2),  //left wall
                    0.0f, b2Vec2(thickness/2,(top-bottom)/2+thickness),true);
    createSolidWall(b2Vec2(right+thickness/2,(top+bottom)/2), //right wall
                    0.0f, b2Vec2(thickness/2,(top-bottom)/2+thickness),true);
    createSolidWall(b2Vec2((left+right)/2,bottom-thickness/2),//bottom wall
                    0.0f, b2Vec2((right-left)/2+thickness,thickness/2),true);
    createSolidWall(b2Vec2((left+right)/2,top+thickness/2),   //top wall
                    0.0f, b2Vec2((right-left)/2+thickness,thickness/2),true);
}

void Physics::createBall(b2Vec2 position,float radius, float stiffness,
                         float damping, float density, float maxSpacing)
{
    Body ball;
    ball.setType(BALL);
    float nodeRadius = maxSpacing*.45;
    ball.setNodeRadius(nodeRadius);
    float mass(density*radius*radius);
    int nodeCount = (2*pi*radius/maxSpacing); //not including center
    float nodeDensity(mass/(nodeRadius*nodeRadius*(nodeCount+1)));
    b2BodyDef ballNodeDef;
    b2CircleShape ballNodeShape;
    ballNodeShape.m_radius = nodeRadius;
    b2FixtureDef ballFixtureDef;
    ballFixtureDef.shape = &ballNodeShape;
    ballFixtureDef.density = nodeDensity;
    ballNodeDef.type = b2_dynamicBody;
    b2DistanceJointDef nodeLinkDef;
    nodeLinkDef.frequencyHz = stiffness;
    nodeLinkDef.dampingRatio = damping;

    //center
    ballNodeDef.position.Set(position.x,position.y);
    b2Body* ballCenter = world->CreateBody(&ballNodeDef);
    ballCenter->CreateFixture(&ballFixtureDef);
    ball.addNode(ballCenter);
    //circle
    b2Vec2 nodePosition;
    //b2Body* firstNode =ballCenter,lastNode = ballCenter;
    for(int i(0);i<nodeCount;i++)
    {
        nodePosition = position + radius*b2Vec2(cos(2*pi*i/nodeCount),
                                                sin(2*pi*i/nodeCount));
        ballNodeDef.position.Set(nodePosition.x,nodePosition.y);
        b2Body* ballNode = world->CreateBody(&ballNodeDef);
        ballNode->CreateFixture(&ballFixtureDef);
        nodeLinkDef.Initialize(ballCenter,ballNode,
                               ballCenter->GetWorldCenter(),
                               ballNode->GetWorldCenter());
        world->CreateJoint(&nodeLinkDef);
        if(i>0)
        {
            nodeLinkDef.Initialize(ballNode,ball.getNode(i-1),
                                   ballNode->GetWorldCenter(),
                                   ball.getNode(i-1)->GetWorldCenter());
            world->CreateJoint(&nodeLinkDef);
        }
        ball.addNode(ballNode);
    }
    nodeLinkDef.Initialize(ball.getNode(nodeCount-1),ball.getNode(1),
                           ball.getNode(nodeCount-1)->GetWorldCenter(),
                           ball.getNode(1)->GetWorldCenter());
    world->CreateJoint(&nodeLinkDef);
    ball.finish();
    ball.setName("Ball");
    ball.setTransform(physics2graphics);
    bodyList.append(ball);
    emit objectListUpdated(bodyList);
}


void Physics::createEffector(float radius)
{
    b2BodyDef effectorDef;
    effectorDef.position.Set(0,0);
    effectorDef.type = b2_dynamicBody;
    b2Body* effectorPhysical = world->CreateBody(&effectorDef);
    b2CircleShape effectorShape;
    effectorShape.m_radius = radius;
    b2FixtureDef effectorFixtureDef;
    effectorFixtureDef.shape = &effectorShape;
    effectorFixtureDef.density = .01;
    effectorPhysical->CreateFixture(&effectorFixtureDef);
    effectorPhysical->SetLinearDamping(200);
    effector.setPhysical(effectorPhysical);
    effector.setRadius(radius);
    effector.setTransformHaptic(device2physics);
}

 void Physics::createEntities()
 {
      std::cout << "creating entities" << std::endl;
     //createWorkspace(-17.7*.95,17.7*.95,-10*.95,10*.95,1);
     createWorkspace(TOP_LEFT.x,BOTTOM_RIGHT.x,
                     BOTTOM_RIGHT.y,TOP_LEFT.y,.5);
     std::cout << "workspace created" << std::endl;
     createBall(b2Vec2(-5,5),2,15.f,0.5f,1,0.4);
     createBall(b2Vec2(0,5),2,35.f,0.5f,1,0.4);
     createBall(b2Vec2(5,5),2,50.f,0.8f,2,0.4);
     createSolidWall(b2Vec2(10,-2),1.2,b2Vec2(5,2),false);
     createSolidWall(b2Vec2(-10,-2),-1.2,b2Vec2(5,2),false);
     createEffector(1);
     //emit worldCreated();
 }

 int Physics::getBodyCount()
 {
     return bodyList.size();
 }

Body Physics::getBody(int index)
{
    return bodyList[index];
}

void Physics::selectBody(int index)
{
    for(int i(0);i<getBodyCount();i++)
    {
        if(index == i) bodyList[i].setSelected(true);
        else bodyList[i].setSelected(false);
    }
}

int Physics::getWorkspWallCount()
{
    return workspaceWalls.size();
}
Body Physics::getWorkspWall(int index)
{
    return workspaceWalls[index];
}

Effector Physics::getEffector()
{
    return effector;
}

void Physics::setDensity(float newDensity)
{
    density = newDensity;
}

void Physics::setStiffness(float newStiffness)
{
    stiffness = newStiffness;
}

void Physics::setDamping(float newDamping)
{
    damping = newDamping;
}

void Physics::createBall(b2Vec2 position, float radius)
{
    createBall(position,radius,stiffness,damping,density,0.4);
}

void Physics::setTransform(sf::Transform t)
{
    physics2graphics = t;
    for(int i(0);i<getBodyCount();i++)
    {
        bodyList[i].setTransform(physics2graphics);
    }
    for(int i(0);i<getWorkspWallCount();i++)
    {
        workspaceWalls[i].setTransform(physics2graphics);
    }
    effector.setTransform(physics2graphics);
}

void Physics::updateBodies()
{
    for(int i(0);i<getBodyCount();i++)
    {
        bodyList[i].updatePosition();
    }
    effector.updateGraphic();
}

void Physics::addBall() //TEMP
{
    createBall(b2Vec2(0,0),2,35.f,0.5f,1,0.4);
}

void Physics::deleteBody(int index)
{
    stopSim();
    if(bodyList.size() > index && index >= 0)
    {
        bodyList[index].destroyNodes();
        bodyList.removeAt(index);
    }
    emit objectListUpdated(bodyList);
    startSim();
}

void Physics::setHapticInterface(HapticInterface *i)
{
    hapticDevice = i;
}

void Physics::step()
{
    QVector2D position(hapticDevice->getPosition());
    emit forceUpdated(effector.updateForce(position));
    world->Step(timeStep,velocityIterations, positionIterations);
}

void Physics::run(){
connect(timer, SIGNAL(timeout()), this, SLOT(step()));
emit objectListUpdated(bodyList);
exec();
}

void Physics::startSim()
{
    timer->start((int)(timeStep*1000));
}
void Physics::stopSim()
{
    timer->stop();
}

void Physics::reset()
{
    size_t nbBodies = getBodyCount();
    //delete bodies from last to first
    for(size_t i(0); i <nbBodies;i++)
    {
        deleteBody(nbBodies-i-1);
    }
}
