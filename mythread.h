#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include "socketserver.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(qintptr ID, SocketServer *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    SocketServer *w;
    QTcpSocket *socket;
    qintptr socketDescriptor;
};
#endif // MYTHREAD_H
