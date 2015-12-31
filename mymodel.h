#ifndef MYMODEL_H
#define MYMODEL_H
#include <QAbstractTableModel>
#include "socketserver.h"

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MyModel(SocketServer *parent);
    ~MyModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


private:
    SocketServer *paServer;
};

enum twTblColIndex {
    TBL_SOCK_DESC, TBL_IP, TBL_PORT, TBL_STATUS
};

#endif // MYMODEL_H
