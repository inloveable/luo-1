#include "inputdevice.h"
#include "qtcpsocket.h"

InputDevice::InputDevice(QTcpSocket* s,QObject *parent)
    : QIODevice{parent},socket(s)
{

}

qint64 InputDevice::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1;
}
