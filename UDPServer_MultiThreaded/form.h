#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void ShowImage(QString);

private:
    Ui::Form *ui;
};

#endif // FORM_H
