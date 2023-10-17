#include "uimanager.hpp"

#include "displaysettingdialog.h"
#include "exargsettingdialog.h"
#include "experimentdocument.hpp"
#include "exsettingreplydialog.hpp"
#include "mousesettingdialog.h"

#include<QRandomGenerator>

#include "qmessagebox.h"
#include "sysbackend.hpp"
#include"ui_mainwindow.h"
#include"enhancedchartviewsystem.h"
#include"datamanager.h"
#include<glog/logging.h>

#include<QAction>
#include<QMenu>
#include<QFileDialog>
#include<QInputDialog>
#include<QMessageBox>
#include<QLogValueAxis>
#include <memory>

#define DEBUG_SYMBOL 0

UIManager::UIManager(MainWindow* adopted,QObject* parent)
    :QObject{parent},mainwindow{adopted}
{
     auto ui=mainwindow->ui;

     mainCharts[0]=ui->graphicsView;
     mainCharts[1]=ui->graphicsView_2;
     mainCharts[2]=nullptr;//current version incompeleted ui
     mainCharts[3]=nullptr;

     mainCharts[0]->setOpenGL(true);
     mainCharts[1]->setOpenGL(true);
     QList<QString> timeZoneArgs;
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("pulseType");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("standardType");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("shockMachineType");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("upLimit");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("lowLimit");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("acceleratePeek");
     timeZoneArgs<<DataManager::GetInstance()->getExSettingInfo("pulseWidth");
     mainCharts[0]->initType_timeZone(timeZoneArgs);

     QAction* readTimeZoneAction=        new QAction{"选择频域信号输入文件"};
     QAction* readAccelerateAction=      new QAction{"选择加速度信号输入文件"};
     QAction* openAccelerateDialogAction=new QAction{"设置加速度实验参数"};
     QAction* openShockReplyDialogAction=new QAction{"设置时域信号实验参数"};

     connect(this,&UIManager::sendAccelerateFile,
             DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::readAccelerateFromFile);
     connect(this,&UIManager::sendTimeZoneFile,
             DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::readTimeZoneFromFile);


     connect(openAccelerateDialogAction,&QAction::triggered,this,&UIManager::showAccerlrateDialog);
     connect(openShockReplyDialogAction,&QAction::triggered,this,&UIManager::showTimeZoneDialog);
     connect(readTimeZoneAction,&QAction::triggered,this,[=]()
     {
         auto str=QFileDialog::getOpenFileName(mainwindow,"选择文件","./","*.txt");
         if(str!="")
         {
             emit sendTimeZoneFile(str);
         }
     });
     connect(readAccelerateAction,&QAction::triggered,this,[=]()
     {
         auto str=QFileDialog::getOpenFileName(mainwindow,"选择文件","./","*.txt");
         if(str!="")
         {
             emit sendAccelerateFile(str);
         }
     });

     connect(DataManager::GetInstance()->getSysBackEnd(),&SYSBackEnd::dataProcessFinished,
             this,&UIManager::onDataReady);
     connect(DataManager::GetInstance(),&DataManager::sendRTData,
             this,&UIManager::onRTDataReady);
     connect(DataManager::GetInstance(),&DataManager::onSampledRawDataSent,
             this,&UIManager::onSampledRawDataSent);


     QMenu* testMenu=new QMenu{"测试"};
     testMenu->addActions(QList<QAction*>{
                              readAccelerateAction,
                              readTimeZoneAction
                          });


     QMenu* gerneralMenu=new QMenu{"通用"};
     QMenu* settingMenu=new QMenu{"设置"};

     settingMenu->addAction(openAccelerateDialogAction);
     settingMenu->addAction(openShockReplyDialogAction);

     gerneralMenu->addMenu(testMenu);

     QAction* action=new QAction{"重新计算SRS"};
     gerneralMenu->addAction(action);
     connect(action,&QAction::triggered,this,[=]()
     {
         QMetaObject::invokeMethod(DataManager::GetInstance()->getSysBackEnd()
                                   ,"reCalculateSRSResult");
     });




     ui->menu->addMenu(gerneralMenu);
     ui->menu->addMenu(settingMenu);

     QMenu* choiceMenu=new QMenu{"选项"};

     QMenu* exportMenu=new QMenu{"导出"};
     choiceMenu->addMenu(exportMenu);

     QAction* action2=new QAction{"导出采样数据"};
     QAction* action3=new QAction{"导出时域数据"};

     //if you need more advanced interactions,unComment the following lines,
     //and just trigger(enable) them again as your need
     //action2->setEnabled(false);
     //action3->setEnabled(false);

     connect(action2,&QAction::triggered,this,&UIManager::onExportSampledData);
     connect(action3,&QAction::triggered,this,&UIManager::onExportProcessedData);

     exportMenu->addAction(action2);
     exportMenu->addAction(action3);

     ui->menu->addMenu(choiceMenu);



#if DEBUG_SYMBOL
     std::shared_ptr<QVector<QPointF>> s=std::make_shared<QVector<QPointF>>();
     for(int i=0;i<300;++i)
     {
         s->push_back(QPointF{static_cast<qreal>(i),
                              static_cast<qreal>(QRandomGenerator::global()->generate())});
     }

     onSampledRawDataSent(s,0);
#endif
}

void UIManager::onChartChanged(int index)
{
    mainwindow->ui->stackedWidget_2->setCurrentIndex(index);
}

void UIManager::changeProjectUnit(const QString& type,const QString& newer)
{
     auto ui=mainwindow->ui;
     //more if elsees would determine the unit of chart
     if(type=="加速度")
     {
         //change chart vertical unit
         for(int i=0;i<2;++i)
         {
             for(int j=0;j<4;++j)
             {
                 mainCharts[i]->transformDataSeries(j,DataManager::GetInstance()
                                                    ->unitMap[newer].toDouble());
             }
         }

         auto yAxis=mainCharts[0]->axisY;
         qobject_cast<QLogValueAxis*>(yAxis)->setLabelFormat(QString("%d %1").arg(newer));


     }
   /*else if(type=="???")
     {
         ...
     }
   */

     changeBackEndUnit(type,newer);

}

void UIManager::changeBackEndUnit(const QString& type,const QString& newer)
{
    qDebug()<<"changing backend unit";
}

void UIManager::onFollowPointBtnClicked_ShockReply()
{
    auto ui=mainwindow->ui;
    static bool isChecked=false;

    if(!isChecked)
    {
        auto button=QMessageBox::question(mainwindow,"警告","开启点值跟踪将关闭GPU加速(开启可提升图表渲染速度)");
        if(button!=QMessageBox::Button::Yes)
        {
            return;
        }
        ui->graphicsView_2->setOpenGL(false);
        ui->graphicsView_2->setIsShowingCallout(true);
        ui->commentBtn_2->setChecked(true);
        isChecked=true;
    }
    else
    {
       ui->graphicsView_2->setOpenGL(true);
       ui->graphicsView_2->setIsShowingCallout(false);
       ui->commentBtn_2->setChecked(false);
       isChecked=false;
    }
}

void UIManager:: zoomIn_ShockReply()
{
    auto ui=mainwindow->ui;
    static bool isChecked=false;
    if(!isChecked)
    {
        ui->graphicsView_2->setIsRubberBand(true);
        ui->zoomBtn_2->setChecked(true);
        isChecked=true;
    }
    else
    {
        ui->graphicsView_2->setIsRubberBand(false);
        ui->zoomBtn_2->setChecked(false);
        isChecked=false;
    }
}

void UIManager::displaySetting_ShockReply()
{
    auto ui=mainwindow->ui;
    DisplaySettingDialog* dialog=new DisplaySettingDialog(mainwindow);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&DisplaySettingDialog::changeLineWidthSignal,
            ui->graphicsView_2,&EnhancedChartViewSystem::setSeriesLineWidth);
    connect(dialog,&DisplaySettingDialog::changeLineColorSignal,ui->graphicsView_2,
            &EnhancedChartViewSystem::setLineColor);
    connect(dialog,&DisplaySettingDialog::checkBoxStateChanged,ui->graphicsView_2,
            &EnhancedChartViewSystem::setLineVisibility);
    connect(dialog,&DisplaySettingDialog::changeLinePattern,ui->graphicsView_2,
            &EnhancedChartViewSystem::setLineType);




    dialog->exec();
}

void UIManager::mouseSetting_ShockReply()
{
    auto ui=mainwindow->ui;
    mouseSettingDialog* dialog=new mouseSettingDialog(mainwindow);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&mouseSettingDialog::addLineCursor,
            ui->graphicsView_2,&EnhancedChartViewSystem::addLineCursor);
    dialog->exec();
}

void UIManager::mouseSetting_Reconstruct_ShockReply()
{

}

void UIManager::acceptLine_one(int destChart,const QList<QLineSeries*>& copySrc )
{
    static bool firstAllocated=false;

    auto chart=mainCharts[destChart];
    if(!firstAllocated)
    {
        //designed for shockReply chart
        standardLines[0]=new QLineSeries;
        standardLines[1]=new QLineSeries;
        standardLines[2]=new QLineSeries;
        for(int i=0;i<3;++i)
        {
            chart->addSeries(i,standardLines[i]);
            standardLines[i]->attachAxis(chart->axisX);
            standardLines[i]->attachAxis(chart->axisY);


        }
        chart->setLineColor(0,Qt::red);
        chart->setLineColor(1,Qt::green);
        chart->setLineColor(2,Qt::green);

        firstAllocated=true;
    }
    if(destChart>4||destChart<0)
    {
        LOG(WARNING)<<"wrong index";
        return;
    }
    if(copySrc.size()!=3)
    {
        LOG(WARNING)<<"wrong series size";
    }

    for(int i=0;i<copySrc.size();++i)
    {
        standardLines[i]->replace(copySrc[i]->points());
    }
    chart->update();
}

void UIManager::acceptLine_Two(int destChart,const QList<QLineSeries*>& copySrc )
{
    static bool firstAllocated=false;
    if(destChart>4||destChart<0)
    {
        LOG(WARNING)<<"wrong index";
        return;
    }
    auto chart=mainCharts[destChart];
    if(!firstAllocated)
    {
        //designed for shockReply chart
        standardLines_two[0]=new QLineSeries;
        standardLines_two[1]=new QLineSeries;
        standardLines_two[2]=new QLineSeries;

         standardLines_two[0]->setName("理想频率");
         standardLines_two[1]->setName("上限频率");
         standardLines_two[2]->setName("下限频率");

        for(int i=0;i<3;++i)
        {
            chart->addSeries(i,standardLines_two[i]);
            standardLines_two[i]->attachAxis(chart->axisX);
            standardLines_two[i]->attachAxis(chart->axisY);


        }
        chart->setLineColor(0,Qt::red);
        chart->setLineColor(1,Qt::green);
        chart->setLineColor(2,Qt::green);

        firstAllocated=true;
    }

    if(copySrc.size()!=3)
    {
        LOG(WARNING)<<"wrong series size";
    }

    for(int i=0;i<copySrc.size();++i)
    {
        standardLines_two[i]->replace(copySrc[i]->points());
    }
    chart->update();
}

void UIManager::moveRight_ShockReply()
{
    mainwindow->ui->graphicsView_2->scollOverX(true);
}

void UIManager::moveLeft_ShockReply()
{
    mainwindow->ui->graphicsView_2->scollOverX(false);
}


void UIManager::moveDown_ShockReply()
{
    mainwindow->ui->graphicsView_2->scollOverY(false);
}


void UIManager::moveUp_ShockReply()
{
    mainwindow->ui->graphicsView_2->scollOverY(true);
}

void UIManager::zoomReset_ShockReply()
{
    mainwindow->ui->graphicsView_2->resetZoom();
}


void UIManager::clearCallOut_ShockReply()
{
    mainwindow->ui->graphicsView_2->clearCallout();
}

void UIManager::showStandardChart(bool r)
{
    //在图表初始化，通过传参的形式，然后可以通过改变参数改变这些东西
    //通过设置线可见性改变可见性
    for(int i=0;i<3;++i)
    {
        mainCharts[0]->setLineVisibility(i,r);
    }

}

void UIManager::onRequestChartUpdate(int index)
{
    mainCharts[index]->update();
}

void UIManager::onSampledRawDataSent(std::shared_ptr<QVector<QPointF>>s,int ch)
{
    std::pair<double,double> yRange{0,0};

    for(auto&& p:*s)
    {
       if(p.ry()>yRange.second)yRange.second=p.ry();
       if(p.ry()<yRange.first)yRange.first=p.ry();
       p.rx()*=20;
    }

    mainCharts[0]->setXRange((*s->begin()).rx(),(*(s->end()-1)).rx());
    mainCharts[0]->setYRange(yRange.first,yRange.second);

    (*s).shrink_to_fit();

    auto line=mainCharts[0]->getLine(ch+3);//first 3 lines are referrence lines;
    if(line==nullptr)
    {
        LOG(INFO)<<"asking for the channel that does not exist,error";
        return;
    }

    line->replace(*s);

    mainCharts[0]->update();

    QMetaObject::invokeMethod(DataManager::GetInstance(),"writeRawDataToDocument",
                            Q_ARG(QWidget*,mainwindow),Q_ARG(QVector<QPointF>,*s));

}

void UIManager::onRTDataReady(int ch,std::shared_ptr<QVector<QPointF>>& vec)
{

    qDebug()<<"accelerate point count:"<<vec->size();
    auto line=mainwindow->ui->graphicsView->getLine(3);
    line->replace(*vec);
    mainwindow->ui->graphicsView->update();
}
void UIManager::onDataReady(int rec,int channel)
{
    //rec  1 for accelerate ,rec 2 as timezone -1 as error
    //there might be multi-channels in the future,by default is first-0
    QVector<QPointF> points;
    if(rec==1)
    {

       QMetaObject::invokeMethod(DataManager::GetInstance()->getSysBackEnd(),"getAccelerateSequence",
                     Qt::BlockingQueuedConnection,Q_RETURN_ARG(QVector<QPointF>,points));
        qDebug()<<"accelerate point count:"<<points.size();
        auto line=mainwindow->ui->graphicsView_2->getLine(3);
        line->replace(points);
        mainwindow->ui->graphicsView_2->update();

        DataManager::GetInstance()->
                writeProcessedDataToDocument(mainwindow,
                                             points);

    }
    else if(rec==2)
    {
        QMetaObject::invokeMethod(DataManager::GetInstance()->getSysBackEnd(),"getTimeZoneSequence",
                      Qt::BlockingQueuedConnection,Q_RETURN_ARG(QVector<QPointF>,points));
        auto line=mainwindow->ui->graphicsView->getLine(3);
        line->replace(points);
        mainwindow->ui->graphicsView->update();
    }
    else
    {
        //error conduct
        LOG(WARNING)<<"error when onDataReady:"<<rec;
        return;
    }



}

void UIManager::showTimeZoneDialog()
{
   mainwindow->horizontalShockDialog->show();
}

void UIManager::showAccerlrateDialog()
{

      mainwindow->shockReplyDialog->show();
}

void UIManager::on_createExDocument()
{
   DataManager::GetInstance()->createExDocument(mainwindow);
}

void UIManager::on_openExDocument()
{   
    DataManager::GetInstance()->openExDocument(mainwindow);
}

void UIManager::on_saveExDocument()
{
     DataManager::GetInstance()->saveExDocument(mainwindow);
}

void UIManager::onExportSampledData()
{
    QString prefix=QFileDialog::getExistingDirectory(mainwindow);
    auto str=DataManager::GetInstance()->exportSampledData(prefix);
}

void UIManager::onExportProcessedData()
{
    QString prefix=QFileDialog::getExistingDirectory(mainwindow);
    auto str=DataManager::GetInstance()->exportProcessedData(prefix);
}
