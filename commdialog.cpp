#include "commdialog.h"
#include "ui_commdialog.h"

commDialog::commDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::commDialog)
{
    ui->setupUi(this);
}

commDialog::~commDialog()
{
    delete ui;
}

void commDialog:: setCommData(QString sockDesc, QString IP, QString Port)
{
    ui->leSockDesc->setText(sockDesc);
    ui->leIP->setText(IP);
    ui->lePort->setText(Port);
}
