#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QObject>
#include <winsock.h>
#include <Windows.h>
#include <fstream>

#define BufferLen 8192      // Buffer Size
#define ACK       "ACK"     // const char ACK message
#define ACKLen    4         // ACK Size


class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QObject *parent = 0);
    ~ServerThread();
    void InitServer();
    void run();

    // DATA MEMBERS:
    WSADATA         wsaData;
    SOCKET          socketServer;
    sockaddr_in     LocalEndPoint;
    sockaddr_in     RemoteEndPoint;
    std::fstream    logFile;

signals:
    void FileReady(QString);
};

#endif // SERVERTHREAD_H
