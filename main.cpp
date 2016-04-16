#include "mainwindow.h"
#include <QApplication>
#include <physics.h>
#include <visual.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Visual * display =  w.findChild<Visual*>(QString("display"));
    //QPushButton *start =

    return a.exec();
}
