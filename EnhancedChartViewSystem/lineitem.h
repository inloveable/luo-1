#ifndef LINEITEM_H
#define LINEITEM_H

#include "qnamespace.h"
#include <QGraphicsLineItem>
#include <QObject>

class LineItem :public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    LineItem(qreal x1, qreal y1, qreal x2, qreal y2,QGraphicsItem *parent = nullptr);
    void setColor(const QColor& c);

    void setLineShape(int type);
    void setLineWidth(int width);

protected:
   void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event )override;
   void mouseReleaseEvent( QGraphicsSceneMouseEvent* event )override;
   QRectF boundingRect()const override;

   void paint(QPainter *painter,
              const QStyleOptionGraphicsItem *option,
              QWidget *widget) override;
signals:
   qreal requestUpdateValue();
 private:
  bool mousePressed=false;
  QPointF mousePressedPoint;

  qreal x2;
  qreal y2;
  qreal x1;
  qreal y1;

  QColor color;
  int width=1;
  Qt::PenStyle lineShape=Qt::SolidLine;

  QPointF m_pos;
  QPointF m_pressedPos;
  QPointF m_startPos;

  QRectF rec;
  QRectF boundRec;

  qreal value=27;
};

#endif // LINEITEM_H
