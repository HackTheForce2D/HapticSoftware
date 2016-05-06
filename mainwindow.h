#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectiondialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_actionConnect_triggered();

    //void on_display_resized();

signals:
    //void displayResized(QSize newSize);
    void findDevice();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
