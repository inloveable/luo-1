#include "networkmanager.h"
#include "qtcpsocket.h"
#include<QTcpSocket>


NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
{

}

void NetworkManager::connectToHost(QHostAddress& add,long port)
{
    if(socket==nullptr)
    {
        socket=new QTcpSocket(this);
    }
    socket->connectToHost(add,port);
    bool sucess=socket->waitForConnected();
    if(!sucess)
    {


    }
    else
    {
        connect(socket,&QTcpSocket::readyRead,
                this,&NetworkManager::readData);
    }
    emit connectResult(sucess);
}

void NetworkManager::readData()
{

}
