#include "channelargdialog.h"
#include "datamanager.h"
#include<QComboBox>
#include "qcombobox.h"
#include "qnamespace.h"
#include "qtablewidget.h"
#include "ui_channelargdialog.h"
#include"projectunitdialog.h"
#include<chrono>
#include<glog/logging.h>
#include <ratio>
#include<QFileDialog>
ChannelArgDialog::ChannelArgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelArgDialog)
{
    ui->setupUi(this);
    setWindowTitle("通道参数");
    inputChannelTableConfiguration();
    flurTableConfiguration();
    stopRateConfiguration();
    callTableConfiguraion();
}

ChannelArgDialog::~ChannelArgDialog()
{
    delete ui;
}

void ChannelArgDialog::on_unitButton_clicked()
{
    projectUnitDialog* dialog=new projectUnitDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&projectUnitDialog::changeChannelValue,this,
            &ChannelArgDialog::changeChannelValue);

    dialog->exec();
}

void ChannelArgDialog::inputChannelTableConfiguration()
{
    ui->inputChannelTable->setColumnCount(5);
    ui->inputChannelTable->setRowCount(4);
    QStringList labels{"类型","耦合方式","传感器类型","灵敏度(值)","灵敏度(单位)"};
    ui->inputChannelTable->setHorizontalHeaderLabels(labels);
    labels.clear();

    for(int i=0;i<4;i++)
    {
        QComboBox* box=new QComboBox;
        labels<<"测试"<<"禁用";
        box->addItems(labels);
        ui->inputChannelTable->setCellWidget(i,0,box);
        mapFromCombobox_inputChannel.insert(box,QPair<int,int>(i,0));
        labels.clear();

        QComboBox* box1=new QComboBox;
        labels<<"AC单端"<<"AC差分"<<"DC单端"<<"DC差分"<<"IEPE"<<"电荷";
        box1->addItems(labels);
        ui->inputChannelTable->setCellWidget(i,1,box1);
        mapFromCombobox_inputChannel.insert(box1,QPair<int,int>(i,1));
        labels.clear();

        QComboBox* box2=new QComboBox;
        labels<<"加速度";
        box2->addItems(labels);
        ui->inputChannelTable->setCellWidget(i,2,box2);
        mapFromCombobox_inputChannel.insert(box2,QPair<int,int>(i,2));

        QTableWidgetItem* widgetItem1=new QTableWidgetItem{"mV/(g)"};
        ui->inputChannelTable->setItem(i,4,widgetItem1);
        widgetItem1 = new QTableWidgetItem{  };
        ui->inputChannelTable->setItem(i, 3, widgetItem1);

        labels.clear();
    }

    auto& table=DataManager::GetInstance()->getChannelInfo("InputChannel");
    for(int i=0;i<4;++i)//用json里的数据更新图表
    {
        for(int j=0;j<4;++j)
        {

            if(j<3)
            {
                auto& itemName=table[i][j];
                for(auto iter=mapFromCombobox_inputChannel.cbegin();
                    iter!=mapFromCombobox_inputChannel.end();++iter)
                {

                    if(iter->first==i&&iter->second==j)
                    {
                        iter.key()->setCurrentText(itemName);
                        break;
                    }
                }
            }
            else
            {
                ui->inputChannelTable->item(i,j)->setText(table[i][j]);
            }
        }
    }

    //电荷对应相对单位

    for(auto iter=mapFromCombobox_inputChannel.cbegin();
        iter!=mapFromCombobox_inputChannel.end();++iter)
    {

        if(iter->second==1)
        {
            connect(iter.key(),&QComboBox::currentTextChanged,[=](const QString& t)
            {
                if(iter.key()->currentText()=="电荷")
                {
                    ui->inputChannelTable->item(iter->first,4)->setText("pC/(g)");
                }
                else
                {
                    ui->inputChannelTable->item(iter->first,4)->setText("mV/(g)");
                }
            });
            continue;
        }
    }
}

void ChannelArgDialog::flurTableConfiguration()
{
    auto before=std::chrono::high_resolution_clock::now();
    ui->flurTable->setColumnCount(6);
    ui->flurTable->setRowCount(4);
    QStringList labels{"状态","截止频率(HZ)","带外衰减(dB)","方式","等级","滤波比"};
    ui->flurTable->setHorizontalHeaderLabels(labels);


    auto& table=DataManager::GetInstance()->getChannelInfo("Flur");
    for(int i=0;i<4;i++)
    {
        QComboBox* box=new QComboBox;
        box->addItem("开");
        box->addItem("关");
        ui->flurTable->setCellWidget(i,0,box);
        mapFromCombobox_Flur.insert(box,QPair<int,int>{i,0});
        box->setCurrentText(table[i][0]);


        box=new QComboBox;
        box->addItem("自定义");
        box->addItem("自动");
        box->addItem("SAE J211");
        box->addItem("ISO 6487");
        ui->flurTable->setCellWidget(i,3,box);
        mapFromCombobox_Flur.insert(box,QPair<int,int>{i,3});
        box->setCurrentText(table[i][3]);

        box=new QComboBox;
        box->addItem("CFC60");
        box->addItem("CFC180");
        box->addItem("CFC600");
        box->addItem("CFC1000");
        box->addItem("CFC3000");
        ui->flurTable->setCellWidget(i,4,box);
        mapFromCombobox_Flur.insert(box,QPair<int,int>{i,4});
        box->setCurrentText(table[i][4]);

        QTableWidgetItem* item1=new QTableWidgetItem{table[i][1]};
        QTableWidgetItem* item2=new QTableWidgetItem{table[i][2]};
        QTableWidgetItem* item3=new QTableWidgetItem{table[i][5]};

        ui->flurTable->setItem(i,1,item1);
        ui->flurTable->setItem(i,2,item2);
        ui->flurTable->setItem(i,5,item3);
    }

   auto after=std::chrono::high_resolution_clock::now();

   LOG(INFO)<<"time of single table configuration:"<<std::chrono::duration_cast
              <std::chrono::milliseconds>(after-before).count();

}

void ChannelArgDialog::stopRateConfiguration()
{
    ui->DRatioTable->setColumnCount(2);
    ui->DRatioTable->setRowCount(4);
    QStringList labels{"阻尼比","Q值"};
    ui->DRatioTable->setHorizontalHeaderLabels(labels);

    auto& table=DataManager::GetInstance()->getChannelInfo("dampingRatio");
    for(int i=0;i<4;i++)
    {
        QTableWidgetItem* item=new QTableWidgetItem{table[i][0]};
        QTableWidgetItem* item1=new QTableWidgetItem{table[i][1]};

        ui->DRatioTable->setItem(i,0,item);
        ui->DRatioTable->setItem(i,1,item1);
    }


    connect(ui->DRatioTable,&QTableWidget::itemChanged,
            this,&ChannelArgDialog::processStopRate);
}

void ChannelArgDialog::processStopRate(QTableWidgetItem* item)
{
   if(ui->DRatioTable->column(item)==0)
   {
      auto brotherItem=ui->DRatioTable->item(item->row(),1);
      double cValue=item->text().toDouble();
      double qValue=1/(2*cValue);
      brotherItem->setText(QString::number(qValue));

   }
   else
   {
       auto brotherItem=ui->DRatioTable->item(item->row(),0);
       double qValue=item->text().toDouble();
       double cValue=1/(2*qValue);
       brotherItem->setText(QString::number(cValue));
   }
}

void ChannelArgDialog::callTableConfiguraion()
{
    ui->callibrateTable->setColumnCount(2);
    ui->callibrateTable->setRowCount(4);
    QStringList labels{"去偏值方式","偏值（g）"};
    ui->callibrateTable->setHorizontalHeaderLabels(labels);

    auto& table=DataManager::GetInstance()->getChannelInfo("callibrate");
    for(int i=0;i<4;i++)
    {
        QComboBox* item=new QComboBox;
        item->addItem("自动");
        item->addItem("自定义");
        
        QTableWidgetItem* item1=new QTableWidgetItem{table[i][1]};
        auto flags = item1->flags();
        
        flags ^= Qt::ItemIsEditable;
         item1->setFlags(flags);
        ui->callibrateTable->setCellWidget(i,0,item);

        connect(item,&QComboBox::currentIndexChanged,
              this,&ChannelArgDialog::processCallibrate);


        ui->callibrateTable->setItem(i,1,item1);
        item->setCurrentText(table[i][0]);
    }

  //connect(ui->callibrateTable,&QTableWidget::itemChanged,
       // this,&ChannelArgDialog::processCallibrate);
}

void ChannelArgDialog::processCallibrate(int index)
{
     QComboBox* senderItem=qobject_cast<QComboBox*>(sender());
     if(senderItem==nullptr)
     {
         return;
     }
     bool activate;
     if(index==1)
     {
         activate=true;
     }
     else
     {
         activate=false;
     }
     auto brotherItem=ui->callibrateTable->item(
                 ui->callibrateTable->currentRow(),1);
     auto flags=brotherItem->flags();
     if(activate)
     {
        flags^=Qt::ItemIsEditable;
        brotherItem->setFlags(flags);
     }
     else
     {
         flags&=Qt::ItemIsEditable;
         brotherItem->setFlags(flags);
     }

}

void ChannelArgDialog::on_pushButton_3_clicked()//cancelbtn
{
    close();
}


void ChannelArgDialog::on_pushButton_5_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存配置文件"),
                               DataManager::GetInstance()->appCachePath,
                               tr("通道参数配置文件 (*.argConfig)"));
    if(fileName=="")
    {
        return;
    }
    LOG(INFO)<<"argConfig save path:"<<fileName.toStdString();
    updateArgData();
    DataManager::GetInstance()->saveChannelInfoFile(fileName);

}


void ChannelArgDialog::updateArgData()
{
    for(int i=0;i<4;i++)
    {
        emit changeChannelValue("dampingRatio",i,
                                ui->DRatioTable->item(i,0)->text(),
                                ui->DRatioTable->item(i,1)->text());
        emit changeChannelValue("callibrate",i,
                                qobject_cast<QComboBox*>(ui->callibrateTable->
                                                         cellWidget(i,0))->currentText(),
                                ui->callibrateTable->item(i,1)->text());
        QList<QString> arg;
        arg<<qobject_cast<QComboBox*>(ui->flurTable->cellWidget(i,0))->currentText();
        arg<<ui->flurTable->item(i,1)->text();
        arg<<ui->flurTable->item(i,2)->text();
        arg<<qobject_cast<QComboBox*>(ui->flurTable->cellWidget(i,3))->currentText();
        arg<<qobject_cast<QComboBox*>(ui->flurTable->cellWidget(i,4))->currentText();
        arg<<ui->flurTable->item(i,5)->text();
        emit changeChannelValueEx("Flur",i,arg);
        arg.clear();


        arg<<qobject_cast<QComboBox*>(ui->inputChannelTable->cellWidget(i,0))->currentText();
        arg<<qobject_cast<QComboBox*>(ui->inputChannelTable->cellWidget(i,1))->currentText();
        arg<<qobject_cast<QComboBox*>(ui->inputChannelTable->cellWidget(i,2))->currentText();
        arg<<ui->inputChannelTable->item(i,3)->text();
        arg<<ui->inputChannelTable->item(i,4)->text();
        emit changeChannelValueEx("InputChannel",i,arg);
        arg.clear();
    }
}

void ChannelArgDialog::on_pushButton_4_clicked()//imortBtn
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("导入配置文件"),
                                                  DataManager::GetInstance()->appCachePath,
                                                  tr("通道参数配置文件 (*.argConfig)"));
    if(fileName=="")
    {
        return;
    }
    qDebug()<<fileName;
    qDebug()<<DataManager::GetInstance()->setChannelInfoFile(fileName);
    emit rebuild();
    this->close();

}


void ChannelArgDialog::on_pushButton_2_clicked()
{
    //okBtn
    updateArgData();

    for(int i=0;i<4;++i)
    {
        auto funcType=qobject_cast<QComboBox*>
                (ui->inputChannelTable->cellWidget(i,1))->currentIndex();
        int method=0;

        if(funcType==4)
        {
            funcType=6;
        }
        else if(funcType==5)
        {
            funcType=3;
        }
        else
        {
            if(funcType%2==0)
            {
                method=0;
            }
            else
            {
                method=1;
            }
            funcType=0;
        }

        auto sesitivity=ui->inputChannelTable->item(i,3)->text().toDouble()/100;
        const int rateVal=10;

        auto trgLv=sesitivity*rateVal/10*32768;

        emit DataManager::GetInstance()
                ->updateBackEndArgument(SYSBackEnd::ArgType::CH_FUNC_TYPE,
                                        QString::number(funcType),i);
        emit DataManager::GetInstance()
                ->updateBackEndArgument(SYSBackEnd::ArgType::CH_SENSITIVITY,
                                        QString::number(sesitivity),i);
        emit DataManager::GetInstance()
                ->updateBackEndArgument(SYSBackEnd::ArgType::CH_INPUT_TYPE,
                                        QString::number(method),i);
        emit DataManager::GetInstance()
                ->updateBackEndArgument(SYSBackEnd::ArgType::TRIG_LEVEL,
                                        QString::number(trgLv),i);
    }



    this->close();
}


void ChannelArgDialog::on_pushButton_clicked()//to default button
{
    qDebug()<<DataManager::GetInstance()->setChannelInfoFile(
                  DataManager::GetInstance()->ChannelArgTemplateFilePath);
    emit rebuild();
    this->close();
}

