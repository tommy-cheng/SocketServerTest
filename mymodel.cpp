#include "mymodel.h"

MyModel::MyModel(SocketServer *parent) : QAbstractTableModel(parent)
{
    paServer = parent;
}

MyModel::~MyModel()
{
}

int MyModel::rowCount(const QModelIndex &) const
{
    int rowCount;

    rowCount = paServer->GetConnCnt();
    qDebug() << QString("rowCount:%1").arg(rowCount);
    return rowCount;
}

int MyModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    QString strItem;

    // generate a log message when this method gets called
    qDebug() << QString("data:row %1, col%2, role %3")
            .arg(row).arg(col).arg(role);

    switch (role) {
    case Qt::DisplayRole:
        strItem = paServer->GetCntString(row, col);
        if (strItem.isNull())
            return QString("Row%1, Column%2").arg(index.row()+1).arg(index.column()+1);
        else
            return strItem;
        break;

    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }

    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case TBL_SOCK_DESC:
                 return QString("SockDscrpt");
            case TBL_IP:
                return QString("IP");
            case TBL_PORT:
                return QString("Port");
            case TBL_STATUS:
                return QString("Status");
            }
        }
    }
    return QVariant();
}
