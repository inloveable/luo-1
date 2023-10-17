#include "projectunitdialog.h"

#include "datamanager.h"
#include "qcombobox.h"
#include "ui_projectunitdialog.h"
#include<QComboBox>
#include<QTableWidgetItem>
#include<QMutex>
projectUnitDialog::projectUnitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::projectUnitDialog)
{
    ui->setupUi(this);

    setWindowTitle("工程单位");

    ui->tableWidget->setRowCount(7);
    ui->tableWidget->setColumnCount(4);
    QStringList labels{"项目","标准单位","显示单位","转换系数"};
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    QTableWidgetItem* item;
    QTableWidgetItem* item1;
    QList<QList<QString>> list{7};
   

    list[0] = (QList<QString>{QString("位移"), QString("m")});
    list[1] = (QList<QString>{QString("速度"), QString("m/s")});
    list[2] = (QList<QString>{QString("加速度"), QString("m/s^2")});
    list[3] = (QList<QString>{QString("力"), QString("N")});
    list[4] = (QList<QString>{QString("质量"), QString("kg")});
    list[5] = (QList<QString>{QString("量级"), QString("ratio")});
    list[6] = (QList<QString>{QString("电压"),QString("V")});


    for(int i=0;i<list.size();++i)
    {
        item=new QTableWidgetItem{list[i][0]};
        item1=new QTableWidgetItem{list[i][1]};
        ui->tableWidget->setItem(i,0,item);
        ui->tableWidget->setItem(i,1,item1);
    }

    comboboxCellConfiguration();


    auto& unitTable=DataManager::GetInstance()->getChannelInfo("projectUnit");

    for(int i=0;i<unitTable.size();++i)
    {
        for(int j=0;j<unitTable[i].size()-1; ++j)
        {
            if(j+1==1)
            {//combobox column
                qobject_cast<QComboBox*>
                        (ui->tableWidget->cellWidget(i,j+2))
                        ->setCurrentText(unitTable[i][j+1]);
            }
            item=new QTableWidgetItem{unitTable[i][j+1]};
            ui->tableWidget->setItem(i,j+2,item);
        }
    }

    for(int i=0;i<7;++i)
    {
        for(int j=0;j<4;++j)
        {
            if(j==2)
            {
                continue;
            }
            else
            {
                ui->tableWidget->item(i,j)->
                        setFlags(ui->tableWidget->item(i,j)->flags()
                                                       & (~Qt::ItemIsEditable));
            }
        }
    }

    for(auto b=mapFromCombobox.begin();b!=mapFromCombobox.end();++b)
    {
        connect(b.key(),&QComboBox::currentTextChanged,this,
                &projectUnitDialog::onComboBoxIndexChanged);
    }

}

void projectUnitDialog::comboboxCellConfiguration()
{
    QComboBox* boxer=new QComboBox;
    QStringList items;
    items<<"m"<<"cm"<<"mm"<<"Ft"<<"ln";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(0,2,boxer);
    mapToFactor["m"]="1";
    mapToFactor["cm"]="0.01";
    mapToFactor["mm"]="0.001";
    mapToFactor["Ft"]="0.3048";
    mapToFactor["ln"]="0.0254";
    mapFromCombobox.insert(boxer,0);


    boxer=new QComboBox;
    items.clear();
    items<<"m/s"<<"cm/s"<<"mm/s"<<"Ft/s"<<"ln/s";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(1,2,boxer);
    mapToFactor["m/s"]="1";
    mapToFactor["cm/s"]="0.01";
    mapToFactor["mm/s"]="0.001";
    mapToFactor["Ft/s"]="0.3048";
    mapToFactor["ln/s"]="0.0254";
    mapFromCombobox.insert(boxer,1);


    boxer=new QComboBox;
    items.clear();
    items<<"g"<<"m/s^2"<<"cm/s^2"<<"mm/s^2"<<"Ft/s^2"<<"ln/s^2";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(2,2,boxer);
    mapToFactor["g"]="9.8";
    mapToFactor["m/s^2"]="1";
    mapToFactor["cm/s^2"]="0.01";
    mapToFactor["mm/s^2"]="0.001";
    mapToFactor["Ft/s^2"]="0.3048";
    mapToFactor["ln/s^2"]="0.0254";
    mapFromCombobox.insert(boxer,2);

    boxer=new QComboBox;
    items.clear();
    items<<"N"<<"LBF";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(3,2,boxer);
    mapToFactor["N"]="1";
    mapToFactor["LBF"]="4.44822";
    mapFromCombobox.insert(boxer,3);



    boxer=new QComboBox;
    items.clear();
    items<<"kg"<<"Gram"<<"lbs"<<"Ounce"<<"Ton";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(4,2,boxer);
    mapToFactor["kg"]="1";
    mapToFactor["lbs"]="0.4536";
    mapToFactor["Gram"]="0.001";
    mapToFactor["Ounce"]="0.02835";
    mapToFactor["Ton"]="1000";
    mapFromCombobox.insert(boxer,4);



    boxer=new QComboBox;
    items.clear();
    items<<"%"<<"ratio"<<"db";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(5,2,boxer);
    mapToFactor["%"]="...";
    mapToFactor["ratio"]="...";
    mapToFactor["db"]="...";
    mapFromCombobox.insert(boxer,5);



    boxer=new QComboBox;
    items.clear();
    items<<"V"<<"mV";
    boxer->addItems(items);
    ui->tableWidget->setCellWidget(6,2,boxer);
    mapToFactor["V"]="1";
    mapToFactor["mV"]="0.001";
    mapFromCombobox.insert(boxer,6);


}
void projectUnitDialog::onComboBoxIndexChanged(const QString& text)
{
    QComboBox* box=qobject_cast<QComboBox*>(sender());
    int row=mapFromCombobox.find(box).value();
    ui->tableWidget->item(row,3)->setText(mapToFactor[text]);

}


projectUnitDialog::~projectUnitDialog()
{
    delete ui;
}

void projectUnitDialog::on_pushButton_clicked()
{
    for(int i=0;i<7;i++)
    {
        emit changeChannelValue("projectUnit",i,
                                qobject_cast<QComboBox*>
                                        (ui->tableWidget->cellWidget(i,2))->currentText(),
                                ui->tableWidget->item(i,3)->text());
        qDebug()<<"signal emitter:"<<"projectUnit"<<" "<<i<<qobject_cast<QComboBox*>
                  (ui->tableWidget->cellWidget(i,2))->currentText()<<
                  ui->tableWidget->item(i,3)->text();
    }
    close();
}


void projectUnitDialog::on_pushButton_2_clicked()
{
    close();
}

