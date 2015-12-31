#include "socketserver.h"
#include "mythread.h"

MyThread::MyThread(qintptr ID, SocketServer *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    w = parent;
}

void MyThread::run()
{
    QString strStatus;

    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

 //   w->AddConnection(socket->socketDescriptor(), socket->peerAddress(), socket->peerPort(), CNT_STATUS_CONNECTED);
    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    QTextStream(&strStatus) << socketDescriptor << " Client connected";
    w->SetStatus(strStatus);

//  strStatus = "";
    QTextStream(&strStatus) << QDateTime::currentDateTime().toString("[dd.MM.yy-hh:mm:ss]") << "-" << socketDescriptor << " Client connected";
    w->SetLog(strStatus);

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void MyThread::readyRead()
{
    QString strLog;

    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    QTextStream(&strLog) << QDateTime::currentDateTime().toString("[dd.MM.yy-hh:mm:ss]") << "-" << socketDescriptor << " Data in: " << Data;
    w->SetLog(strLog);
//  socket->write(Data);
}

void MyThread::disconnected()
{
    QString strLog;

    QTextStream(&strLog) << socketDescriptor << " Disconnected";
    w->SetLog(strLog);
    w->DelConnection(socketDescriptor);
    socket->deleteLater();
    exit(0);
}
