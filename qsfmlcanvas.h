#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QTimer>

//http://www.sfml-dev.org/tutorials/1.6/graphics-qt.php

class QSfmlCanvas : public QWidget, public sf::RenderWindow
{
public:
    //explicit QSfmlCanvas(QWidget *parent = 0);
    QSfmlCanvas(QWidget* Parent, unsigned int FrameTime = 0);

    virtual ~QSfmlCanvas();

//signals:

//public slots:
private:
    virtual void OnInit();

    virtual void OnUpdate();

    virtual QPaintEngine* paintEngine() const;

    virtual void showEvent(QShowEvent*);

    virtual void paintEvent(QPaintEvent*);

    QTimer myTimer;
    bool   myInitialized;
};

#endif // QSFMLCANVAS_H
