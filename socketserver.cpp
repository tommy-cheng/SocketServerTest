#include <QtNetwork>
#include <QString>
#include <QMessageBox>
#include <QStandardItemModel>
#include "mymodel.h"
#include "socketserver.h"
#include "mythread.h"
#include "ui_socketserver.h"

ConnectStatus::ConnectStatus(int sockDesc, QHostAddress ip, quint16 port, int status)
{
    this->sockDscrpt = sockDesc;
    this->ip = ip;
    this->port = port;
    this->Status = status;
}

ConnectStatus::~ConnectStatus()
{
}

SocketServer::SocketServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SocketServer)
{
//  QStandardItemModel *model = new QStandardItemModel(0, 4, this);
    ui->setupUi(this);

    myModel = new MyModel(this);
    ui->tvCntTbl->setModel(myModel);
    ui->tvCntTbl->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:yellow }");
    ui->tvCntTbl->setGridStyle(Qt::SolidLine);
    ui->tvCntTbl->setShowGrid(true);
    ui->tvCntTbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvCntTbl->setSelectionMode(QAbstractItemView::SingleSelection);
    bService = false;   
}

SocketServer::~SocketServer()
{
    delete ui;
}

int SocketServer::GetConnCnt ()
{
    int iRet;

    iRet = cntVector.count();
    return iRet;
}

void SocketServer::AddConnection(int sockDscrpt, QHostAddress ip, quint16 port, int status)
{
    ConnectStatus *cntNew;
    int rowCount;

    cntNew = new ConnectStatus(sockDscrpt, ip, port, status);

    cntVector.append(cntNew);
    rowCount = myModel->rowCount();
    myModel->insertRow(rowCount);

    emit myModel->layoutChanged();
}

void SocketServer::DelConnection(int sockDesc)
{
    int err = 1, i;

    for (i=0; i<cntVector.size(); i++) {
        if (cntVector.at(i)->sockDscrpt == sockDesc) {
            err = 0;
            break;
        }
    }

    if (err == 0) {
        cntVector.remove(i);
        myModel->removeRow(i);
        emit myModel->layoutChanged();
    }
    else
        QMessageBox::information(this, tr("Socket Server"),
                                 tr("socket is not found"));
}

QString SocketServer::GetCntString(int row, int col)
{
    QString strRet(QString::null);
    int size;

    size = cntVector.size();
    if (row >= size) {
        qDebug() << QString("GetCntString idx is too big[vector:%1, idx:%2]")
                    .arg(size).arg(row);
    }
    else {
        switch (col) {
        case TBL_SOCK_DESC:
            strRet = QString::number(cntVector.at(row)->sockDscrpt);
            break;

        case TBL_IP:
            strRet = cntVector.at(row)->ip.toString();
            break;

        case TBL_PORT:
            strRet = QString::number(cntVector.at(row)->port);
            break;

        case TBL_STATUS:
            switch (cntVector.at(row)->Status) {
            case CNT_STATUS_LISTEN:
                strRet = "Listen";
                break;

            case CNT_STATUS_CONNECTED:
                strRet = "Connected";
                break;

            default:
                strRet = "Unknown";
                break;
            }
        }
    }

    return strRet;
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

    AddConnection(newTCPConn->socketDescriptor(), newTCPConn->peerAddress(), newTCPConn->peerPort(), CNT_STATUS_CONNECTED);
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
        AddConnection(tcpSocketServer->socketDescriptor(), QHostAddress::Any, port, CNT_STATUS_LISTEN);
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
    if (ui->rbTCP->isChecked()) {
        DelConnection(tcpSocketServer->socketDescriptor());
        tcpSocketServer->close();
    }
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

void SocketServer::on_tvCntTbl_doubleClicked(const QModelIndex &index)
{
    QString SockDesc, IP, Port;
    QModelIndexList selection = ui->tvCntTbl->selectionModel()->selectedRows();

     Q_UNUSED(index);
    // Multiple rows can be selected
    for(int i=0; i< selection.count(); i++) {
        QModelIndex index = selection.at(i);
        qDebug() << index.row();
        commDlg = new commDialog(this);
        SockDesc = GetCntString(index.row(),TBL_SOCK_DESC);
        IP = GetCntString(index.row(),TBL_IP);
        Port = GetCntString(index.row(), TBL_PORT);
        commDlg->setCommData(SockDesc, IP, Port);
        commDlg->exec();
    }
}
