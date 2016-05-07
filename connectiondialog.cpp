#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include <iostream>

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

//void ConnectionDialog::on_buttonBox_clicked(QAbstractButton *button)
void ConnectionDialog::accept()
{
    //if (button-> == QDialogButtonBox::AcceptRole)
    //if(ui->buttonBox->standardButton(button) == QDialogButtonBox::AcceptRole)
    //if (ui->buttonBox->button(QDialogButtonBox::Help) == button)
    //if(QDialogButtonBox::buttonRole(button) == QDialogButtonBox::AcceptRole)
    if(QDialogButtonBox::AcceptRole == QDialogButtonBox::AcceptRole)
    {
        QLineEdit* addressBox =
                this->findChild<QLineEdit*>(QString("addressBox"));
        QSpinBox* portSpinBox =
                this->findChild<QSpinBox*>(QString("portSpinBox"));
        QString address = addressBox->text();
        int port = portSpinBox->value();
        emit connectToDevice(address,port);
        hide();
    }
}


