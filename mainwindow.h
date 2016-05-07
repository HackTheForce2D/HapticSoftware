#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <connectiondialog.h>
#include <createobject.h>
#include <body.h>
#include <QStringListModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void bodyclicked(int bodyIndex);

private slots:

    void on_actionConnect_triggered();

    //void on_display_resized();

    void on_buttonAddObject_clicked();
    void updateListView(QList<Body> bodyList);

    void on_deleteAllButton_clicked();

    void on_objectList_clicked(const QModelIndex &index);

    void on_buttonDelObject_clicked();

signals:
    //void displayResized(QSize newSize);
    void findDevice();
    void createObject();
    void deleteAt(int index);
    void deleteAll();
    void selectedObject(int index);

private:
    Ui::MainWindow *ui;
    int indexSelected;
    QStringListModel *objectListSource;
};

#endif // MAINWINDOW_H
