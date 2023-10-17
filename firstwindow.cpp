#include "firstwindow.h"

#include "qmessagebox.h"
#include "qnamespace.h"
#include "ui_firstwindow.h"
#include"connectinputdialog.h"
firstWindow::firstWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::firstWIndow)
{
    ui->setupUi(this);
}

firstWindow::~firstWindow()
{
    delete ui;
}

void firstWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void firstWindow::on_connectToHostButton_clicked()
{
   ConnectInputDialog* dialog=new ConnectInputDialog(this);
   dialog->setAttribute(Qt::WA_DeleteOnClose);
   connect(dialog,&ConnectInputDialog::connectStart,this,[=](QHostAddress& ad,long p)
   {
       add=ad;
       port=p;

       emit connectToDevice(add,p);
   });
   dialog->show();
}

void firstWindow::isConnectedToDevice(bool s)
{
   if(s)
   {
       QMessageBox::information(this,"提示","成功连接到设备");
       ui->label_4->setText(add.toString());
       ui->label_5->setText(QString("%1").arg(port));
       this->isDeviceConnected=true;
   }
   else
   {
       QMessageBox::warning(this,"提示","连接到设备失败，请检查相关参数并重新连接");
       this->isDeviceConnected=false;
   }
}


void firstWindow::on_pushButton_6_clicked()
{
    if(isDeviceConnected)
    {
       emit this->activateMainwindow(MainWindow::ExType::HorizontalSHOCK);
        close();
    }
    else
    {
        QMessageBox::warning(this,"提示","设备未连接");
        emit this->activateMainwindow(MainWindow::ExType::HorizontalSHOCK);
        close();
    }
}


void firstWindow::on_pushButton_7_clicked()
{
    if(isDeviceConnected)
    {
       emit this->activateMainwindow(MainWindow::ExType::SHOCKReply);
        close();
    }
    else
    {
        QMessageBox::warning(this,"提示","设备未连接");
        emit this->activateMainwindow(MainWindow::ExType::SHOCKReply);
        close();
    }
}

