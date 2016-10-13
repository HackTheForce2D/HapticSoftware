#ifndef BODY_H
#define BODY_H

#include<SFML/Graphics.hpp>
#include<Box2D/Box2D.h>
#include<QVector>
#include<QVector2D>
# define M_PI           3.14159265358979323846


const int WORKSPACE(0),BALL(1),PLANE(2),WALL(3),RIGIDBALL(4);

class Body : public sf::Drawable
{
public:
    Body();
    void createSolidCircle(b2World *world, b2Vec2 position, float radius,
                           float density, bool isStatic);
    void createSolidLine(b2World *world, b2Vec2 position, float rotation, b2Vec2 size,
                         float density, bool isStatic);
    void createElasticCircle(b2World *world, b2Vec2 position, float radius,
                             float stiffness,float damping, float density,
                             float maxSpacing, bool isStatic);
    void createElasticLine();
    static const int FORCE_FACTOR;
    void setType(int type);
    void setSelected(bool selected);
    void setNodeRadius(float newNodeRadius);
    void addNode(b2Body* node);
    bool contains(sf::Vector2f position);
    b2Body* getNode(int index);
    void finish();
    QString getName() const;
    void setName(QString newName);
    void setRadius(float radius_);
    void setWallPosition();
    void setTransform(sf::Transform transform);
    void updatePosition();
    long int getId() const;
    bool operator==(const Body& rhs);
    void destroyNodes();

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    int bodyType;
    long int id;
    QString name;
    sf::Color defaultColor;
    bool isSelected;
    float nodeRadius; //for ball
    float rotation; //for wall
    float radius;
    sf::CircleShape rigidBall; // for rigid circle
    sf::Vector2f convertPosition(b2Vec2 v);
    sf::VertexArray vertices;
    //sf::Texture m_texture;
    QVector<b2Body*> nodeList;
    sf::Transform physics2graphics;
    sf::Transform rotateWall; //for wall
};

#endif // BODY_H
