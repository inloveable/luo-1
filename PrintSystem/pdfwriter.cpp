#include "pdfwriter.h"
#include "qdebug.h"
#include "qnamespace.h"
#include "qpagesize.h"
#include<QPdfWriter>
#include<QFile>
#include<QPainter>
#include<QPrinter>

PDFWriter::PDFWriter(QFile* f,QObject *parent)
    : QObject{parent},option(Qt::AlignCenter)
{
   file=f;
   qDebug()<<f->open(QIODevice::ReadWrite);
   pdfWriter=new QPdfWriter(f);
   painter=new QPainter(pdfWriter);

   pdfWriter->setPageSize(QPageSize::A4);                  //设置纸张为A4纸
   pdfWriter->setResolution(QPrinter::ScreenResolution);           //设置分辨率 屏幕分辨率 打印机分辨率 高分辨率
   pdfWriter->setPageMargins(QMarginsF(40, 40, 40, 40));
   //设置页边距 顺序是:左上右下


           font.setFamily("Microsoft YaHei");                            //设置字体 微软雅黑、宋体之类的
           font.setPointSize(10);                                        //设置字体大小
           font.setBold(false);                                           //加粗
           painter->setFont(font);

   qDebug()<<"height of the pdf:"<<pdfWriter->height();
   qDebug()<<"width of the pdf:"<<pdfWriter->width();
   qDebug()<<"title of the document"<<pdfWriter->title();


   pen=new QPen(QBrush(Qt::black), 8);
   painter->setPen(*pen);
   //当前设置下，一个汉字约为50，100长度的线对应1cm




}

void PDFWriter::drawTitle(const QString& title)
{
    option.setWrapMode(QTextOption::WordWrap);
    font.setPointSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(QRect(2800, 100, 4000, 300), title, option);
}

void PDFWriter::paintStraghtHLine(const QPointF& origin,int length)
{

    font.setFamily("Microsoft YaHei");                            //设置字体 微软雅黑、宋体之类的
    font.setPointSize(10);                                        //设置字体大小
    font.setBold(false);                                           //加粗
    painter->setFont(font);

      QPoint p1(origin.x()+length,origin.y());
      painter->drawLine(origin,p1);


}
void PDFWriter::paintText(const QPointF& origin,const QString& text)
{

    font.setFamily("Microsoft YaHei");                            //设置字体 微软雅黑、宋体之类的
    font.setPointSize(10);                                        //设置字体大小
    font.setBold(false);                                           //加粗
    painter->setFont(font);

    painter->drawText(origin,text);

}
void PDFWriter::paintBoltText(const QRectF& position,const QString& word,Qt::Alignment all)
{
    font.setPointSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(position,all,word);
}

void PDFWriter::addNewPage()
{
    pdfWriter->setPageSize(QPageSize::A4);                  //设置纸张为A4纸
    pdfWriter->setResolution(QPrinter::ScreenResolution);           //设置分辨率 屏幕分辨率 打印机分辨率 高分辨率
    pdfWriter->setPageMargins(QMarginsF(0,0,0,0));
    pdfWriter->newPage();
    qDebug()<<pdfWriter->pageLayout();
    qDebug()<<pdfWriter->height();
    qDebug()<<pdfWriter->width();
}

void PDFWriter::paintPixmap(const QRect& position,const QPixmap& map)
{
    painter->drawPixmap(position,map);
}


void PDFWriter::paintTable(const QVector<QVector<QString>>& table ,
                            const QRectF& position, const QVector<int> &hwidth,
                           const QVector<int> &vwidth)
{
    painter->drawRect(position);
    font.setFamily("Microsoft YaHei");                            //设置字体 微软雅黑、宋体之类的
    font.setPointSize(10);                                        //设置字体大小
    font.setBold(false);                                           //加粗
    painter->setFont(font);

    QPointF leftBegin=position.topLeft();
    QPointF rightEnd;
    for(int i=0;i<vwidth.size()-1;i++)
    {
        leftBegin.setY(leftBegin.y()+vwidth[i]);
        rightEnd.setX(leftBegin.x()+position.width());
        rightEnd.setY(leftBegin.y());
        painter->drawLine(leftBegin,rightEnd);
    }

    QPointF topBegin=position.topLeft();
    QPointF buttonEnd;
    for(int i=0;i<hwidth.size()-1;i++)
    {
        topBegin.setX(topBegin.x()+hwidth[i]);
        buttonEnd.setY(topBegin.y()+position.height());
        buttonEnd.setX(topBegin.x());
        painter->drawLine(topBegin,buttonEnd);
    }

    QPointF origin=position.topLeft();
    int offsetX=0;
    for(int i=0;i<hwidth.size();i++)
    {
        //horizontal first
        int offsetY=0;
        for(int j=0;j<vwidth.size();j++)
        {
            QRectF textRec{origin.x()+
                        offsetX,origin.y()+offsetY,static_cast<qreal>(hwidth[i]),
                        static_cast<qreal>(vwidth[j])};
            painter->drawText(textRec,Qt::AlignCenter,table[j][i]);
            offsetY+=vwidth[j];
        }
        offsetX+=hwidth[i];

    }


}
PDFWriter::~PDFWriter()
{
    delete painter;
    delete pdfWriter;
    delete pen;
    qDebug()<<"painter deleted";
}
