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
    void updateProperties(float density, float stiffness, float damping);
    void toggleStatic(bool isStatic);
    void toggleRigid(bool isRigid);
    void newCircle();
    void newBox();

private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void on_buttonClose_clicked();

    void on_selectBall_toggled(bool checked);

    void on_checkStatic_toggled(bool checked);

    void on_checkRigid_toggled(bool checked);

private:
    Ui::CreateObject *ui;
    double density;
    double stiffness;
    double damping;
    bool   isRigid;
    bool   isStatic;
};

#endif // CREATEOBJECT_H
