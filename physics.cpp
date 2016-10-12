#include "physics.h"
#include<iostream>

const sf::Vector2f Physics::TOP_LEFT = sf::Vector2f(-17.7,10);
const sf::Vector2f Physics::BOTTOM_RIGHT = sf::Vector2f(17.7,-10);

Physics::Physics()
{
    // Parameters for Box2D
    gravity = b2Vec2(0.0f, -10.0f);
    world = new b2World(gravity);
    velocityIterations = 6;
    positionIterations = 2;
    timeStep = 0.001f;
    density = 1;
    damping = .5;
    stiffness = 10;
    timer = new QTimer(this);
    updateRequested = false;

    // Set the coordinate transform to convert real workspace coordinates
    // into physical simulation coordinates
    device2physics = device2physics.Identity;
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
    device2physics.scale(sf::Vector2f(xScale,yScale), -(centerPhysics - centerPantograph));
    // Uncomment this for debugging if the transform is not correct
    //sf::Vector2f testPoint= device2physics.transformPoint(Pantograph::TOP_LEFT);
    //testPoint= device2physics.transformPoint(Pantograph::BOTTOM_RIGHT);
}


void Physics::createSolidWall(b2Vec2 position, float rotation,
                              b2Vec2 size, bool isWorkspace=false,
                              bool isStatic = true)
{
    emit stopDisplay(true);
    Body wall;
    wall.createSolidLine(world,position,rotation,size,density,isStatic);
    if(isWorkspace)
    {
        workspaceWalls.append(wall);
    }else
    {
        wall.setTransform(physics2graphics);
        bodyList.append(wall);
        emit objectListUpdated(bodyList);
    }
    emit stopDisplay(false);
}

void Physics::createWorkspace(float left, float right, float bottom,
                              float top, float thickness)
{
    createSolidWall(b2Vec2(left-thickness/2,(top+bottom)/2),  //left wall
                    0.0f, b2Vec2(thickness/2,(top-bottom)/2+thickness),true,true);
    createSolidWall(b2Vec2(right+thickness/2,(top+bottom)/2), //right wall
                    0.0f, b2Vec2(thickness/2,(top-bottom)/2+thickness),true,true);
    createSolidWall(b2Vec2((left+right)/2,bottom-thickness/2),//bottom wall
                    0.0f, b2Vec2((right-left)/2+thickness,thickness/2),true,true);
    createSolidWall(b2Vec2((left+right)/2,top+thickness/2),   //top wall
                    0.0f, b2Vec2((right-left)/2+thickness,thickness/2),true,true);
}

void Physics::createBall(b2Vec2 position,float radius, float stiffness,
                         float damping, float density, float maxSpacing, bool isStatic = true)
{
    emit stopDisplay(true);
    Body ball;
    ball.createElasticCircle(world, position,radius, stiffness,damping,
                             density,maxSpacing,isStatic);
    ball.setTransform(physics2graphics);
    bodyList.append(ball);
    emit objectListUpdated(bodyList);
    emit stopDisplay(false);
}

void Physics::createRigidBall(b2Vec2 position,float radius,bool isStatic=true)
{
    emit stopDisplay(true);
    Body rigidBall;
    rigidBall.createSolidCircle(world,position,radius,density,isStatic);
    rigidBall.setTransform(physics2graphics);
    bodyList.append(rigidBall);
    emit objectListUpdated(bodyList);
    emit stopDisplay(false);
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
     createWorkspace(TOP_LEFT.x,BOTTOM_RIGHT.x,
                     BOTTOM_RIGHT.y,TOP_LEFT.y,.5);
     createBall(b2Vec2(-5,5),2,15.f,0.5f,1,0.2,false);
     createBall(b2Vec2(0,-5),2,35.f,0.5f,1,0.2,false);
     createBall(b2Vec2(5,5),2,50.f,0.8f,2,0.2,false);
     createSolidWall(b2Vec2(10,-2),1.2,b2Vec2(5,2),false,false);
     createSolidWall(b2Vec2(-10,-2),-1.2,b2Vec2(5,2),false,true);
     //createRigidBall(b2Vec2(6,5),2,false);
     createSolidWall(b2Vec2(0.5,-1),0,b2Vec2(1,1),false,false);
     createEffector(1);
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

void Physics::setRigid(bool isRigid)
{
    newObjectIsRigid = isRigid;
}

void Physics::setStatic(bool isStatic)
{
    newObjectIsStatic = isStatic;
}

void Physics::createNewCircle(b2Vec2 position, float radius)
{
    if(newObjectIsRigid)
    {
        createRigidBall(position,radius,newObjectIsStatic);
    }else
    {
        createBall(position,radius,stiffness,damping,
                   density,0.2,newObjectIsStatic);
    }
}

void Physics::createNewBox(b2Vec2 position, float rotation,
                           b2Vec2 size)
{
    if(true)//if(newObjectIsRigid) // Finish the other one
    {
        //createSolidWall(position,rotation,size,false,newObjectIsStatic);
        createSolidWall(position,rotation,size,false,newObjectIsStatic);
    }
}


void Physics::setObjectProperties(float newDensity, float newStiffness, float newDamping)
{
    density = newDensity;
    stiffness = newStiffness;
    damping = newDamping;
    std::cout << density << " " << stiffness << " " << damping << " " << std::endl;
}

void Physics::createBall(b2Vec2 position, float radius, bool isStatic = true)
{
    createBall(position,radius,stiffness,damping,density,0.2,isStatic);
}

// Rigid transformation plus scaling to convert the simulation coordinates
// in meters to the display window coordinates in pixels
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

void Physics::deleteBody(int index)
{
    stopSim();
    emit stopDisplay(true);
    if(bodyList.size() > index && index >= 0)
    {
        std::cout << "Physics: destroying body " << index << " :"
                  << std::endl;
        bodyList[index].destroyNodes();
        bodyList.removeAt(index);
    }
    emit objectListUpdated(bodyList);
    emit stopDisplay(false);
    startSim();
}

void Physics::setHapticInterface(HapticInterface *i)
{
    hapticDevice = i;
}

void Physics::step()
{
    // Get the updated position from the TCP interface
    QVector2D position(hapticDevice->getPosition());
    // Update the force on the effector
    emit forceUpdated(effector.updateForce(position));
    // Step the simulation
    world->Step(timeStep,velocityIterations, positionIterations);
    if(updateRequested)
    {
      updateBodies();
      updateRequested = false;
    }
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

void Physics::requestUpdate()
{
    updateRequested = true;
}

void Physics::reset()
{
    size_t nbBodies = getBodyCount();
    //delete bodies from last to first, so their indices don't change
    for(size_t i(0); i <nbBodies;i++)
    {
        deleteBody(nbBodies-i-1);
    }
}
