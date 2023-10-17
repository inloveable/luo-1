#pragma once

#include <QObject>
#include"mainwindow.h"

class EnhancedChartViewSystem;

class QLineSeries;
class UIManager:public QObject
{
    Q_OBJECT
public:
    UIManager(MainWindow* adopted,QObject* parent=nullptr);

    friend class MainWindow;

    void acceptLine_one(int destChart,const QList<QLineSeries*>& copySrc);
    void acceptLine_Two(int destChart,const QList<QLineSeries*>& copySrc);
    MainWindow* mainwindow;


    void onChartChanged(int index);
    void onRequestChartUpdate(int index);



    //shockReplyMap
    void onFollowPointBtnClicked_ShockReply();
    void moveLeft_ShockReply();
    void moveRight_ShockReply();
    void moveUp_ShockReply();
    void moveDown_ShockReply();
    void zoomIn_ShockReply();
    void zoomReset_ShockReply();
    void mouseSetting_ShockReply();
    void mouseSetting_Reconstruct_ShockReply();
    void clearCallOut_ShockReply();
    void displaySetting_ShockReply();

    //experiment documents
    void on_createExDocument();
    void on_openExDocument();
    void on_saveExDocument();


    //timezoneMap
    void showStandardChart(bool r);


    void changeProjectUnit(const QString& type,const QString& newer);
    void changeBackEndUnit(const QString& type,const QString& newer);


    void onDataReady(int rec,int channel);
    void onRTDataReady(int ch,std::shared_ptr<QVector<QPointF>>& vec);
    void onSampledRawDataSent(std::shared_ptr<QVector<QPointF>>s,int ch);

    signals:
    void sendTimeZoneFile(const QString& path);
    void sendAccelerateFile(const QString& path);

    void requestWriteDataToSampledFile(QWidget* mainwindow,std::shared_ptr<QVector<QPointF>> s);

private:

    std::array<EnhancedChartViewSystem*,4> mainCharts;
    std::array<QLineSeries*,3>             standardLines;
    std::array<QLineSeries*,3>             standardLines_two;


    void showAccerlrateDialog();
    void showTimeZoneDialog();


    void onExportSampledData();
    void onExportProcessedData();

   //timezone


};

