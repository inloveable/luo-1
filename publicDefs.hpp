#pragma once

#include <array>
#include<QString>
#ifndef SINGLETON_DECLARE
#define SINGLETON_DECLARE(Class) \
public:\
    static Class *GetInstance();\
    void Destory(){\
         realDestroy();\
    }\
private:\
    static Class *m_pInstance;\
    void realDestroy();\

//cpp中添加实现
#define SINGLETON_IMPLEMENT(Class) \
    Class * Class::m_pInstance = new Class;\
    Class * Class::GetInstance()\
{\
    return m_pInstance;\
}\

#endif


class targetpoint;
class mTargetPoint{
public:
    mTargetPoint(){};
    mTargetPoint(double fre,double accel,double upMiss, double lowMiss)
        :x(fre),y{accel},upMiss{upMiss},lowMiss{lowMiss}
    {

    };
    targetpoint toTargetPoint();
private:
    double x;
    double y;
    double upMiss;
    double lowMiss;

};


class SrsStateBar{
public:
    SrsStateBar(){};

    inline  void setMaxVal(double val);
    inline  void setMinVal(double val);
    inline  void setMaxFre(double fre);
    inline  void setMinFre(double fre);
    inline  void setDampRate(double rate);
    inline  void setExceed(double val);


    QString getDampRate(){return arrs[0];};
    QString getMaxVal(){return arrs[1];};
    QString getMaxFre(){return arrs[2];};
    QString getMinVal(){return arrs[3];};
    QString getMinFre(){return arrs[4];};
    QString getExceed(){return arrs[5];};

private:

        std::array<QString,6> arrs{""};
};

void SrsStateBar::setDampRate(double val)
{
    QString tmp=QString::number(val);
    arrs[0]=tmp;
}

void SrsStateBar::setMaxVal(double val)
{
    QString tmp=QString::number(val);
    arrs[1]=tmp;
}

void SrsStateBar::setMaxFre(double fre)
{
    QString tmp=QString::number(fre);
    arrs[2]=tmp;
}

void SrsStateBar::setMinVal(double val)
{
    QString tmp=QString::number(val);
    arrs[3]=tmp;
}

void SrsStateBar::setMinFre(double fre)
{
    QString tmp=QString::number(fre);
    arrs[4]=tmp;
}

void SrsStateBar::setExceed(double val)
{
    QString tmp=QString::number(val);
    arrs[5]=tmp;
}
