#ifndef PDFWRITER_H
#define PDFWRITER_H

#include "qfont.h"
#include "qnamespace.h"
#include <QObject>
#include<QVector>
#include<QString>
#include <vector>
#include<QTextOption>
class QPdfWriter;
class QPainter;
class QPrinter;
class QFile;
class PDFWriter : public QObject
{
    Q_OBJECT
public:
    explicit PDFWriter(QFile* f,QObject *parent = nullptr);
    ~PDFWriter();



    void paintTable(const QVector<QVector<QString>>& table ,
                   const QRectF& position,
                    const QVector<int>& hwidtth,
                    const QVector<int>& vwidth);
    void paintSignatureLine(const QString& label,const QString& name,int length);
    void paintEmptySpace(const QRectF& rec);//for test,printBlackArea
    void paintPixmap(const QRect& position,const QPixmap& map);
    void paintStraghtHLine(const QPointF& begin,int length);
    void paintText(const QPointF& begin,const QString& word);
    void paintText(const QRectF& position,const QString& word,Qt::Alignment all=Qt::AlignCenter);
    void paintBoltText(const QRectF& position,const QString& word,Qt::Alignment all=Qt::AlignCenter);
    void drawTitle(const QString& title);

    void addNewPage();


    QFont& getFont(){return font;};
    void   setFont(const QFont& f){font=f;};
    QPen&  getPen(){return *pen;};

signals:

private:
    QPdfWriter* pdfWriter;
    QPainter*   painter;

    QTextOption option;




    QFont font;
    QPen* pen;
    QFile* file;

};

#endif // PDFWRITER_H
