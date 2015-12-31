#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class SocketServer;
}

class MyModel;          // forward declration

class ConnectStatus
{
public:
     ConnectStatus(int, QHostAddress, quint16, int);
     ~ConnectStatus();
     int sockDscrpt;
     QHostAddress ip;
     quint16 port;
     int Status;
};

class SocketServer : public QDialog
{
    Q_OBJECT

public:
    explicit SocketServer(QWidget *parent = 0);
    ~SocketServer();
    void SetStatus(QString);
    void SetLog(QString);
    void AddConnection(int, QHostAddress, quint16, int);
    void DelConnection(int);
    int GetConnCnt();
    QString GetCntString(int , int );

private slots:

    void on_pbStartService_clicked();

    void on_lePortNo_selectionChanged();
    
    void on_pbStopService_clicked();


    void on_pbExit_clicked();
    void incomingConnection();
    void readyUdpRead();
    void UDPErrHandle();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QVector<ConnectStatus *> cntVector;
    Ui::SocketServer *ui;
    QTcpServer *tcpSocketServer;
    QUdpSocket *udpSocket;
    bool bService;
    MyModel *myModel;

    void ServiceIsOn();
    void SetPBStatus();
    bool StartTCPService();
    bool StartUDPService();
};

enum tvConnectStatus {
   CNT_STATUS_LISTEN, CNT_STATUS_CONNECTED, CNT_STATUS_CLOSED
};

#endif // SOCKETSERVER_H
