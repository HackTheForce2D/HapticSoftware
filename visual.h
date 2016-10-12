#ifndef VISUAL_H
#define VISUAL_H

#include<qsfmlcanvas.h>
#include<QVector2D>
#include<physics.h>
#include<QResizeEvent>

class Visual : public QSfmlCanvas
{
    Q_OBJECT
public:
    Visual(QWidget* Parent);
    static const QVector2D TOP_LEFT;
    static const QVector2D BOTTOM_RIGHT;
    void setPhysics(Physics *newPhysics);
    void startDisplay();

public slots:
    void startCreationMode();
    void newCircle();
    void newBox();
    void endCreationMode();
    void startCalibrationMode();
    void endCalibrationMode();
    void stopUpdating(bool stop);

signals:
    void bodyClicked(int index);
    void createNewCircle(b2Vec2 position, float radius);
    void createNewBox(b2Vec2 position, float rotation, b2Vec2 size);
    void calibrationPointEntered(int index);
    void readyToCalibrate();
    void deletePressed();

private:
    Physics *physics;
    sf::VertexArray polygon;
    sf::CircleShape newBall,calibrationTarget;
    sf::RectangleShape newWall;
    sf::Vector2f newWallPos, newWallAngle;
    float newWallLength,newWallThickness, graphicalThickness;
    sf::Vector2f position1,position2;
    sf::VertexArray workspaceBounds;
    sf::Transform physics2graphics;
    sf::Vector2f topLeft, bottomRight;
    bool creationMode;
    bool newObjectIsCircle;
    bool position1Set;
    bool stopUpdate;
    int calibrationPoint;
    float radius;
    void defineTransform(QSize windowSize);
    void nextCalibrationPoint();
    void getObjectLocations();
    void updateNewWall();
    QVector2D convertCoordinates();
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void wheelEvent(QWheelEvent *event);
    void OnInit();
    void OnUpdate();
};

#endif // VISUAL_H
