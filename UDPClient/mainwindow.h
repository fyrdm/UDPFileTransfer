#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <winsock.h>
//#include <WinSock2.h>
#include <fstream>
#include <Windows.h>
#include <cstdio>
#include <cstring>

#define BufferLen 8192
#define fnameLen  64
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_Select_Img_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
