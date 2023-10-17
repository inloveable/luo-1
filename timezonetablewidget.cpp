#include "timezonetablewidget.hpp"
#include "qlineseries.h"
#include "qnamespace.h"
#include "ui_timezonetablewidget.h"
#include"glog/logging.h"
TimeZoneTableWidget::TimeZoneTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeZoneTableWidget)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setRowCount(2);
    QStringList headLabel;
    headLabel<<"帧序号"<<"输入通道"<<"加速度峰值(g)"
         <<"加速度峰值误差"<<"脉冲宽度(ms)"
          <<"脉冲宽度误差(%)"<<"速度变化量(m/s)"
         <<"速度变化量误差(%)";
    ui->tableWidget->setHorizontalHeaderLabels(headLabel);

    ui->checkBox->setCheckState(Qt::Unchecked);
    emit showStandardChart(false);


}

TimeZoneTableWidget::~TimeZoneTableWidget()
{
    delete ui;
}

void TimeZoneTableWidget::setAdpotedSeries(const QList<QLineSeries*>& s)
{
   if(s.size()!=3)
   {
       LOG(FATAL)<<"Error size not in 3(setAdoptedSeries)";
       exit(1);
   }
   adoptedSeries[0]=s[0];
   adoptedSeries[1]=s[1];
   adoptedSeries[2]=s[2];
}
void TimeZoneTableWidget::on_checkBox_stateChanged(int arg1)
{
    auto checked=ui->checkBox->checkState();
    if(checked==0)
    {
       emit showStandardChart(false);
    }
    else if(checked==2)
    {
        emit showStandardChart(true);
    }
}


void TimeZoneTableWidget::on_pushButton_2_clicked()
{
    //move standard chart left
    auto range=requestTimeZoneXChartRange();

    auto moveDistance=range/100;

    for(int i=0;i<3;++i)
    {
        seriesToDirection(adoptedSeries[i],OPType::LEFT,moveDistance);
    }
    emit requestChartUpdate(0);
}

void TimeZoneTableWidget::seriesToDirection(QLineSeries* series,OPType type,double distance)
{
    if(type==OPType::LEFT)
    {
        auto points=series->points();
        for(auto&& p:points)
        {
            p.setX(p.x()-distance);
        }
        series->replace(points);
    }
    else if(type==OPType::RIGHT)
    {
        auto points=series->points();
        for(auto&& p:points)
        {
            p.setX(p.x()+distance);
        }
        series->replace(points);
    }
    else if(type==OPType::UP)
    {
        auto points=series->points();
        for(auto&& p:points)
        {
            p.setY(p.y()+distance);
        }
        series->replace(points);
    }
    else if(type==OPType::DOWN)
    {
        auto points=series->points();
        for(auto&& p:points)
        {
            p.setY(p.y()-distance);
        }
        series->replace(points);
    }
    else if(type==OPType::DEOPPOSITE||type==OPType::OPPOSITE)
    {
        auto points=series->points();
        for(auto&& p:points)
        {
            p.setY(-p.y());
        }
        series->replace(points);
    }
}


void TimeZoneTableWidget::on_pushButton_3_clicked()
{
    auto range=requestTimeZoneXChartRange();

    auto moveDistance=range/100;

    for(int i=0;i<3;++i)
    {
        seriesToDirection(adoptedSeries[i],OPType::RIGHT,moveDistance);
    }
    emit requestChartUpdate(0);
}


void TimeZoneTableWidget::on_pushButton_clicked()
{
    auto range=requestTimeZoneYChartRange();

    auto moveDistance=range/50;

    for(int i=0;i<3;++i)
    {
        seriesToDirection(adoptedSeries[i],OPType::UP,moveDistance);
    }
    emit requestChartUpdate(0);
}


void TimeZoneTableWidget::on_pushButton_4_clicked()
{
    auto range=requestTimeZoneYChartRange();

    auto moveDistance=range/50;

    for(int i=0;i<3;++i)
    {
        seriesToDirection(adoptedSeries[i],OPType::DOWN,moveDistance);
    }
    emit requestChartUpdate(0);
}





void TimeZoneTableWidget::on_pushButton_5_clicked()
{
    static bool clicked=false;
    if(!clicked)
    {
        for(int i=0;i<3;++i)
        {
            seriesToDirection(adoptedSeries[i],OPType::OPPOSITE,0);
        }
        clicked=true;
    }
    else
    {
        for(int i=0;i<3;++i)
        {
            seriesToDirection(adoptedSeries[i],OPType::DEOPPOSITE,0);
        }
        clicked=false;
    }
}

