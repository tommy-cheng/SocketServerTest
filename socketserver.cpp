#include <QtNetwork>
#include <QString>
#include <QMessageBox>
#include "socketserver.h"
#include "mythread.h"
#include "ui_socketserver.h"

SocketServer::SocketServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SocketServer)
{
    ui->setupUi(this);
    bService = false;   
}

SocketServer::~SocketServer()
{
    delete ui;
}

void SocketServer::SetStatus(QString strStatus)
{
    ui->leStatus->setText(strStatus);
}

void SocketServer::SetLog(QString strLog)
{
    ui->teLog->appendPlainText(strLog);
    ui->teLog->viewport()->update();
}

void SocketServer::SetPBStatus()
{
    if (ui->lePortNo->text().isEmpty()) {
        if (!bService) { // text -> 0, serv -> 0
            ui->pbStartService->setEnabled(false);
            ui->pbStopService->setEnabled(false);
        }
        else {          // text -> 0, serv -> 1
            ui->pbStartService->setEnabled(false);
            ui->pbStopService->setEnabled(true);
        }
    }
    else {
        if (!bService) {  // text -> 1, serv -> 0
            ui->pbStartService->setEnabled(true);
            ui->pbStopService->setEnabled(false);
        }
        else {            // text -> 1, serv -> 1
            ui->pbStartService->setEnabled(false);
            ui->pbStopService->setEnabled(true);
        }
    }
}

void SocketServer::incomingConnection()
{
    QString strStatus;
    QTcpSocket *newTCPConn;
    qintptr socketDescriptor;

    newTCPConn = tcpSocketServer->nextPendingConnection();
    socketDescriptor = newTCPConn->socketDescriptor();
    // We have a new connection
    QTextStream(&strStatus) << socketDescriptor << " Connecting...";
    ui->leStatus->setText(strStatus);

    MyThread *thread = new MyThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

bool SocketServer::StartTCPService()
{
    int port;
    QString strStatus;
    bool bRet;

    tcpSocketServer = new QTcpServer(this);
    port = ui->lePortNo->text().toInt();
    if(!tcpSocketServer->listen(QHostAddress::Any,port)) {

        QMessageBox::critical(this, "Socket Server", tcpSocketServer->errorString());
        bRet = false;
    }
    else {
        QTextStream(&strStatus) << "Listening to TCP port " << port << "...";
        ui->leStatus->setText(strStatus);
        connect(tcpSocketServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
        bRet = true;
    }
    return bRet;
}

void SocketServer::readyUdpRead()
{
    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    QString strLog;

    udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    // will write on server side window
    QTextStream(&strLog) << QDateTime::currentDateTime().toString("[dd.MM.yy-hh:mm:ss]") << "-" << udpSocket->socketDescriptor() << " Data in: " << datagram << sender.toString() << senderPort;
    SetLog(strLog);
}

void SocketServer::UDPErrHandle()
{
    ui->leStatus->setText(udpSocket->errorString());
}

bool SocketServer::StartUDPService()
{
    int port;
    QString strStatus;
    bool bRet;

    udpSocket = new QUdpSocket(this);
    port = ui->lePortNo->text().toInt();
    if (!udpSocket->bind(QHostAddress::Any, port)) {
        QMessageBox::critical(this, "Socket Server", udpSocket->errorString());
        bRet = false;
    }
    else {
        QTextStream(&strStatus) << "Listening to UDP port " << port << "...";
        ui->leStatus->setText(strStatus);
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readyUdpRead()));
        connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(UDPErrHandle()));
        bRet = true;
    }
    return bRet;
}

void SocketServer::on_pbStartService_clicked()
{
    bool bRet;

    if (ui->rbTCP->isChecked())
        bRet = StartTCPService();
    else
        bRet = StartUDPService();

    if (bRet) {
        bService = true;
        ui->rbTCP->setEnabled(false);
        ui->rbUDP->setEnabled(false);
        ui->pbStartService->setEnabled(false);
        ui->pbStopService->setEnabled(true);
    }
}

void SocketServer::on_lePortNo_selectionChanged()
{
    SetPBStatus();    
}

void SocketServer::on_pbStopService_clicked()
{
    if (ui->rbTCP->isChecked())
        tcpSocketServer->close();
    else
        udpSocket->close();
    bService = false;
    ui->rbTCP->setEnabled(true);
    ui->rbUDP->setEnabled(true);
    SetPBStatus();
    ui->leStatus->setText("");
}

void SocketServer::on_pbExit_clicked()
{
    close();
}
