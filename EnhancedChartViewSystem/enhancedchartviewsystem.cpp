#include "enhancedchartviewsystem.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include "callout.h"
#include "qgraphicsitem.h"
#include "qlogvalueaxis.h"
#include "qnamespace.h"
#include "qpoint.h"
#include "qvalueaxis.h"
#include <QtGui/QMouseEvent>
#include<QRubberBand>
#include<QValueAxis>
#include <algorithm>
#include <array>
#include<random>
#include<lineitem.h>
#include<QFile>
#include<QLogValueAxis>
#include<QGraphicsEllipseItem>
#include<QLoggingCategory>

EnhancedChartViewSystem::EnhancedChartViewSystem(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_chart(0),
      m_tooltip(0)
{

    QLoggingCategory::setFilterRules("*.debug=false\n"
                                        "*.info=false\n"
                                        "*.warning=false\n"
                                        "*.critical=true");

    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    m_rubberBand=new QRubberBand(QRubberBand::Rectangle, this);
    m_rubberBand->setEnabled(true);
    // chart
    m_chart = new QChart;

    if(!conf)
    {
        conf=new QHash<QXYSeries::PointConfiguration, QVariant>;
        (*conf)[QXYSeries::PointConfiguration::Color] = QColor(Qt::red);
        (*conf)[QXYSeries::PointConfiguration::Size] = 6;
        (*conf)[QXYSeries::PointConfiguration::LabelVisibility] = true;

    }

    m_chart->setTheme(QChart::ChartThemeDark);


}

void EnhancedChartViewSystem::initType_timeZone(const QList<QString>& args)
{

   QLineSeries *series = new QLineSeries;

    m_chart->addSeries(series);

    ///setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //m_chart->update();
    m_chart->createDefaultAxes();
    axisX=static_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
    axisY=static_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());

     axisX->setRange(0,300);
     axisY->setRange(-8000,8000);
    static_cast<QValueAxis*>(axisX)->setLabelFormat("%d ms");


    m_chart->setAcceptHoverEvents(true);

    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(m_chart);

    connect(series, &QLineSeries::clicked, this, &EnhancedChartViewSystem::keepCallout);
    connect(series, &QLineSeries::hovered, this, &EnhancedChartViewSystem::tooltip);

    lines.insert(3,series);
    this->setMouseTracking(true);

}



void EnhancedChartViewSystem::initType_shockReply()
{

    lines[3]=new QLineSeries;
    axisX=new QLogValueAxis;
    axisY=new QLogValueAxis;
    QLogValueAxis* xAxis=static_cast<QLogValueAxis*>(axisX);//tricky
    QLogValueAxis* yAxis=static_cast<QLogValueAxis*>(axisY);
   xAxis->setBase(10);
   xAxis->setRange(20,10000);
   xAxis->setLabelFormat("%d Hz");
   yAxis->setBase(10);
   yAxis->setLabelFormat("%d g");
   yAxis->setRange(10,10000);

   m_chart->addAxis(xAxis,Qt::AlignBottom);
   m_chart->addAxis(yAxis,Qt::AlignLeft);
   m_chart->setTitle("加速度");

   lines[3]->setColor(Qt::yellow);
   lines[3]->setName("通道1");
   m_chart->addSeries(lines[3]);
   lines[3]->attachAxis(xAxis);
   lines[3]->attachAxis(yAxis);
   connect(lines[3], &QSplineSeries::clicked, this, &EnhancedChartViewSystem::keepCallout);
   connect(lines[3], &QSplineSeries::hovered, this, &EnhancedChartViewSystem::tooltip);

   setRenderHint(QPainter::Antialiasing);
   scene()->addItem(m_chart);
}


LineItem* EnhancedChartViewSystem::addLineCursor()
{
    auto position=m_chart->mapRectToScene(m_chart->plotArea());
    pLineCursor=new LineItem{position.topLeft().x()+100,
            position.topLeft().y(),position.topLeft().x()+100,position.bottomLeft().y()};
    connect(pLineCursor,&LineItem::requestUpdateValue,this,[=]()
    {
        auto item=dynamic_cast<LineItem*>(sender());
        auto scenePosition=item->scenePos();
        auto chartPosition=m_chart->mapFromScene(scenePosition);
        return chartPosition.x();
    });
    this->scene()->addItem(pLineCursor);

    return pLineCursor;
}

QLineSeries* EnhancedChartViewSystem::getLine(int index)
{
    if(lines.find(index)==lines.end())
    {
        qDebug()<<"requesting a wrong index when getLine[EnhancedChartViewSystem]";
        return nullptr;
    }
    return lines[index];

}

void EnhancedChartViewSystem::removeLineCursor(LineItem* c)
{
    this->scene()->removeItem(c);
}


void EnhancedChartViewSystem::setChartTitle(const QString& t)
{
    m_chart->setTitle(t);
}

void EnhancedChartViewSystem::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());

         const auto callouts = m_callouts;
         for (Callout *callout : callouts)
             callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}

void EnhancedChartViewSystem::setOpenGL(bool s)
{
    for(auto&& i:this->lines)
    {
        i->setUseOpenGL(s);
    }
}

void EnhancedChartViewSystem::setShowCallout(bool s)
{
    if(s)
    {
        for(int i=0;i<lines.size();++i)
        {
          connect(lines[i], &QSplineSeries::clicked, this, &EnhancedChartViewSystem::keepCallout);
          connect(lines[i], &QSplineSeries::hovered, this, &EnhancedChartViewSystem::tooltip);
        }
    }
    else
    {
        for(int i=0;i<lines.size();++i)
        {
          disconnect(lines[i], &QSplineSeries::clicked, this, &EnhancedChartViewSystem::keepCallout);
          disconnect(lines[i], &QSplineSeries::hovered, this, &EnhancedChartViewSystem::tooltip);
        }
    }
    isShowingCallout=s;

}

void EnhancedChartViewSystem::mouseMoveEvent(QMouseEvent *event)
{

    if (m_rubberBand && m_rubberBand->isVisible())
    {

        int width = event->pos().x() - m_rubberBandOrigin.x();
        int height = event->pos().y() - m_rubberBandOrigin.y();
        m_rubberBand->setGeometry(QRect(m_rubberBandOrigin.x(),
                                        m_rubberBandOrigin.y(), width, height).normalized());
    }
    else
    {

        QGraphicsView::mouseMoveEvent(event);

    }
}

void EnhancedChartViewSystem::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void EnhancedChartViewSystem::clearCallout()
{
    for(auto&& i:m_callouts)
    {
        scene()->removeItem(i);
    }
    m_callouts.clear();
}

void EnhancedChartViewSystem::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_rubberBand && m_rubberBand->isVisible())
    {
        if (event->button() == Qt::LeftButton)
        {
            m_rubberBand->hide();
            QRectF rect = m_rubberBand->geometry();
            m_chart->zoomIn(rect);
            event->accept();
        }

    }
    else if (m_rubberBand &&
             event->button() == Qt::RightButton && isRubberBand)
    {
        m_chart->zoomOut();
        event->accept();
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);

    }
}

void EnhancedChartViewSystem::mousePressEvent(QMouseEvent* event)
{

    //------>x
    //-
    //-
    //
      QRectF plotArea =m_chart->plotArea();
      if (m_rubberBand && m_rubberBand->isEnabled()
              && event->button() == Qt::LeftButton
              && plotArea.contains(event->pos())
              && isRubberBand) {
          m_rubberBandOrigin = event->pos();
          m_rubberBand->setGeometry(QRect(m_rubberBandOrigin, QSize()));
          m_rubberBand->show();
          event->accept();
      }
      else
      {

          QGraphicsView::mousePressEvent(event);
      }

}

void EnhancedChartViewSystem::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);
    if(!(this->isShowingCallout))
    {
        return;
    }

    if (state) {
        qDebug()<<m_chart->mapFromScene(point);
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void EnhancedChartViewSystem::resetZoom()
{
    m_chart->zoomReset();
}

void EnhancedChartViewSystem::addSeries(int index,QLineSeries* se)
{

    //you'll have to attach axis by your own
    lines.insert(index,se);
    m_chart->addSeries(se);


}

void EnhancedChartViewSystem::showPeekValue(int index ,PEEK type)
{
    auto line=lines.find(index);
    if(line==lines.end())
    {
        qDebug()<<"line not found";
        return;
    }

    line.value()->clearPointsConfiguration();
    m_chart->update();

    double previousV=0;

    QVector<int> maxPoints;
    QVector<int> minPoints;
    auto points=line.value()->points();

    for(int i=0;i<points.size()-1;++i)
    {
        double currentV=points[i].ry();
        if(currentV>previousV&&currentV>points[i+1].ry())
        {
            maxPoints.push_back(i);
            if(maxPoints.size()>maxPeek)
            {
                break;
            }

        }
        else if(currentV<previousV&&currentV<points[i+1].ry())
        {
            minPoints.push_back(i);
            if(minPoints.size()>minPeek)
            {
                break;
            }
        }

        previousV=currentV;
    }



    if(type==PEEK::BOTH)
    {
        for(auto&& i:maxPoints)
        {
            line.value()->setPointConfiguration(i,*conf);
        }
        for(auto&& i:minPoints)
        {
            line.value()->setPointConfiguration(i,*conf);
        }
    }
    else if(type==PEEK::MAX)
    {
        for(auto&& i:maxPoints)
        {
            line.value()->setPointConfiguration(i,*conf);
        }
    }
    else if(type==PEEK::MIN)
    {
        for(auto&& i:minPoints)
        {
            line.value()->setPointConfiguration(i,*conf);
        }
    }
}


void EnhancedChartViewSystem::scollOverX(bool direction)
{

    int dx=10;
    qDebug()<<"dx"<<dx;
    if(direction)
    {
        m_chart->scroll(dx,0);
    }
    else
    {
        m_chart->scroll(-dx,0);
    }
}

void EnhancedChartViewSystem::scollOverY(bool direction)
{

    int dy=10;
    if(direction)
    {
        m_chart->scroll(0,dy);
    }
    else
    {
        m_chart->scroll(0,-dy);
    }

     qDebug()<<"dY"<<dy;
}

void EnhancedChartViewSystem::setSeriesLineWidth(int index,int width)
{
    auto iter=lines.find(index);
    if(iter==lines.end())
    {
        return;

    }
    QPen p=iter.value()->pen();
    p.setWidth(width);
    iter.value()->setPen(p);
}

void EnhancedChartViewSystem::setLineColor(int index,QColor c)
{
    auto iter=lines.find(index);
    if(iter==lines.end())
    {
        return;

    }
    QPen p=iter.value()->pen();
    p.setColor(c);
    iter.value()->setPen(p);
    update();
}

void EnhancedChartViewSystem::setLineVisibility(int index, bool s)
{
    auto iter=lines.find(index);
    if(iter==lines.end())
    {
        return;
    }
    iter.value()->setVisible(s);
}

void EnhancedChartViewSystem::setLineType(int index,const QString& type)
{
    auto iter=lines.find(index);
    if(iter==lines.end())
    {
        return;
    }
    qDebug()<<"line type:"<<type;
    QPen p=iter.value()->pen();

    if(type=="实线")
    {
        p.setStyle(Qt::SolidLine);
    }
    else if(type=="虚线")
    {
        p.setStyle(Qt::DashLine);
    }
    else if(type=="点")
    {
        p.setStyle(Qt::DotLine);
    }
    else if(type=="点划线")
    {
         p.setStyle(Qt::DashDotLine);
    }
    else if(type=="双点划线")
    {
         p.setStyle(Qt::DashDotDotLine);
    }
    iter.value()->setPen(p);
}

void EnhancedChartViewSystem::transformDataSeries(int index,double rate)
{
    auto iter=lines.find(index);
    if(iter==lines.end())
    {
        return;
    }

    auto points=iter.value()->points();
    for(auto&& p:points)
    {
        p.ry()=(p.ry()*rate);
    }
    iter.value()->replace(points);
}

void EnhancedChartViewSystem::setXRange(double min,double max)
{
    this->axisX->setRange(min,max);
}

void EnhancedChartViewSystem::setYRange(double min,double max)
{
    this->axisY->setRange(min,max);
}
