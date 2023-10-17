#pragma once

#include "qlogvalueaxis.h"
#include <QDialog>
#include<QMap>
#include <array>

namespace Ui {
class ExSettingReplyDialog;
}
class QTableWidgetItem;
class QLineSeries;
class ExSettingReplyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExSettingReplyDialog(QWidget *parent = nullptr);
    ~ExSettingReplyDialog();

   void activate(){sendStandardLine(1,QList<QLineSeries*>{seriesMap[0],seriesMap[1],seriesMap[2]});}
signals:
   void sendStandardLine(int destChart,const QList<QLineSeries*>& copySrc);
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();


    void on_pushButton_5_clicked();

private:
    Ui::ExSettingReplyDialog *ui;

    QVector<QVector<QTableWidgetItem*>> itemMatrix;


    std::array<QVector<QPointF>,3> pointMap;
    std::array<QLineSeries*,3>  seriesMap;

    void updateChart();

    double getK(const QPointF& p1,const QPointF& p2);
    QPointF getValueFromK(const QPointF& p1,double k,double another,bool type);
    double getValueFromDb(double db,double v1);

    QLogValueAxis* xAxis;
    QLogValueAxis* yAxis;

   void  initUi();







};

