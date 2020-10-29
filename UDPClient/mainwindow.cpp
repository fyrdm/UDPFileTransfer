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
    fstream logfile;
    HANDLE  FileHandleForReading;
    logfile.open("Out.txt" , ios::app);
    logfile << "-------------CLIENT---------------" << endl;

    WSADATA         wsaData;
    SOCKET          UDPSocketClient;
    sockaddr_in     RemoteEndPoint;
    char            Buffer[8192];//            = "newFile.dat";//"Hello from Client!";
    char            fname[64]                  = "newFile.pdf";
    //int             BufferLen                = sizeof(Buffer);
    int             RemoteEndPointLength       = sizeof(RemoteEndPoint);
    int             NumberOfBytesRead          = 0;

    //STEP-1 Initialize WinSock API
    if(WSAStartup(MAKEWORD(2,0),&wsaData) < 0)
        logfile << "1CLIENT::ERROR::WSAStartup FAILED!" << endl;

    //STEP-2 Initialize UDPServer (sockaddr_in) Structure
    RemoteEndPoint.sin_family = AF_INET;
    RemoteEndPoint.sin_addr.s_addr = inet_addr("127.0.0.1");
    RemoteEndPoint.sin_port = htons(8001);

    //STEP-3 Create SOCKET
    UDPSocketClient = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(UDPSocketClient == INVALID_SOCKET)
        logfile << "2CLIENT::ERROR::Socket Creation Failed! --> " << WSAGetLastError() << endl;


    //STEP-4 Send to Server

    // Send FNAME:
    QFileInfo fi(ui->textBrowser_Path->toPlainText());
    memset(fname,0,fnameLen);
    memcpy_s(fname,fnameLen,
             fi.fileName().toStdString().c_str(),
             fi.fileName().length());

    if( sendto(UDPSocketClient,
               fname,
               strlen(fname),
               NULL,
               (SOCKADDR*)&RemoteEndPoint,
               RemoteEndPointLength) == SOCKET_ERROR )
        logfile << "3CLIENT::ERROR:: Sendto FNAME FAILED!" << WSAGetLastError() << endl;

    // Receive ACK:
    memset(Buffer,0,BufferLen);
    if(recvfrom(UDPSocketClient,
                Buffer,
                BufferLen,
                NULL,
                (SOCKADDR*)&RemoteEndPoint,
                &RemoteEndPointLength) == SOCKET_ERROR )
        logfile << "4CLIENT::ERROR::Receiving ACK Failed!!" << WSAGetLastError() << endl;
    logfile << "5CLIENT::MESSAGE FROM SERVER: " << Buffer << endl;

    // Open File to be sent:
    FileHandleForReading = CreateFileA(ui->textBrowser_Path->toPlainText().toStdString().c_str(),
                                        GENERIC_READ,
                                        FILE_SHARE_READ,
                                        NULL,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);

    // SEND LOOP
    while(1)
    {
        // Read Data
        memset(Buffer,0,BufferLen);
        ReadFile(FileHandleForReading, Buffer, BufferLen,(DWORD*)&NumberOfBytesRead, NULL);

        // Send Data
        if( sendto(UDPSocketClient,
                   Buffer,
                   NumberOfBytesRead,
                   NULL,
                   (SOCKADDR*)&RemoteEndPoint,
                   RemoteEndPointLength) < 0 )
            logfile << "6CLIENT::ERROR:: Send Data FAILED!" << WSAGetLastError() << endl;
        else
            logfile << "7CLIENT::MESSAGE SENT : " << Buffer << endl;

        // Receive ACK:
        memset(Buffer,0,BufferLen);
        if(recvfrom(UDPSocketClient,
                    Buffer,
                    BufferLen,
                    NULL,
                    (SOCKADDR*)&RemoteEndPoint,
                    &RemoteEndPointLength) == SOCKET_ERROR )
            logfile << "8CLIENT::ERROR::Receiving ACK Failed!!" << WSAGetLastError() << endl;
        logfile << "9CLIENT::MESSAGE FROM SERVER: " << Buffer << endl;

        // Indicator to EOF reached...
        if(NumberOfBytesRead < BufferLen)
        {
            logfile << "CLIENT::EOF Reached:: NumberOfBytesRead -- BufferLen :"
                    << NumberOfBytesRead << " -- " << BufferLen << endl;
            break;
        }
    }
    //STEP-5 CLOSE
    logfile << "10CLIENT::Closing..." << endl;
    closesocket(UDPSocketClient);
    WSACleanup();
    CloseHandle(FileHandleForReading);
    logfile.close();
}

void MainWindow::on_pushButton_Select_Img_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "GÃ¶rÃ¼ntÃ¼ SeÃ§iniz",QDir::homePath()+"/Desktop");
    this->ui->textBrowser_Path->setText(filename);
    QPixmap picture(filename);
    this->ui->label_picture->setPixmap(picture.scaled(ui->label_picture->width(),ui->label_picture->height(),Qt::KeepAspectRatio));


}
