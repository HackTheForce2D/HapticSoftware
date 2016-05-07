#include "createobject.h"
#include "ui_createobject.h"

CreateObject::CreateObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateObject)
{
    ui->setupUi(this);
}

CreateObject::~CreateObject()
{
    delete ui;
}
