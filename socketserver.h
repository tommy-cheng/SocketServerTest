#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class SocketServer;
}

class ConnectStatus {
public:
     ConnectStatus(QTcpSocket *, QHostAddress, quint16, int);
     ~ConnectStatus();
     QTcpSocket *socket;
     QHostAddress ip;
     quint16 port;
     int Status;
};

class SocketServer : public QDialog
{
    Q_OBJECT

public:
    explicit SocketServer(QWidget *parent = 0);
    void SetStatus(QString);
    void SetLog(QString);
    void AddConnection(QTcpSocket *, QHostAddress, quint16, int);
    void DelConnection(QTcpSocket *);
    ~SocketServer();

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

    void ServiceIsOn();
    void SetPBStatus();
    bool StartTCPService();
    bool StartUDPService();
    void SetTblData(int , int , QString );
    enum twTblColIndex {
        TBL_SOCK_DESC, TBL_IP, TBL_PORT, TBL_Status
    };
};

#endif // SOCKETSERVER_H
