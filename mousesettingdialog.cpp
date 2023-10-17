#include "mousesettingdialog.h"
#include "qcolordialog.h"
#include "qlistwidget.h"
#include "ui_mousesettingdialog.h"
#include<QColorDialog>
#include"lineitem.h"

QMap<QString,LineItem*>  mouseSettingDialog::lineMap{};
int                  mouseSettingDialog::current=0;

mouseSettingDialog::mouseSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mouseSettingDialog)
{
    ui->setupUi(this);

     ;
    ui->label_4->setStyleSheet(QString("background-color:%1").arg(color.name()));

    for(auto iter=lineMap.begin();iter!=lineMap.end();++iter)
    {
        ui->listWidget_3->addItem(new QListWidgetItem{iter.key()});
    }
}

mouseSettingDialog::~mouseSettingDialog()
{

    delete ui;
}

void mouseSettingDialog::on_pushButton_7_clicked()
{
    auto c=QColorDialog::getColor(color,this);
    if(c.isValid())
    {
        color=c;
        ui->label_4->setStyleSheet(QString("background-color:%1").arg(color.name()));
        auto items=ui->listWidget_3->selectedItems();
        for(auto&& i:items)
        {
            lineMap[i->text()]->setColor(color);
        }
    }
}


void mouseSettingDialog::on_pushButton_8_clicked()
{
    auto item=emit addLineCursor();
    QString mark=QString{"光标%1"}.arg(current);
    lineMap[mark]=item;
    ui->listWidget_3->addItem(new QListWidgetItem(mark));
    current++;
}


void mouseSettingDialog::on_comboBox_currentIndexChanged(int index)
{
    auto items=ui->listWidget_3->selectedItems();
    for(auto&& i:items)
    {
        lineMap[i->text()]->setLineWidth(index+1);
    }
}


void mouseSettingDialog::on_comboBox_2_currentIndexChanged(int index)
{
    auto items=ui->listWidget_3->selectedItems();
    for(auto&& i:items)
    {
        lineMap[i->text()]->setLineShape(index+1);
    }
}


void mouseSettingDialog::on_pushButton_clicked()
{
    close();
}


void mouseSettingDialog::on_pushButton_2_clicked()
{
    close();
}

