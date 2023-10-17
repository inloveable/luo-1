#pragma once


#include <QWidget>
#include <array>

namespace Ui {
class TimeZoneTableWidget;
}
class QLineSeries;
class TimeZoneTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeZoneTableWidget(QWidget *parent = nullptr);
    ~TimeZoneTableWidget();

    void setAdpotedSeries(const QList<QLineSeries*>& series);

    enum class OPType{LEFT,RIGHT,UP,DOWN,OPPOSITE,DEOPPOSITE};
signals:
    void showStandardChart(bool);
    void operateReferrenceSeries(OPType type);
    double requestTimeZoneXChartRange();
    double requestTimeZoneYChartRange();
    void requestChartUpdate(int index);

private slots:
    void on_checkBox_stateChanged(int arg1);
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

private:
    Ui::TimeZoneTableWidget *ui;
    std::array<QLineSeries*,3> adoptedSeries;
    void seriesToDirection(QLineSeries* src,OPType type,double distance);
};

