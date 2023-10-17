#include "shockreplaytable.hpp"
#include "publicDefs.hpp"
#include "ui_shockreplaytable.h"

shockReplayTable::shockReplayTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::shockReplayTable)
{
    ui->setupUi(this);
     ui->tableWidget->setColumnCount(7);
     ui->tableWidget->setRowCount(2);
     QStringList headLabel;
    headLabel<<"名称"<<"阻尼比"<<"最大值(g)"
          <<"频率(Hz)"<<"最小值(g)"
           <<"频率(Hz)"<<"容差"
          ;
     ui->tableWidget->setHorizontalHeaderLabels(headLabel);
}

shockReplayTable::~shockReplayTable()
{
    delete ui;
}

void shockReplayTable::setSrsBar(SrsStateBar bar,int ch)
{
    const int dstRow =ch;
    if(ui->tableWidget->rowCount()<dstRow)
    {
        ui->tableWidget->setRowCount(dstRow);
    }

    QList<QTableWidgetItem*> itemList;
    for(int i=0;i<7;++i)
    {
        itemList<<new QTableWidgetItem;
    }
    itemList[1]->setText(bar.getDampRate());
    itemList[2]->setText(bar.getMaxVal());
    itemList[3]->setText(bar.getMaxFre());
    itemList[4]->setText(bar.getMinVal());
    itemList[5]->setText(bar.getMinFre());
    itemList[6]->setText(bar.getExceed());

    for(int i=0;i<7;++i)
    {
        ui->tableWidget->setItem(dstRow,i,itemList[i]);
    }
}
