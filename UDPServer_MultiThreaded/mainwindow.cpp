#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->removeTab(1);

    serverthread = new ServerThread(this);
    connect(serverthread,SIGNAL(FileReady(QString)),
            this,        SLOT(onFileReady(QString)));
    serverthread->start();
}

void MainWindow::onFileReady(QString fname)
{
    Form *newForm = new Form(this->ui->tabWidget);
    newForm->ShowImage(fname);
    ui->tabWidget->addTab(newForm,QString(fname));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    //ui->tabWidget->setCurrentWidget(ui->tabWidget->widget(ui->tabWidget->count()-1));
}

MainWindow::~MainWindow()
{
    serverthread->terminate();
    delete ui;
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(index != 0)
    {
        QWidget* tab = ui->tabWidget->widget(index);
        ui->tabWidget->removeTab(index);
        delete tab;
    }
}
