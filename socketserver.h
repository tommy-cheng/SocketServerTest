#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class SocketServer;
}

class SocketServer : public QDialog
{
    Q_OBJECT

public:
    explicit SocketServer(QWidget *parent = 0);
    void SetStatus(QString);
    void SetLog(QString);
    ~SocketServer();

private slots:

    void on_pbStartService_clicked();

    void on_lePortNo_selectionChanged();
    
    void on_pbStopService_clicked();


    void on_pbExit_clicked();
    void incomingConnection();
    void readyUdpRead();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    Ui::SocketServer *ui;
    QTcpServer *tcpSocketServer;
    QUdpSocket *udpSocket;
    bool bService;

    void ServiceIsOn();
    void SetPBStatus();
    bool StartTCPService();
    bool StartUDPService();
};

#endif // SOCKETSERVER_H
