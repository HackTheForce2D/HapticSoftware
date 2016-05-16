#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

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

void MainWindow::on_buttonAddObject_clicked()
{
    emit createObject();
}

void MainWindow::updateListView(QList<Body> bodyList)
{
    std::cout << "update list view " << std::endl;
    objectListSource = new QStringListModel;
    //objectList->setModel(objectListSource );
    objectListSource->insertColumns(0,1);
    objectListSource->insertRows(0,bodyList.size());
    for(int i(0);i<bodyList.size();i++)
    {
        objectListSource->setData(objectListSource->index(i),bodyList[i].getName());
    }
    QListView * objectList = findChild<QListView*>(QString("objectList"));
    objectList->setModel(objectListSource);
}

void MainWindow::on_deleteAllButton_clicked()
{
    emit deleteAll();
}

void MainWindow::on_objectList_clicked(const QModelIndex &index)
{
    indexSelected = index.row();
    std::cout << "Index Selected : " << indexSelected << std::endl;
    emit selectedObject(indexSelected);
}

void MainWindow::on_buttonDelObject_clicked()
{
    std::cout << "Index Selected : " << indexSelected << std::endl;
    emit deleteAt(indexSelected);
}

void MainWindow::bodyclicked(int bodyIndex)
{
    indexSelected = bodyIndex;
    QListView * objectList = findChild<QListView*>(QString("objectList"));
    if(indexSelected == -1) //used clicked on empty space
    {
        objectList->clearSelection();
    }
    else
    {
       QModelIndex qIndex = objectListSource->index(indexSelected);
       objectList->setCurrentIndex(qIndex);
    }
}

void MainWindow::on_actionCalibrate_triggered()
{
    emit calibrationStarted();
}
