#include "connectinputdialog.h"
#include "qhostaddress.h"
#include "qregularexpression.h"
#include "ui_connectinputdialog.h"
#include<QRegularExpression>
#include<QMessageBox>
ConnectInputDialog::ConnectInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectInputDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText("127.0.0.1");
}

ConnectInputDialog::~ConnectInputDialog()
{
    delete ui;
}

void ConnectInputDialog::on_okButton_clicked()
{


    QRegularExpression ipREx{"((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.)"
                             "{3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))"};
    QRegularExpression poREx{"([0-9]|[1-9]\\d{1,3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]"
                             "\\d{2}|655[0-2]\\d|6553[0-5])"};


    QRegularExpressionMatch match=ipREx.match(ui->lineEdit->text());
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"提示","请输入正确的IP地址");
        return;
    }
    match=poREx.match(ui->lineEdit_2->text());
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"提示","请输入正确的端口号");
        return;
    }

    QHostAddress add(ui->lineEdit->text());
    emit this->connectStart(add
                            ,ui->lineEdit_2->text().toLong());


    this->close();


}


void ConnectInputDialog::on_cancelButton_clicked()
{
    this->close();
}

