#ifndef ENHANCEDCHARTVIEWSYSTEM_H
#define ENHANCEDCHARTVIEWSYSTEM_H

#include "qabstractaxis.h"
#include "qlineseries.h"

#include <QGraphicsView>
#include <QObject>
#include<QLocale>
class QChart;
class Callout;
class QRubberBand;
class LineItem;
class EnhancedChartViewSystem : public QGraphicsView
{
    Q_OBJECT
public:
    EnhancedChartViewSystem(QWidget* parent=nullptr);

    void setIsRubberBand(bool f){isRubberBand=f;};
    void setIsShowingCallout(bool f){setShowCallout(f);};
    void scollOverX(bool direction);//false for left;true for right;
    void scollOverY(bool direction);
    void resetZoom();

    enum class PEEK{MAX,MIN,BOTH};
    void showPeekValue(int index,PEEK type=PEEK::BOTH);
    void addSeries(int index,QLineSeries* s);
    void setMaxPeekCount(int i){maxPeek=i;};
    void setMinPeekCount(int i){minPeek=i;};
    void setBothPeekCount(int i){bothPeek=i;};
    void setPeekPointSize(int s);
    void setPeekPointColor(const QColor& c){(*conf)[QXYSeries::PointConfiguration::Color]=c;}
    void transformDataSeries(int index,double rate);
    void setOpenGL(bool s);
    void setShowCallout(bool s);
    void setSeriesLineWidth(int index,int width);
    void setLineColor(int index,QColor c);
    void setLineVisibility(int index,bool s);
    void setLineType(int index,const QString& type);
    void setChartTitle(const QString& title);
    void setXRange(double min,double max);
    void setYRange(double min,double max);
    void setLabelLocaleOnAxis(const QString& index,const QLocale& loca);
    void initType_timeZone(const QList<QString>& args);
    void initType_shockReply();

   QAbstractAxis* axisX;
   QAbstractAxis* axisY;

    LineItem* addLineCursor();
    void removeLineCursor(LineItem* c);

    QLineSeries* getLine(int index);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event)override;

public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);
    void clearCallout();

private:

    QChart *m_chart;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;

    QRubberBand* m_rubberBand;
    QPoint m_rubberBandOrigin;
    //flags;
    bool isRubberBand=false;//for zooming
    bool isCommenting=false;
    bool isShowingCallout=false;


    //default series is 0 des 1 uplimit 2 downlimit ,3 is dataSeries
    QMap<int,QLineSeries*> lines;
    QMap<int,QVector<QPair<QPointF,QPointF>>>* pointMap=nullptr;




    int maxPeek=3;
    int minPeek=3;
    int bothPeek=3;

    QHash<QXYSeries::PointConfiguration, QVariant>* conf=nullptr;

    LineItem* pLineCursor;
    QPair<LineItem*,LineItem*> zontCursorPair;



    void initPointMap();


    //series;
};

#endif // ENHANCEDCHARTVIEWSYSTEM_H
