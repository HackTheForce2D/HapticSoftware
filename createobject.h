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

private:
    Ui::CreateObject *ui;
};

#endif // CREATEOBJECT_H
