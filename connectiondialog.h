#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include<QSpinBox>
#include<QLineEdit>
#include<QAbstractButton>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();

private slots:
    //void on_buttonBox_clicked(QAbstractButton *button);
    void accept();

signals:
    void connectToDevice(QString address, int port);

private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
