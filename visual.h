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
    void endCreationMode();
    void startCalibrationMode();
    void endCalibrationMode();

signals:
    void bodyClicked(int index);
    void createNewBody(b2Vec2 position, float radius);
    void calibrationPointEntered(int index);
    void readyToCalibrate();

private:
    Physics *physics;
    sf::VertexArray polygon;
    sf::CircleShape newBall,calibrationTarget;
    sf::RectangleShape newWall;
    sf::VertexArray workspaceBounds;
    sf::Transform physics2graphics;
    sf::Vector2f topLeft, bottomRight;
    bool creationMode;
    int calibrationPoint;
    float radius;
    void defineTransform(QSize windowSize);
    void nextCalibrationPoint();
    void getObjectLocations();
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
