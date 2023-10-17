#include "exargsettingdialog.h"
#include "qlineseries.h"
#include "qnamespace.h"
#include "ui_exargsettingdialog.h"
#include<QFileDialog>
#include"datamanager.h"
#include<glog/logging.h>
ExArgSettingDialog::ExArgSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExArgSettingDialog)
{
    ui->setupUi(this);

    ui->comboBox_2->setCurrentIndex(2);
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->comboBox_2,&QComboBox::currentIndexChanged,
            this,&ExArgSettingDialog::onComboBox_2IndexChanged);



    adopedChildren<<ui->doubleSpinBox<<ui->doubleSpinBox_2
                 <<ui->doubleSpinBox_3<<ui->doubleSpinBox_4<<ui->doubleSpinBox_5
                <<ui->spinBox_10<<ui->spinBox_9<<ui->comboBox<<ui->checkBox_2<<ui->checkBox_3;

    for(auto&& i:adopedChildren)
    {
        i->setEnabled(false);
    }

    inputParas.resize(5);
    for(int i=0;i<inputParas.size();++i)
    {
        inputParas[i].resize(3);
        inputParas[i][0]=QString("%1").arg(10.00);
        inputParas[i][1]=QString("%1").arg(-12.00);
        inputParas[i][2]=QString("%1").arg(-2304);
    }

    ui->comboBox_2->setCurrentIndex(2);



    initUi();

}

void ExArgSettingDialog::initUi()
{

    setWindowTitle("参数设置");
    auto& table=DataManager::GetInstance()->getExSettingInfo("FramePointCount");
    ui->comboBox_3->setCurrentText(table[0]);
    table=DataManager::GetInstance()->getExSettingInfo("SampleFrequency");
    ui->comboBox_4->setCurrentText(table[0]);
    table=DataManager::GetInstance()->getExSettingInfo("ShockTime");
    ui->doubleSpinBox_8->setValue(table[0].toDouble());
    table=DataManager::GetInstance()->getExSettingInfo("AllWaveWidth");
    ui->doubleSpinBox_10->setValue(table[0].toDouble());
    table=DataManager::GetInstance()->getExSettingInfo("MainWaveWidth");
    ui->spinBox_4->setValue(table[0].toInt());
    table=DataManager::GetInstance()->getExSettingInfo("PeekSearchRangeMin");
    ui->spinBox_5->setValue(table[0].toInt());
    table=DataManager::GetInstance()->getExSettingInfo("PeekSearchRangeMax");
    ui->spinBox_6->setValue(table[0].toInt());
    table=DataManager::GetInstance()->getExSettingInfo("TimeTrigger");
    ui->doubleSpinBox_9->setValue(table[0].toDouble());

    table=DataManager::GetInstance()->getExSettingInfo("pulseType");
    ui->comboBox_5->setCurrentText(table[0]);
    table=DataManager::GetInstance()->getExSettingInfo("standardType");
    ui->comboBox_6->setCurrentText(table[0]);
    table=DataManager::GetInstance()->getExSettingInfo("shockMachineType");
    ui->comboBox_7->setCurrentText(table[0]);
    table=DataManager::GetInstance()->getExSettingInfo("upLimit");
    ui->doubleSpinBox_11->setValue(table[0].toDouble());
    table=DataManager::GetInstance()->getExSettingInfo("lowLimit");
    ui->doubleSpinBox_12->setValue(table[0].toDouble());
    table=DataManager::GetInstance()->getExSettingInfo("acceleratePeek");
    ui->doubleSpinBox_13->setValue(table[0].toDouble());
    table=DataManager::GetInstance()->getExSettingInfo("pulseWidth");
    ui->doubleSpinBox_14->setValue(table[0].toDouble());



    this->upLimit=new QLineSeries;
    this->standardSeries=new QLineSeries;
    this->downLimit=new QLineSeries;

    QVector<QPointF> points{QPointF{0,0},QPointF{85.78,0},QPointF{100,3000},QPointF{112.44,0},QPointF{200,0}};
    standardSeries->replace(points);
    peekPointIndex=2;
    pulseBeginIndex=1;
    pulseEndIndex=3;

    QVector<QPointF> points1{QPointF{0,-600},QPointF{85.78,-600},QPointF{100,2400},QPointF{112.44,-600},QPointF{200,-600}};
    downLimit->replace(points1);
    //非极值点间与standard的差距应当以峰值点差距为准

    QVector<QPointF> points2{QPointF{0,600},QPointF{85.78,600},QPointF{100,3600},QPointF{112.44,600},QPointF{200,600}};
    upLimit->replace(points2);
    ui->doubleSpinBox_13->setValue(3000);

    //offsetSeries(upLimit,20,standardSeries);
    //offsetSeries(downLimit,-20,standardSeries);

    auto pen=standardSeries->pen();
    pen.setColor(Qt::green);
    standardSeries->setPen(pen);

    pen.setColor(Qt::red);
    downLimit->setPen(pen);
    upLimit->setPen(pen);

    chart = new QChart;
    chart->addSeries(upLimit);
    chart->addSeries(standardSeries);
    chart->addSeries(downLimit);

    

    chart->createDefaultAxes();
    ui->graphicsView->setChart(chart);

    chart->setTheme(QChart::ChartThemeDark);


    on_pushButton_2_clicked();

}

void ExArgSettingDialog::offsetSeries(QLineSeries* s,double rate,QLineSeries* origin)
{
    auto points=origin->points();
    double offset=points[peekPointIndex].y()*(rate/100);
    for(auto&& i:points)
    {
        i.setY(i.y()+offset);
    }
    s->replace(points);
}

void ExArgSettingDialog::onComboBox_2IndexChanged(int index)
{
    if(index==0)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if(index==1)
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
    else
    {
        inputParaActivated=index-2;
        ui->doubleSpinBox_6->setValue(inputParas[inputParaActivated][0].toDouble());
        ui->doubleSpinBox_7->setValue(inputParas[inputParaActivated][1].toDouble());
        ui->spinBox_11->setValue(inputParas[inputParaActivated][2].toDouble());
        ui->stackedWidget->setCurrentIndex(0);
    }
}

ExArgSettingDialog::~ExArgSettingDialog()
{
    delete ui;
}

void ExArgSettingDialog::on_pushButton_3_clicked()
{
    static bool clicked=false;

    if(!clicked)
    {
        for(auto&& i:adopedChildren)
        {
            i->setEnabled(true);
        }
        ui->pushButton_3->setText("关闭参数设置");
        clicked=true;
    }
    else
    {
        for(auto&& i:adopedChildren)
        {
            i->setEnabled(false);
        }
        ui->pushButton_3->setText("开启参数设置");
        clicked=false;
    }
}

void ExArgSettingDialog::importExArg()
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("导入配置文件"),
                                                  DataManager::GetInstance()->appCachePath,
                                                  tr("通道参数配置文件 (*.exConfig)"));
    if(fileName=="")
    {
        return;
    }
    qDebug()<<fileName;
    qDebug()<<DataManager::GetInstance()->setExSettingFile(fileName);
    emit rebuild();
    this->close();
}


void ExArgSettingDialog::on_pushButton_6_clicked()
{
    importExArg();
}


void ExArgSettingDialog::on_pushButton_7_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存配置文件"),
                               DataManager::GetInstance()->appCachePath,
                               tr("通道参数配置文件 (*.ExConfig)"));
    if(fileName=="")
    {
        return;
    }
    LOG(INFO)<<"argConfig save path:"<<fileName.toStdString();
    updateArgData();
    DataManager::GetInstance()->saveExSettingFile(fileName);
}

void ExArgSettingDialog::updateArgData()
{
     QVector<QString> argList;
     argList<<ui->comboBox_3->currentText();
     argList<<ui->comboBox_4->currentText();
     argList<<ui->doubleSpinBox_8->cleanText();
     argList<<ui->doubleSpinBox_10->cleanText();
     argList<<ui->spinBox_4->cleanText();
     argList<<ui->spinBox_5->cleanText();
     argList<<ui->spinBox_6->cleanText();
     argList<<ui->doubleSpinBox_9->cleanText();

     for(int i=0;i<5;i++)
     {
         for(int j=0;j<3;j++)
         {
             argList<<inputParas[i][j];
         }
     }

     argList<<ui->comboBox_5->currentText();
     argList<<ui->comboBox_6->currentText();
     argList<<ui->comboBox_7->currentText();
     argList<<QString::number(ui->doubleSpinBox_11->value());
     argList<<QString::number(ui->doubleSpinBox_12->value());
     argList<<QString::number(ui->doubleSpinBox_13->value());
     argList<<QString::number(ui->doubleSpinBox_14->value());

     DataManager::GetInstance()->updateExArgAtOnce(argList);


}




void ExArgSettingDialog::on_doubleSpinBox_6_valueChanged(double arg1)
{
    inputParas[inputParaActivated][0]=QString("%1").arg(arg1);
}


void ExArgSettingDialog::on_doubleSpinBox_7_valueChanged(double arg1)
{
    inputParas[inputParaActivated][1]=QString("%1").arg(arg1);
}


void ExArgSettingDialog::on_spinBox_11_valueChanged(int arg1)
{
    inputParas[inputParaActivated][2]=QString("%1").arg(arg1);
}


void ExArgSettingDialog::on_pushButton_5_clicked()
{
    QList<QString> args;
    args<<ui->comboBox_5->currentText();
    args<<ui->comboBox_6->currentText();
    args<<ui->comboBox_7->currentText();
    args<<QString::number(ui->doubleSpinBox_11->value());
    args<<QString::number(ui->doubleSpinBox_12->value());
    args<<QString::number(ui->doubleSpinBox_13->value());
    args<<QString::number(ui->doubleSpinBox_14->value());



    auto points=standardSeries->points();
    auto&& peekPoint=points[peekPointIndex];
    peekPoint.setY(ui->doubleSpinBox_13->value());

    points[pulseBeginIndex].setX(peekPoint.x()-(ui->doubleSpinBox_14->value()/2));
    points[pulseEndIndex].setX(peekPoint.x()+(ui->doubleSpinBox_14->value()/2));

    standardSeries->replace(points);

    offsetSeries(upLimit,ui->doubleSpinBox_11->value(),standardSeries);
    offsetSeries(downLimit,-(ui->doubleSpinBox_12->value()),standardSeries);

    chart->update();

    DataManager::GetInstance()->changeExArgValueEx("ShockReplayChartSettings",-1,args);


    sendStandardLine(0,QList<QLineSeries*>{standardSeries,upLimit,downLimit});


    LOG(INFO)<<"refreshed";


}

void ExArgSettingDialog::on_pushButton_2_clicked()// To update exsetting information
{
    //Read box's information
    QList<QString> args;//To save information
    args<<ui->comboBox_3->currentText();//FramePointCount
    args<<ui->comboBox_4->currentText();//SampleFrequency



    args<<QString::number(ui->doubleSpinBox_8->value());//ShockTime

    args<<QString::number(ui->doubleSpinBox_10->value());//AllWaveWidth
    args<<QString::number(ui->spinBox_4->value());//MainWaveWidth
    args<<QString::number(ui->spinBox_5->value());//PeekSearchRangeMin
    args<<QString::number(ui->spinBox_6->value());//PeekSearchRangeMax

    args<<QString::number(ui->doubleSpinBox_17->value());//TimeTrriger

    DataManager::GetInstance()->changeExArgValueEx("ExperimentSettings",1,args);
    args.clear();

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            args<<inputParas[i][j] ;
        }
    }

    DataManager::GetInstance()->changeExArgValueEx("ExperimentSettings",2,args);
    args.clear();
    //DataManager::GetInstance()->


    this->close();
}


void ExArgSettingDialog::on_pushButton_clicked()
{
    close();
}

