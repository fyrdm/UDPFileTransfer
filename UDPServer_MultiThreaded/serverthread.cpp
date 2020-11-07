#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent)
{
    InitServer();
}

ServerThread::~ServerThread()
{
    closesocket(this->socketServer);
    WSACleanup();
    logFile.close();
}

void ServerThread::InitServer()
{
    // For Debugging Purposes
    logFile.open("Out.txt", std::ios::app);

    // STEP-1 Initialize WinSock API
    if(WSAStartup(MAKEWORD(2,0),&wsaData) < 0)
        logFile << "SERVER::ERROR::WSAStartup FAILED!" << std::endl;

    // STEP-2 Set LocalEndPoint Parameters
    LocalEndPoint.sin_family        = AF_INET;                 // IPv4
    LocalEndPoint.sin_addr.s_addr   = inet_addr("127.0.0.1");  // Local Address
    LocalEndPoint.sin_port          = htons(8001);             // Arbitrary Port Number

    // STEP-3 Create socket descriptor
    socketServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // UDP Socket Creation
    if(socketServer == INVALID_SOCKET)
        logFile << "SERVER::ERROR::Socket Creation Failed! : \n" << WSAGetLastError() << std::endl;

    // STEP-4 Bind Socket
    if( bind(socketServer,
             (SOCKADDR*)&LocalEndPoint,
             sizeof(LocalEndPoint)) == SOCKET_ERROR)
        logFile << "SERVER::ERROR::Bind FAILED! :\n" << WSAGetLastError() << std::endl;

}

void ServerThread::run()
{
    char Buffer[BufferLen];                                 // Buffer for interaction...
    int  RemoteEndPointLength  = sizeof(RemoteEndPoint);    // Neccessary for sendto , recvfrom...
    int  NumberOfBytesRead;                                 //
    int  NumberOfBytesWritten;                              //
    HANDLE FileHandleForWriting;                        // File Handle to create incoming file


    // Outer Loop: Loops for new transfers...
    while(true)
    {
        FileHandleForWriting    = NULL;                      // File Handle to create incoming file
        NumberOfBytesRead       = 0;
        NumberOfBytesWritten    = 0;
        // Sit and Wait For A File Name:
        memset(Buffer,0,BufferLen);
        if( recvfrom(socketServer,
                     Buffer,
                     BufferLen,
                     NULL,
                     (SOCKADDR*)&RemoteEndPoint,
                     &RemoteEndPointLength) == SOCKET_ERROR )
            logFile << "SERVER::ERROR::Receiving FILE NAME FAILED! :\n" << WSAGetLastError() << std::endl;
        logFile << "SERVER::MESSAGE FROM CLIENT: " << Buffer << "\n"
                << "Buffer Size : " << BufferLen << std::endl;

        // Create a File Using <Received FNAME> :
        FileHandleForWriting = CreateFileA(Buffer,
                                           GENERIC_WRITE | GENERIC_READ,
                                           FILE_SHARE_WRITE,
                                           NULL,
                                           CREATE_NEW,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL);
        // Hold FNAME:
        QString FileName(Buffer);

        // SEND ACK:
        if( sendto(socketServer,
                   ACK,
                   ACKLen,
                   NULL,
                   (SOCKADDR*)&RemoteEndPoint,
                   RemoteEndPointLength) == SOCKET_ERROR )
            logFile << "SERVER::ERROR::Sendto -- FNAME ACK -- FAILED! :\n" << WSAGetLastError() << std::endl;


        // Inner Loop: Loops for receiving data packets...
        while(true)
        {
            // Receive a packet:
            logFile << "SERVER::INNER RECEIVE LOOP" << std::endl;
            memset(Buffer,0,BufferLen);
            NumberOfBytesRead = recvfrom(socketServer,
                                         Buffer,
                                         BufferLen,
                                         NULL,
                                         (SOCKADDR*)&RemoteEndPoint,
                                         &RemoteEndPointLength);
            logFile << "SERVER:: NUMBER OF BYTES READ : " << NumberOfBytesRead << std::endl;

            if(NumberOfBytesRead < 0)
            {
                logFile << "SERVER::ERROR::Receiving Data Failed!" << std::endl;
                break;
            }

            // Write into file:
            WriteFile(FileHandleForWriting,
                      Buffer,
                      NumberOfBytesRead,
                      (DWORD*)&NumberOfBytesWritten,
                      NULL);
            logFile << "SERVER::NumberOfBytesWritten: " << NumberOfBytesWritten << std::endl;

            // Send ACK:
            if( sendto(socketServer,
                       ACK,
                       ACKLen,
                       NULL,
                       (SOCKADDR*)&RemoteEndPoint,
                       RemoteEndPointLength) == SOCKET_ERROR)
                logFile << "SERVER::ERROR:: Sendto --- DATA ACK --- Failed! : \n" << WSAGetLastError() << std::endl;

            // Indicator to EOF Reached:
            if(NumberOfBytesRead < BufferLen)
            {
                logFile << "SERVER::EOF Reached:: NumberOfBytesRead < BufferLen : "
                        << NumberOfBytesRead << " < " << BufferLen << std::endl;
                break;
            }
        } // End Inner Loop

        // Emit Signal:
        if(!FileName.isNull())
        {
            CloseHandle(FileHandleForWriting);
            emit FileReady(FileName);
        }
        else
            break;
    } // End Outer Loop
}
