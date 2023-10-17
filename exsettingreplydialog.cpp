#include "exsettingreplydialog.hpp"

#include "qlineseries.h"
#include "qlogvalueaxis.h"
#include "qnamespace.h"
#include<QLogValueAxis>
#include<QChart>
#include "sysbackend.hpp"
#include "ui_exsettingreplydialog.h"
#include <algorithm>
#include<glog/logging.h>
#include <vector>
#include<QMessageBox>
#include<cassert>


#include"datamanager.h"
ExSettingReplyDialog::ExSettingReplyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExSettingReplyDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(3);
    QChart* chart=new QChart;
    ui->graphicsView->setChart(chart);

    QStringList headers{"频率","加速度","左斜率","右斜率","中断上限","中断下限"};

    ui->tableWidget->setHorizontalHeaderLabels(headers);

    QVector<QVector<QString>> table;//for dataManager
    table.emplaceBack(QVector<QString>{"20","100","","","6","-6"});
    table.emplaceBack(QVector<QString>{"200","500","","","6","-6"});
    table.emplaceBack(QVector<QString>{"3000","500","","","6","-6"});

    //0 origin 1 upper 2 lower
    pointMap[0]=QVector<QPointF>{QPointF{20,100},QPointF{200,500},QPointF{3000,500}};
    pointMap[1]=QVector<QPointF>{QPointF{20,200},QPointF{200,1000},QPointF{3000,1000}};
    pointMap[2]=QVector<QPointF>{QPointF{20,50},QPointF{200,250},QPointF{3000,250}};

    seriesMap[0]=new QLineSeries;
    seriesMap[1]=new QLineSeries;
    seriesMap[2]=new QLineSeries;


     xAxis=new QLogValueAxis;
     yAxis=new QLogValueAxis;

    xAxis->setBase(10);
    xAxis->setRange(20,10000);
    xAxis->setLabelFormat("%d Hz");
    yAxis->setBase(10);
    yAxis->setLabelFormat("%d g");
    yAxis->setRange(10,10000);

    chart->addAxis(xAxis,Qt::AlignBottom);
    chart->addAxis(yAxis,Qt::AlignLeft);
    chart->setTitle("加速度");

    for(int i=0;i<3;++i)
    {
        seriesMap[i]->replace(pointMap[i]);
        chart->addSeries(seriesMap[i]);
        seriesMap[i]->attachAxis(xAxis);
        seriesMap[i]->attachAxis(yAxis);

    }

    seriesMap[0]->setColor(Qt::green);
    seriesMap[1]->setColor(Qt::red);
    seriesMap[2]->setColor(Qt::red);

    seriesMap[0]->setName("理想频率");
    seriesMap[1]->setName("上限频率");
    seriesMap[2]->setName("下限频率");




    itemMatrix.resize(table.size());
    for(int i=0;i<itemMatrix.size();++i)
        itemMatrix[i].resize(6);

    for(int i=0;i<table.size();++i)
    {
        for(int j=0;j<6;++j)
        {
            auto item=new QTableWidgetItem{table[i][j]};
            ui->tableWidget->setItem(i,j,item);
            itemMatrix[i][j]=item;
        }
    }

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    initUi();

    on_pushButton_5_clicked();
}

void ExSettingReplyDialog::initUi()
{
     auto dManager=DataManager::GetInstance();
     auto& info=dManager->getExSettingInfo("ShockReplayExSettings");
     //都有自己的vector
     if(info.size()==0)
     {
         LOG(WARNING)<<"error when getting ShockReplayExSettings infos";
         return;
     }
     ui->doubleSpinBox_4->setValue(info[0].toDouble());//参考频率
     ui->doubleSpinBox_2->setValue(info[1].toDouble());//下限频率
     ui->doubleSpinBox_3->setValue(info[2].toDouble());//上限频率
     ui->comboBox_2->setCurrentText(info[3]);//倍频分析率
     ui->comboBox_3->setCurrentText(info[4]);//冲击响应类型
     ui->doubleSpinBox_5->setValue(info[5].toDouble());//采样时间
     ui->doubleSpinBox_6->setValue(info[6].toDouble());//冲击时间
     ui->comboBox->setCurrentText(info[7]);//采样频率
     ui->doubleSpinBox_7->setValue(info[8].toDouble());//复位时间


}

ExSettingReplyDialog::~ExSettingReplyDialog()
{
    delete ui;
}

void ExSettingReplyDialog::on_pushButton_2_clicked()
{
    auto list=ui->tableWidget->selectedItems();

    if(list.size()>0)
    {
        itemMatrix.remove(list[0]->row());
        ui->tableWidget->removeRow(list[0]->row());
        
    }

}

void ExSettingReplyDialog::on_pushButton_clicked()
{
    int rowCount=ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(rowCount+1);
    itemMatrix.push_back(QVector<QTableWidgetItem*>{6});
    for(int j=0;j<4;++j)
    {
        auto item=new QTableWidgetItem{""};
        ui->tableWidget->setItem(rowCount,j,item);
        itemMatrix[rowCount][j]=item;
    }

    auto item=new QTableWidgetItem{"6"};
    ui->tableWidget->setItem(rowCount,4,item);
    itemMatrix[rowCount][4]=item;

    item=new QTableWidgetItem{"-6"};
    ui->tableWidget->setItem(rowCount,5,item);
    itemMatrix[rowCount][5]=item;
    qDebug()<<"item Matrix"<<itemMatrix;
}

double ExSettingReplyDialog::getK(const QPointF& p1,const QPointF& p2)
{
    double upper=(log(p2.y())/log(10))-(log(p1.y())/log(10));
    double lower=(log(p2.x())/log(2))-(log(p1.x())/log(2));

    return (upper/lower)*20;
}

double ExSettingReplyDialog::getValueFromDb(double db,double value)
{
    //(db/20)=log10(p2/p1)
    auto leftSide=pow(10,db/20);
    return leftSide*value;

}


QPointF ExSettingReplyDialog::getValueFromK(const QPointF& p1,double k,
                                            double another,bool type)
{
    auto C2=log(p1.y())/log(10);
    auto C1=log(p1.x())/log(2);

    if(type)// has x
    {
       auto rate=k/20;
       auto leftSide=C2-(rate*C1)+(rate*(log(another)/log(2)));

       QPointF rec{another,pow(10,leftSide)};
       return rec;
    }
    else
    {
        auto rate=20/k;
        auto leftSide=((-rate)*C2)+C1+(rate*(log(another)/log(10)));
        QPointF rec{another,pow(2,leftSide)};
        return rec;
    }
}


void ExSettingReplyDialog::on_pushButton_3_clicked()//refresh
{
    int rowCount=ui->tableWidget->rowCount();
    assert(itemMatrix.size()==rowCount);

    bool hasLeftX=(itemMatrix[0][0]->text()!="");
    bool hasLeftY=(itemMatrix[0][1]->text()!="");
    bool hasRightX=(itemMatrix[1][0]->text()!="");
    bool hasRightY=(itemMatrix[1][1]->text()!="");
    bool hasRightK=(itemMatrix[0][3]->text()!="");

    if(hasLeftX&&hasLeftY)
    {
        QPointF p1{itemMatrix[0][0]->text().toDouble(),itemMatrix[0][1]->text().toDouble()};
        if(hasRightX&&hasRightY)
        {
            QPointF p2{itemMatrix[1][0]->text().toDouble(),itemMatrix[1][1]->text().toDouble()};
            auto k=this->getK(p1,p2);
            itemMatrix[0][3]->setText(QString::number(k));
        }
        else if(hasRightX&&hasRightK)
        {
             auto p2=this->getValueFromK(p1,itemMatrix[0][3]->text().toDouble(),
                     itemMatrix[1][0]->text().toDouble(),true);
             itemMatrix[1][0]->setText(QString::number(p2.x()));
             itemMatrix[1][1]->setText(QString::number(p2.y()));
        }
        else if(hasRightY&&hasRightK)
        {
            auto p2=this->getValueFromK(p1,itemMatrix[0][3]->text().toDouble(),
                    itemMatrix[1][1]->text().toDouble(),false);
            itemMatrix[1][0]->setText(QString::number(p2.x()));
            itemMatrix[1][1]->setText(QString::number(p2.y()));
        }
        else
        {
            QMessageBox::warning(this,"警告","错误的输入(行1)");
            return;
        }
    }
    else
    {
        QMessageBox::warning(this,"警告","错误的输入(行1)");
        return;
    }

    //for middle
         for(int i=1;i<rowCount-1;++i)
         {
             int previous=i-1;
             int     post=i+1;

             hasLeftX=(itemMatrix[previous][0]->text()!="");
             hasLeftY=(itemMatrix[previous][1]->text()!="");
             hasRightX=(itemMatrix[post][0]->text()!="");
             hasRightY=(itemMatrix[post][1]->text()!="");
             hasRightK=(itemMatrix[i][2]->text()!="");
             bool hasCurrentX=(itemMatrix[i][0]->text()!="");
             bool hasCurrentY=(itemMatrix[i][1]->text()!="");
             bool hasPreviousRightK=(itemMatrix[previous][3]->text()!="");


             if(hasCurrentX&&hasCurrentY)
             {
                 if(hasPreviousRightK)
                 {
                    itemMatrix[i][2]->setText(itemMatrix[previous][3]->text());
                 }
                  QPointF p1{itemMatrix[i][0]->text().toDouble(),itemMatrix[i][1]->text().toDouble()};

                 if(hasRightX&&hasRightY)
                 {
                     QPointF p2{itemMatrix[post][0]->text().toDouble(),itemMatrix[post][1]->text().toDouble()};
                     auto k=this->getK(p1,p2);
                     itemMatrix[i][3]->setText(QString::number(k));
                 }
                 else if(hasRightX&&hasRightK)
                 {
                      auto p2=this->getValueFromK(p1,itemMatrix[i][3]->text().toDouble(),
                              itemMatrix[post][0]->text().toDouble(),true);
                      itemMatrix[post][0]->setText(QString::number(p2.x()));
                      itemMatrix[post][1]->setText(QString::number(p2.y()));
                 }
                 else if(hasRightY&&hasRightK)
                 {
                     auto p2=this->getValueFromK(p1,itemMatrix[i][3]->text().toDouble(),
                             itemMatrix[post][1]->text().toDouble(),false);
                     itemMatrix[post][0]->setText(QString::number(p2.x()));
                     itemMatrix[post][1]->setText(QString::number(p2.y()));
                 }
                 else
                 {
                     QMessageBox::warning(this,"警告",QString("错误的输入(行%1)").arg(i));
                     return;
                 }
             }
             else
             {
                 QMessageBox::warning(this,"警告",QString("错误的输入(行%1)").arg(i));
                 return;
             }

         }
    //end for middle

    int maxRowI=rowCount-1;

    hasLeftX=(itemMatrix[maxRowI-1][0]->text()!="");
    hasLeftY=(itemMatrix[maxRowI-1][1]->text()!="");
    hasRightX=(itemMatrix[maxRowI][0]->text()!="");
    hasRightY=(itemMatrix[maxRowI][1]->text()!="");
    bool hasLeftK=(itemMatrix[maxRowI][2]->text()!="");

    if(hasRightX&&hasRightY)
    {
        QPointF p2{itemMatrix[maxRowI][0]->text().toDouble(),
                    itemMatrix[maxRowI][1]->text().toDouble()};
        if(hasLeftX&&hasLeftY)
        {
            QPointF p1{itemMatrix[maxRowI-1][0]->text().toDouble(),
                        itemMatrix[maxRowI-1][1]->text().toDouble()};
            auto k=this->getK(p1,p2);
            itemMatrix[maxRowI][2]->setText(QString::number(k));
        }
        else if(hasLeftX&&hasLeftK)
        {
            auto p1=this->getValueFromK(p2,itemMatrix[maxRowI][2]->text().toDouble(),
                    itemMatrix[maxRowI][0]->text().toDouble(),true);
            itemMatrix[maxRowI-1][0]->setText(QString::number(p1.x()));
            itemMatrix[maxRowI-1][1]->setText(QString::number(p1.y()));
        }
        else if(hasLeftY&&hasLeftK)
        {
            auto p1=this->getValueFromK(p2,itemMatrix[maxRowI][2]->text().toDouble(),
                    itemMatrix[maxRowI][1]->text().toDouble(),false);
            itemMatrix[maxRowI-1][0]->setText(QString::number(p1.x()));
            itemMatrix[maxRowI-1][1]->setText(QString::number(p1.y()));
        }
    }



}


void ExSettingReplyDialog::on_pushButton_4_clicked()
{
    updateChart();
}

void ExSettingReplyDialog::updateChart()
{
    LOG(INFO)<<"replyChart updated";

    //1.根据中断上限等更新series linemap。。。
    //2.根据ui等更新坐标轴(Keyixiugia)
    QVector<QPointF> origins;
    for(int i=0;i<itemMatrix.size();++i)
    {
        if(itemMatrix[i][0]->text()==""||itemMatrix[i][1]->text()=="")
        {
            continue;
        }
        auto x=itemMatrix[i][0]->text().toDouble();
        auto y=itemMatrix[i][1]->text().toDouble();
        origins.emplace_back(QPointF{x,y});
    }

    pointMap[0]=origins;
    seriesMap[0]->replace(origins);

    auto tmp=origins;
    for(int i=0;i<origins.size();++i)
    {
        auto y=getValueFromDb(itemMatrix[i][4]->text().toDouble(),origins[i].y());
        tmp[i].setY(y);
    }
    pointMap[1]=tmp;
    seriesMap[1]->replace(tmp);

    tmp=origins;
    for(int i=0;i<origins.size();++i)
    {
        auto y=getValueFromDb(itemMatrix[i][5]->text().toDouble(),origins[i].y());
        tmp[i].setY(y);
    }
    pointMap[2]=tmp;
    seriesMap[2]->replace(tmp);

    sendStandardLine(1,QList<QLineSeries*>{seriesMap[0],seriesMap[1],seriesMap[2]});

    ui->graphicsView->update();


}



void ExSettingReplyDialog::on_pushButton_5_clicked()
{
    //确定
    QList<QString> argList;
    argList<<QString::number(ui->doubleSpinBox_4->value());//参考频率
    argList<<QString::number(ui->doubleSpinBox_2->value());//下限频率
    argList<<QString::number(ui->doubleSpinBox_3->value());//上限频率
    argList<<ui->comboBox_2->currentText();//倍频分析率
    argList<<ui->comboBox_3->currentText();//冲击响应类型
    argList<<QString::number(ui->doubleSpinBox_5->value());//采样时间
    argList<<QString::number(ui->doubleSpinBox_6->value());//冲击时间
    argList<<ui->comboBox->currentText();//采样频率
    argList<<QString::number(ui->doubleSpinBox_7->value());//复位时间

    DataManager::GetInstance()->changeExArgValueEx("ShockReplayExSettings",-1,argList);

    QMap<QString,QPair<QString,int>> sampleValMap;
    sampleValMap["384000"]=QPair<QString,int>{"500000",1};
    sampleValMap["192000"]=QPair<QString,int>{"200000",2};
    sampleValMap["96000"]=QPair<QString,int>{"100000",3};
    sampleValMap["48000"]=QPair<QString,int>{"50000",4};
    sampleValMap["24000"]=QPair<QString,int>{"20000",5};
    sampleValMap["12000"]=QPair<QString,int>{"10000",6};
    sampleValMap["6000"]=QPair<QString,int>{"5000",7};
    sampleValMap["3000"]=QPair<QString,int>{"2000",8};
    sampleValMap["1500"]=QPair<QString,int>{"1000",9};



    auto SampleF=argList[7];
    auto SampleT=argList[5];
    if(SampleF=="自动大小")
        SampleF="96000";

    qint64 rate1=sampleValMap[SampleF].first.toLongLong();

    QString acq_len=QString::number(rate1*(SampleT.toUInt()*0.001));

    auto delayRate=ui->doubleSpinBox_8->value()/100;
    uint delayReal=(acq_len.toUInt()*delayRate);


    emit DataManager::GetInstance()
            ->updateBackEndArgument(SYSBackEnd::ArgType::ACQ_LEN,acq_len);
    emit DataManager::GetInstance()
            ->updateBackEndArgument(SYSBackEnd::ArgType::ACQ_RATE,
                                    QString::number(sampleValMap[SampleF].second));
    emit DataManager::GetInstance()
            ->updateBackEndArgument(SYSBackEnd::ArgType::DLY_LEN,
                                    QString::number(delayReal));

    QVector<mTargetPoint> targetSequence;

    for(int i=0;i<itemMatrix.size();++i)
    {
        targetSequence.emplace_back(mTargetPoint{itemMatrix[i][0]->text().toDouble(),
                                         itemMatrix[i][1]->text().toDouble(),
                                         itemMatrix[i][4]->text().toDouble(),
                                         itemMatrix[i][5]->text().toDouble()});
    }
    DataManager::GetInstance()->replaceTargetPoint(targetSequence);
}

