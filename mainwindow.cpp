

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include"channelargdialog.h"
#include "enhancedchartviewsystem.h"
#include "exargsettingdialog.h"
#include "exsettingreplydialog.hpp"
#include "firstwindow.h"
#include "mousesettingdialog.h"
#include  "networkmanager.h"
#include "datamanager.h"
#include "printdialog.hpp"
#include "shockreplaytable.hpp"
#include "timezonetablewidget.hpp"
#include "uimanager.hpp"
#include"displaysettingdialog.h"

#include<QThread>
#include<QAction>
#include<QSplitter>
#include "qmessagebox.h"
#include "qnamespace.h"
#include<QFileDialog>
#include <vector>






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);





    //network=new NetworkManager;
   // networkThread=new QThread;
   //network->moveToThread(networkThread);

    prewindow=new firstWindow(this);

    networkConfiguration();
    prewindowConfiguration();



    QMenu* menu=new QMenu{"采集"};

    //inrelative ui operations

    //QAction* ac=new QAction("开始采集");
    //connect(ac,&QAction::triggered,DataManager::GetInstance()->getSysBackEnd(),
        //    &SYSBackEnd::startAcqusition);
   // menu->addAction(ac);

    //ac=new QAction("停止采集");
   // connect(ac,&QAction::triggered,DataManager::GetInstance()->getSysBackEnd(),
   ///         &SYSBackEnd::stopAcqusition);
   // menu->addAction(ac);

    QAction* ac=new QAction("采集实时数据");
    connect(ac,&QAction::triggered,this,&MainWindow::startAcquisition);
    menu->addAction(ac);

    ac=new QAction("停止采集实时数据");
    connect(ac,&QAction::triggered,DataManager::GetInstance()->getSysBackEnd(),
            &SYSBackEnd::stopRTAcqusition);
    menu->addAction(ac);









    ui->menubar->addMenu(menu);
    ui->splitter->setSizes(QList<int>(1000,181));


    prewindow->setAttribute(Qt::WA_DeleteOnClose);
    prewindow->show();
    //networkThread->start();




}



void MainWindow::prewindowConfiguration()
{
  connect(prewindow,&firstWindow::activateMainwindow,
           this,&MainWindow::actualShow);
  connect(prewindow,&firstWindow::connectToDevice,
        DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::connectToHost);
  connect(DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::acqusitionStarted,
          prewindow,&firstWindow::isConnectedToDevice);
    //       network,&NetworkManager::connectToHost);

}

void MainWindow::networkConfiguration()
{

  //  connect(network,&NetworkManager::connectResult,
     //       prewindow,&firstWindow::isConnectedToDevice);
   //make sure that nerwork manager is never blocked;
}



void MainWindow::actualShow(MainWindow::ExType mark)
{
    uiConfiguration(mark);
    uiType=mark;
    this->show();
}

void MainWindow::uiConfiguration(MainWindow::ExType mark)
{
    ui->pushButton_4->setVisible(false);
    ui->pushButton_6->setVisible(false);

    uiManager=new UIManager(this,this);

    ui->graphicsView_2->initType_shockReply();

    horizontalShockDialog=new ExArgSettingDialog(this);
    shockReplyDialog=new ExSettingReplyDialog(this);

    connect(horizontalShockDialog,&ExArgSettingDialog::sendStandardLine,
           uiManager,&UIManager::acceptLine_one);
    connect(shockReplyDialog,&ExSettingReplyDialog::sendStandardLine,
            uiManager,&UIManager::acceptLine_Two);

    shockReplyDialog->activate();
    horizontalShockDialog->activate();

    connect(ui->widget_2,&TimeZoneTableWidget::requestTimeZoneXChartRange,
            this,&MainWindow::getTimeZoneChartRange);
    connect(ui->widget_2,&TimeZoneTableWidget::requestTimeZoneYChartRange,
            this,&MainWindow::getTimeZoneYChartRange);
    connect(ui->widget_2,&TimeZoneTableWidget::requestChartUpdate,uiManager,&UIManager::onRequestChartUpdate);



    ui->tabWidget->setTabVisible(2,false);
    ui->tabWidget->setTabVisible(3,false);
    ui->tabWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);

    ui->splitter_2->setSizes(QList<int>()<<1/3*ui->stackedWidget->width()<<
                             2/3*ui->stackedWidget->width());
    if(mark==ExType::HorizontalSHOCK)
    {

        setWindowTitle("水平冲击碰撞实验");

    }else if(mark==ExType::SHOCKReply)
    {
         setWindowTitle("冲击响应实验");


    }

    connect(ui->commentBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::onFollowPointBtnClicked_ShockReply);
    connect(ui->moveDownBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::moveDown_ShockReply);
    connect(ui->moveLeftBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::moveLeft_ShockReply);
    connect(ui->moveRightBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::moveRight_ShockReply);
    connect(ui->moveUpBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::moveUp_ShockReply);
    connect(ui->zoomBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::zoomIn_ShockReply);
    connect(ui->mouseSettingBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::mouseSetting_ShockReply);
    connect(ui->resetBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::zoomReset_ShockReply);
    connect(ui->clearPVBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::clearCallOut_ShockReply);
    connect(ui->displaySettingBtn_2,&QPushButton::clicked,uiManager,
            &UIManager::displaySetting_ShockReply);
    connect(ui->tabWidget,&QTabWidget::currentChanged,uiManager,&UIManager::onChartChanged);
    connect(ui->widget_2,&TimeZoneTableWidget::showStandardChart,uiManager,&UIManager::showStandardChart);
    connect(horizontalShockDialog,&ExArgSettingDialog::rebuild,this,
            &MainWindow::exArgDialogReconstruct);
    //make control pannel widget has the ownership of the standard series;
    connect(DataManager::GetInstance(),&DataManager::changeProjectUnit,
            uiManager,&UIManager::changeProjectUnit);
    ui->widget_2->setAdpotedSeries(QList<QLineSeries*>{ui->graphicsView->getLine(0),
                                                       ui->graphicsView->getLine(1),
                                                       ui->graphicsView->getLine(2)});
    connect(ui->pushButton,&QPushButton::clicked,uiManager,&UIManager::on_createExDocument);
    connect(ui->pushButton_2,&QPushButton::clicked,uiManager,&UIManager::on_openExDocument);
    connect(ui->pushButton_3,&QPushButton::clicked,uiManager,&UIManager::on_saveExDocument);

    uiManager->showStandardChart(false);

    connect(DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::sendStateBar,
            ui->widget,&shockReplayTable::setSrsBar);
}


void MainWindow::updatePeekVal(double x,double y)
{
    if(YPeekVal==nullptr)
    {
        YPeekVal=new QPair<double,double>;
    }
    YPeekVal->first=x;
    YPeekVal->second=y;
}

double MainWindow::getTimeZoneChartRange()
{
    auto timeZoneData=ui->graphicsView->getLine(3);
  
    auto points=timeZoneData->points();
    if (points.size()==0)
    {
        return 10;
    }
    return points[points.size()-1].x()-points[0].x();
}
double MainWindow::getTimeZoneYChartRange()
{
    auto timeZoneData=ui->graphicsView->getLine(3);
    auto points = timeZoneData->points();
    if (points.size() == 0)
    {
        return 100;
    }
    if(YPeekVal==nullptr)
    {
        YPeekVal=new QPair<double,double>;
        

        for(int i=0;i<points.size();++i)
        {
            if(points[i].y()<YPeekVal->first)
            YPeekVal->first=points[i].y();
            if(points[i].y()>YPeekVal->second)
            YPeekVal->second=points[i].y();
        }
    }


    return YPeekVal->second-YPeekVal->first;
}







void MainWindow::startAcquisition()
{
    DataManager::GetInstance()->getSysBackEnd()->startRTAcqusition();
    qDebug()<<"startAqusition:";
}

MainWindow::~MainWindow()
{

    //networkThread->quit();


    DataManager::GetInstance()->Destory();
    //qDebug()<<"network thread on quit:"<<networkThread->wait();

    //delete network;
    if(YPeekVal!=nullptr)
    {
        delete YPeekVal;
    }
    //delete networkThread;
    delete uiManager;
    delete ui;




}

//begin first chart setting

void MainWindow::on_channelArgBtn_clicked()
{
    ChannelArgDialog* dialog=new ChannelArgDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&ChannelArgDialog::changeChannelValue,
            DataManager::GetInstance(),&DataManager::changeChannelValue);
    connect(dialog,&ChannelArgDialog::changeChannelValueEx,
            DataManager::GetInstance(),&DataManager::changeChannelValueEx);
    connect(dialog,&ChannelArgDialog::rebuild,this,&MainWindow::channelArgDialogReconstruct
    );
    dialog->show();
}

void MainWindow::channelArgDialogReconstruct()
{
    ChannelArgDialog* Another=new ChannelArgDialog(this);
    Another->setAttribute(Qt::WA_DeleteOnClose);
    connect(Another,&ChannelArgDialog::changeChannelValue,
            DataManager::GetInstance(),&DataManager::changeChannelValue);
    connect(Another,&ChannelArgDialog::changeChannelValueEx,
            DataManager::GetInstance(),&DataManager::changeChannelValueEx);
    connect(Another,&ChannelArgDialog::rebuild,this,&MainWindow::channelArgDialogReconstruct
    );
    Another->show();
}





void MainWindow::on_moveRightBtn_clicked()
{
    ui->graphicsView->scollOverX(true);
}

void MainWindow::on_moveLeftBtn_clicked()
{
    ui->graphicsView->scollOverX(false);
}


void MainWindow::on_moveUpBtn_clicked()
{
    ui->graphicsView->scollOverY(false);
}


void MainWindow::on_moveDownBtn_clicked()
{
    ui->graphicsView->scollOverY(true);
}


void MainWindow::on_clearPVBtn_clicked()
{
    ui->graphicsView->clearCallout();
}

void MainWindow::on_resetBtn_clicked()
{
    ui->graphicsView->resetZoom();
}


void MainWindow::on_zoomBtn_clicked()
{
    static bool isChecked=false;
    if(!isChecked)
    {
        ui->graphicsView->setIsRubberBand(true);
        ui->zoomBtn->setChecked(true);
        isChecked=true;
    }
    else
    {
        ui->graphicsView->setIsRubberBand(false);
        ui->zoomBtn->setChecked(false);
        isChecked=false;
    }
}

void MainWindow::on_displaySettingBtn_clicked()
{
    DisplaySettingDialog* dialog=new DisplaySettingDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&DisplaySettingDialog::changeLineWidthSignal,
            ui->graphicsView,&EnhancedChartViewSystem::setSeriesLineWidth);
    connect(dialog,&DisplaySettingDialog::changeLineColorSignal,ui->graphicsView,
            &EnhancedChartViewSystem::setLineColor);
    connect(dialog,&DisplaySettingDialog::checkBoxStateChanged,ui->graphicsView,
            &EnhancedChartViewSystem::setLineVisibility);
    connect(dialog,&DisplaySettingDialog::changeLinePattern,ui->graphicsView,
            &EnhancedChartViewSystem::setLineType);


    dialog->updateChartState();

    dialog->exec();
}

void MainWindow::on_commentBtn_clicked()
{


    static bool isChecked=false;

    if(!isChecked)
    {
        auto button=QMessageBox::question(this,"警告","开启点值跟踪将关闭GPU加速(开启可提升图表渲染速度)");
        if(button!=QMessageBox::Button::Yes)
        {
            return;
        }
        ui->graphicsView->setOpenGL(false);
        ui->graphicsView->setIsShowingCallout(true);
        ui->commentBtn->setChecked(true);
        isChecked=true;
    }
    else
    {
       ui->graphicsView->setOpenGL(true);
       ui->graphicsView->setIsShowingCallout(false);
       ui->commentBtn->setChecked(false);
       isChecked=false;
    }
}

void MainWindow::on_mouseSettingBtn_clicked()
{
    mouseSettingDialog* dialog=new mouseSettingDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&mouseSettingDialog::addLineCursor,
            ui->graphicsView,&EnhancedChartViewSystem::addLineCursor);
    dialog->exec();
}

//end first chart setting

















void MainWindow::on_pushButton_9_clicked()
{
    if(uiType==ExType::HorizontalSHOCK)
    {
        horizontalShockDialog->show();
    }
    else
    {

        shockReplyDialog->show();
    }
}

void MainWindow::exArgDialogReconstruct()
{

    ExArgSettingDialog* dialog=new ExArgSettingDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog,&ExArgSettingDialog::rebuild,this,
            &MainWindow::exArgDialogReconstruct);
    dialog->show();

    horizontalShockDialog->close();
    horizontalShockDialog=dialog;
}




//reply chart



void MainWindow::on_pushButton_5_clicked()
{

  PrintDialog* dialog=new PrintDialog(this);
  dialog->mainwindow=this;
  dialog->show();
}


void MainWindow::on_zoomBtn_2_clicked()
{

}


void MainWindow::on_commentBtn_2_clicked()
{

}


void MainWindow::on_moveLeftBtn_2_clicked()
{

}

