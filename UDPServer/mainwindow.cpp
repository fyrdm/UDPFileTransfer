// Author: Faruk YARDIM

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QTextBrowser>
#include <QPixmap>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    HANDLE  FileHandleForWriting;
    fstream logfile;
    logfile.open("Out.txt" , ios::app);
    logfile << "-------------SERVER---------------" << endl;

    WSADATA         wsaData;
    SOCKET          UDPSocketServer;
    sockaddr_in     LocalEndPoint, RemoteEndPoint;
    char            Buffer[8192];
    //int             BufferLen                       = sizeof(Buffer);
    int             RemoteEndPointLength            = sizeof(RemoteEndPoint);
    int             NumberOfBytesRead               = 0;
    int             NumberOfBytesWritten            = 0;

    //STEP-1 Initialize WinSock API
    if(WSAStartup(MAKEWORD(2,0),&wsaData) < 0)
        logfile << "1SERVER::ERROR::WSAStartup FAILED!" << endl;

    //STEP-2 Initialize LocalEndPoint (sockaddr_in) Structure
    LocalEndPoint.sin_family = AF_INET;
    LocalEndPoint.sin_addr.s_addr = inet_addr("127.0.0.1");
    LocalEndPoint.sin_port = htons(8001);

    //STEP-3 Create SOCKET
    UDPSocketServer = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(UDPSocketServer == INVALID_SOCKET)
        logfile << "2SERVER::ERROR::Socket Creation Failed! --> " << WSAGetLastError() << endl;

    //STEP-4 Bind Socket
    if(::bind(UDPSocketServer,
              (SOCKADDR*)&LocalEndPoint,
              sizeof(LocalEndPoint )) == SOCKET_ERROR)
        logfile << "3SERVER::ERROR::Bind FAILED!" << endl;
/* -----------------------------------------------------------------  */
    //STEP-5 RECEIVE from Client

    // Receive fname:

    memset(Buffer,0,BufferLen);
    if(recvfrom(UDPSocketServer,
                Buffer,
                BufferLen,
                NULL,
                (SOCKADDR*)&RemoteEndPoint,
                &RemoteEndPointLength) == SOCKET_ERROR )
        logfile << "4SERVER::ERROR::Receiving FNAME Failed!!" << WSAGetLastError() << endl;
    logfile << "5SERVER::MESSAGE FROM CLIENT: " << Buffer << "BL:" << BufferLen << endl;

    // Create a File Using fname:
    FileHandleForWriting = CreateFileA( Buffer,
                                        GENERIC_WRITE|GENERIC_READ,
                                        FILE_SHARE_WRITE,
                                        NULL,
                                        CREATE_NEW,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);
    // Set Path:
    QString FileName(Buffer);
    this->ui->textBrowser_Path->setText(FileName);

    // Send ACK:
    if( sendto(UDPSocketServer,
               ACK,
               ACKLen,
               NULL,
               (SOCKADDR*)&RemoteEndPoint,
               RemoteEndPointLength) == SOCKET_ERROR )
        logfile << "6SERVER::ERROR:: Sendto FAILED!" << WSAGetLastError() << endl;

    // RECEIVE LOOP:
    while(1)
    {
        logfile << "SERVER::1" << endl;
        // Receive Data
        memset(Buffer,0,BufferLen);
        NumberOfBytesRead = recvfrom(UDPSocketServer,
                                     Buffer,
                                     BufferLen,
                                     NULL,
                                     (SOCKADDR*)&RemoteEndPoint,
                                     &RemoteEndPointLength);
        logfile << "SERVER::2 NumberOfBytesRead -- BufferLen :"
                << NumberOfBytesRead << " -- " << BufferLen << endl;

        if(NumberOfBytesRead < 0 )
        {
            logfile << "7SERVER::ERROR::Receiving Data Failed!!" << WSAGetLastError() << endl;
            break;
        }
        else
            logfile << "8SERVER::MESSAGE FROM CLIENT: " << Buffer << endl;

        // Write to file
        //WriteFileEx(FileHandleForWriting,Buffer,NumberOfBytesRead,NULL,NULL);
        WriteFile(FileHandleForWriting, Buffer, NumberOfBytesRead, (DWORD*)&NumberOfBytesWritten, NULL);
        logfile << "SERVER::3  NumberOfBytesWritten: " << NumberOfBytesWritten << endl;
        // Send ACK:
        if( sendto(UDPSocketServer,
                   ACK,
                   ACKLen,
                   NULL,
                   (SOCKADDR*)&RemoteEndPoint,
                   RemoteEndPointLength) == SOCKET_ERROR )
            logfile << "9SERVER::ERROR:: Sendto FAILED!" << WSAGetLastError() << endl;

        logfile << "3.1SERVER::EOF Reached??:: NumberOfBytesRead -- BufferLen :"
                << NumberOfBytesRead << " -- " << BufferLen << endl;
        // Indicator to EOF reached...
        if(NumberOfBytesRead < BufferLen)
        {
            logfile << "SERVER::EOF Reached:: NumberOfBytesRead -- BufferLen :"
                    << NumberOfBytesRead << " -- " << BufferLen << endl;
            break;
        }

    }
    //STEP-6 CLOSE
    logfile << "10SERVER::Closing..." << endl;
    closesocket(UDPSocketServer);
    WSACleanup();
    CloseHandle(FileHandleForWriting);
    logfile.close();

    QPixmap picture(ui->textBrowser_Path->toPlainText());
    this->ui->label_picture->setPixmap(picture.scaled(ui->label_picture->width(),ui->label_picture->height(),Qt::KeepAspectRatio));

}
