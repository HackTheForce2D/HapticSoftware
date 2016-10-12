#include "createobject.h"
#include "ui_createobject.h"

CreateObject::CreateObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateObject)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    density = 1.0;
    stiffness = 10.0;
    damping = 1.0;
}

CreateObject::~CreateObject()
{
    delete ui;
}

// When the user changes the properties of the object to be created,
// send a signal with the new property value, to be received by Physics class
void CreateObject::on_doubleSpinBox_valueChanged(double arg1)
{
    //emit densityChanged((float)arg1);
    density = arg1;
    emit updateProperties( density, stiffness, damping);
}

void CreateObject::on_doubleSpinBox_2_valueChanged(double arg1)
{
    //emit stiffnessChanged((float)arg1);
    stiffness = arg1;
    emit updateProperties( density,  stiffness, damping);
}

void CreateObject::on_doubleSpinBox_3_valueChanged(double arg1)
{
    //emit dampingChanged((float)arg1);
    damping = arg1;
    emit updateProperties( density,  stiffness, damping);
}

// Stop the creation mode when the user closes the dialog
// Signal received by Visual class, which stops drawing the new object preview
void CreateObject::on_buttonClose_clicked()
{
    emit endCreationMode();
    hide();
}

void CreateObject::on_selectBall_toggled(bool checked)
{
    if(checked)  emit newCircle();
    else emit newBox();
}

void CreateObject::on_checkStatic_toggled(bool checked)
{
    emit toggleStatic(checked);
}

void CreateObject::on_checkRigid_toggled(bool checked)
{
    emit toggleRigid(checked);
}
