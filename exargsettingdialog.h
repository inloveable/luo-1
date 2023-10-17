#ifndef EXARGSETTINGDIALOG_H
#define EXARGSETTINGDIALOG_H

#include "qlineseries.h"
#include <QDialog>
#include<QSplineSeries>
namespace Ui {
class ExArgSettingDialog;
}

class ExArgSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExArgSettingDialog(QWidget *parent = nullptr);
    ~ExArgSettingDialog();

    void onComboBox_2IndexChanged(int index);

    void initUi();
    void updateExArg();
    void importExArg();

    void activate(){sendStandardLine(0,QList<QLineSeries*>{standardSeries,upLimit,downLimit});}
signals:
    void rebuild();
    void                       changeExArgValueEx(const QString& table,int index,
                                                   const QList<QString>&);
    void sendStandardLine(int destChart,const QList<QLineSeries*>& copySrc);
private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_doubleSpinBox_6_valueChanged(double arg1);

    void on_doubleSpinBox_7_valueChanged(double arg1);

    void on_spinBox_11_valueChanged(int arg1);

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::ExArgSettingDialog *ui;

    QList<QWidget*> adopedChildren;

    int inputParaActivated=0;


    QVector<QVector<QString>> inputParas;

    void updateArgData();


    void offsetSeries(QLineSeries* s,double rate,QLineSeries* origin);

    QLineSeries* upLimit;
    QLineSeries* downLimit;
    QLineSeries* standardSeries;
    int peekPointIndex=0;
    int pulseBeginIndex=0;
    int pulseEndIndex=0;

    QChart* chart;
};

#endif // EXARGSETTINGDIALOG_H
