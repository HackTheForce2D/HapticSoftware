#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_actionConnect_triggered()
{
    emit findDevice();
}
/*
void MainWindow::on_display_resized()
{
    emit displayResized(this->findChild<Visual *>("display")->size());
}*/
