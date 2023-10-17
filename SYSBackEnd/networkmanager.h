#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include<QHostAddress>

#include<QPointF>

class QTcpSocket;
class QUdpSocket;

class NetworkManager : public QObject
{
    Q_OBJECT


public:
    explicit NetworkManager(QObject *parent = nullptr);

    void connectToHost(QHostAddress& add,long port);
    void readData();
signals:
    void connectResult(bool);
    void sendData(const QList<QPointF>& points,int index);

private:

    QTcpSocket* socket=nullptr;
    QUdpSocket* prepareSocket=nullptr;

    //QList<QPointF> c1;
    //QList<QPointF> c2;
   // QList<QPointF> c3;
   // QList<QPointF> c4;

    void conductData(int index);

};

#endif // NETWORKMANAGER_H
