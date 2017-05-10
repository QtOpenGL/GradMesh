#include "dialog.h"
#include "ui_dialog.h"
#include <QColorDialog>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QColorDialog *colorDialog = new QColorDialog(Qt::white, parent);

    colorDialog->setWindowFlags(Qt::Widget);
    colorDialog->exec();

}

Dialog::~Dialog()
{
    delete ui;
}
