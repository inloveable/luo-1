#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "qtcpsocket.h"
#include <QIODevice>

class InputDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit InputDevice(QTcpSocket* s,QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
private:
    QTcpSocket* socket;
};

#endif // INPUTDEVICE_H
