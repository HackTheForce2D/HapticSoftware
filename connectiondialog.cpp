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

// When user clicks OK, send a signal with the IP address and port
// Signal is received by the HapticInterface class which connects to the device
void ConnectionDialog::accept()
{
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


