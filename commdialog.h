#ifndef COMMDIALOG_H
#define COMMDIALOG_H

#include <QDialog>

namespace Ui {
class commDialog;
}

class commDialog : public QDialog
{
    Q_OBJECT

public:
    explicit commDialog(QWidget *parent = 0);
    ~commDialog();
    void setCommData(QString, QString, QString);

private:
    Ui::commDialog *ui;
};

#endif // COMMDIALOG_H
