#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

void Form::ShowImage(QString fname)
{
    ui->label_fname->setText(fname);
    QPixmap picture(fname);
    ui->label_picture->setPixmap(picture.scaled(ui->label_picture->width(),
                                                ui->label_picture->height(),
                                                Qt::KeepAspectRatio));
}

Form::~Form()
{
    delete ui;
}
