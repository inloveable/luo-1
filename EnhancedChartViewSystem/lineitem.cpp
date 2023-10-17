#include "lineitem.h"
#include "qgraphicssceneevent.h"
#include "qnamespace.h"
#include "qpainter.h"

LineItem::LineItem(qreal x1, qreal y1, qreal x2, qreal y2,QGraphicsItem *parent)
    :QGraphicsItem{parent},x2{x2},y2{y2},x1{x1},y1{y1}
{
    rec={x1-10,y1,x2-x1+50,y2-y1};
}


QRectF LineItem::boundingRect() const
{
    return rec;
}

void LineItem::setColor(const QColor& c)
{
    color=c;
    update();
}
void LineItem::setLineWidth(int w)
{
    width=w;
    update();
}
void LineItem::setLineShape(int type)
{
    this->lineShape=static_cast<Qt::PenStyle>(type);
    update();
}
void LineItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    auto pen=painter->pen();
    pen.setColor(color);
    pen.setWidth(width);
    pen.setStyle(lineShape);
    painter->setPen(pen);

    painter->drawLine(x1,y1,x2,y2);
    painter->drawText(QPointF{x2+10,y2},QString("%1").arg(value));
}

void LineItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    mousePressed=true;
    auto point=event->scenePos();
    point.setY(0);
    mousePressedPoint=this->scenePos()-point;
}

void LineItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    if(mousePressed==true)
    {
        auto point=event->scenePos();
        point.setY(0);
        qDebug()<<point+mousePressedPoint;
        this->setPos(point+mousePressedPoint);
    }
}

void LineItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    value = emit requestUpdateValue();
    mousePressed=false;
    update();
}
