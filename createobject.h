#ifndef CREATEOBJECT_H
#define CREATEOBJECT_H

#include <QDialog>

namespace Ui {
class CreateObject;
}

class CreateObject : public QDialog
{
    Q_OBJECT

public:
    explicit CreateObject(QWidget *parent = 0);
    ~CreateObject();

signals:
    void endCreationMode();
    void densityChanged(float density);
    void stiffnessChanged(float stiffness);
    void dampingChanged(float damping);

private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void on_buttonClose_clicked();

private:
    Ui::CreateObject *ui;
};

#endif // CREATEOBJECT_H
