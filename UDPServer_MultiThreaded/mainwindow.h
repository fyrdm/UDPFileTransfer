#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serverthread.h"
#include "form.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onFileReady(QString);
private slots:
    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::MainWindow *ui;
    ServerThread *serverthread;
};
#endif // MAINWINDOW_H
