#include "displaysettingdialog.h"
#include "qcombobox.h"
#include "qnamespace.h"
#include "qobject.h"
#include "ui_displaysettingdialog.h"
#include<QTableWidgetItem>
#include<QComboBox>
#include<QCheckBox>
#include<QPushButton>
#include<QColorDialog>
#include"datamanager.h"
DisplaySettingDialog::DisplaySettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplaySettingDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount(4);
    ui->tableWidget->setColumnCount(5);
    QStringList labels{"是否启用","显示名称","曲线颜色","曲线宽度","曲线类型"};
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    ui->tableWidget->setVerticalHeader(nullptr);

    auto& table=DataManager::GetInstance()->getDisplayInfo("DisplaySettings");
    colorList.resize(4);
    for(int i=0;i<4;++i)
    {
        QCheckBox*        w0=new QCheckBox;
        QTableWidgetItem* w1=new QTableWidgetItem{QString("输入%1（t）").arg(i)};
        QPushButton*      w2=new QPushButton;
        QComboBox*        w3=new QComboBox;
        QComboBox*        w4=new QComboBox;

        mapToPosition.insert(w0,QPair<int, int>(i, 0));
        mapToPosition.insert(w2,QPair<int,int>(i,2));
        mapToPosition.insert(w3,QPair<int,int>(i,3));
        mapToPosition.insert(w4,QPair<int,int>(i,4));


        QStringList items{"实线","虚线","点","点划线","双点划线"};
        w4->addItems(items);
        items.clear();
        items<<"1"<<"2"<<"3"<<"4"<<"5";
        w3->addItems(items);



        connect(w2,&QPushButton::clicked,this,&DisplaySettingDialog::changeColorFunc);
        connect(w3,&QComboBox::currentTextChanged,this,&DisplaySettingDialog::onWidthChanged);
        connect(w0,&QCheckBox::stateChanged,this,[=](int state)
        {
           auto box=qobject_cast<QCheckBox*>(sender());
           auto& posi=mapToPosition.find(box).value();
           if(state==0)
           {
               emit checkBoxStateChanged(posi.first,false);
           }
           else if(state==2)
           {
                emit checkBoxStateChanged(posi.first,true);
           }

        });
        connect(w4,&QComboBox::currentTextChanged,this,[=](const QString& t)
        {
            auto box=qobject_cast<QComboBox*>(sender());
            auto& posi=mapToPosition.find(box).value();
            emit changeLinePattern(posi.first,t);
        });

        ui->tableWidget->setCellWidget(i,0,w0);
        ui->tableWidget->setItem(i,1,w1);
        ui->tableWidget->setCellWidget(i,2,w2);
        ui->tableWidget->setCellWidget(i,3,w3);
        ui->tableWidget->setCellWidget(i,4,w4);


        qDebug() << "w0:" << w0;
        qDebug() << "i:" << i;
        if(table[i][0]=="true")
        {
            w0->setCheckState(Qt::Checked);
        }
        else
        {
            w0->setCheckState(Qt::Unchecked);
        }

        QString sheet=QString{"background-color:%1"}.arg(table[i][1]);
        w2->setStyleSheet(sheet);
        w3->setCurrentText(table[i][2]);
        w4->setCurrentText(table[i][3]);
        colorList[i]=table[i][1];


    }

    connect(this,QOverload<const QString& ,int ,int,
            const QString&>::of(&DisplaySettingDialog::changeExSettingValue),
            DataManager::GetInstance(),QOverload<const QString& ,int,int,
            const QString&>::of(&DataManager::changeDisplaySettingValue));

    connect(this,QOverload<const QString& ,int ,
            const QList<QString>&>::of(&DisplaySettingDialog::changeExSettingValue),
            DataManager::GetInstance(),QOverload<const QString& ,int ,
            const QList<QString>&>::of(&DataManager::changeDisplaySettingValue));



    updateChartState();

}

void DisplaySettingDialog::onWidthChanged(const QString& text)
{
    QComboBox* box=qobject_cast<QComboBox*>(sender());
    auto& posi=mapToPosition.find(box).value();

    emit changeExSettingValue("DisplaySettings",posi.first,2,text);
    emit changeLineWidthSignal(posi.first,text.toInt());
}

void DisplaySettingDialog::changeColorFunc()
{
   QPushButton* bu=qobject_cast<QPushButton*>(sender());


   QColor newColor=QColorDialog::getColor(Qt::white,this);
   if(!newColor.isValid())
   {
       return;
   }
   QString sheet=QString{"background-color:%1"}.arg(newColor.name());



   bu->setStyleSheet(sheet);

   bu->update();

   auto& posi=mapToPosition.find(bu).value();
   colorList[posi.first]=newColor.name();

   emit changeLineColorSignal(posi.first,newColor);
   emit changeExSettingValue("DisplaySettings",posi.first,1,newColor.name());
}

void DisplaySettingDialog::updateChartState()
{

}



DisplaySettingDialog::~DisplaySettingDialog()
{
    delete ui;
}

void DisplaySettingDialog::on_pushButton_clicked()
{
    for(int i=0;i<4;i++)
    {
        QList<QString> data;
        bool s=qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(i,0))->checkState();
        if(s)
        {
            data<<"true";
        }
        else
        {
            data<<"false";
        }
        data<<colorList[i];
        data<<qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i,3))->currentText();
        data<<qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i,4))->currentText();
        emit changeExSettingValue("DisplaySettings",i,data);

    }

    this->close();
}

