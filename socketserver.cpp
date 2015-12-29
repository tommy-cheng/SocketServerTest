#include <QtNetwork>
#include <QString>
#include <QMessageBox>
#include "socketserver.h"
#include "mythread.h"
#include "ui_socketserver.h"

ConnectStatus::ConnectStatus(QTcpSocket *tcpSocket, QHostAddress ip, quint16 port, int status)
{
    this->socket = tcpSocket;
    this->ip = ip;
    this->port = port;
    this->Status = status;
}

SocketServer::SocketServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SocketServer)
{
    ui->setupUi(this);

    ui->twCntTbl->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:yellow }");
    ui->twCntTbl->setGridStyle(Qt::SolidLine);
    ui->twCntTbl->setShowGrid(true);
    bService = false;   
}

SocketServer::~SocketServer()
{
    delete ui;
}

void SocketServer::AddConnection(QTcpSocket *tcpSocket, QHostAddress ip, quint16 port, int status)
{
    ConnectStatus *cntNew;
    int rowCnt;

    cntNew = new ConnectStatus(tcpSocket, ip, port, status);

    cntVector.append(cntNew);

    rowCnt = ui->twCntTbl->rowCount();
    ui->twCntTbl->insertRow(rowCnt);
    SetTblData(rowCnt, TBL_SOCK_DESC, QString::number(tcpSocket->socketDescriptor()));
    SetTblData(rowCnt, TBL_IP, ip.toString());
    SetTblData(rowCnt, TBL_PORT, QString::number(port));
    SetTblData(rowCnt, TBL_Status, "Connected");
}

void SocketServer::DelConnection(QTcpSocket *tcpSocket)
{
    int err = 1, i;

    for (i=0; i<cntVector.size(); i++) {
        if (cntVector.at(i)->socket == tcpSocket) {
            err = 0;
            break;
        }
    }

    if (err == 0)
        cntVector.remove(i);
    else
        QMessageBox::information(this, tr("Socket Server"),
                                 tr("socket is not found"));
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

void SocketServer::SetTblData(int row, int col, QString data)
{
    QTableWidgetItem *qtItem;

    qtItem = new QTableWidgetItem(data);
    qtItem->setTextAlignment(Qt::AlignCenter);
    ui->twCntTbl->setItem(row, col, qtItem);
}

bool SocketServer::StartTCPService()
{
    int port;
    QString strStatus;
    bool bRet;

    tcpSocketServer = new QTcpServer(this);
    port = ui->lePortNo->text().toInt();
    ui->twCntTbl->setColumnCount(4);
    if(!tcpSocketServer->listen(QHostAddress::Any,port)) {

        QMessageBox::critical(this, "Socket Server", tcpSocketServer->errorString());
        bRet = false;
    }
    else {
        QTextStream(&strStatus) << "Listening to TCP port " << port << "...";
        ui->leStatus->setText(strStatus);
        connect(tcpSocketServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));

        ui->twCntTbl->insertRow(0);
        SetTblData(0, TBL_SOCK_DESC, QString::number(tcpSocketServer->socketDescriptor()));
        SetTblData(0, TBL_IP, "0");
        SetTblData(0, TBL_PORT, "0");
        SetTblData(0, TBL_Status, "Listen");
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
