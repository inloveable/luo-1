#pragma once




#include "qhostaddress.h"
#include <QObject>
#include <memory>
#include <vector>
#include<QPointF>
#include"../publicDefs.hpp"
class PVIBApi;
struct DeviceInfo;
class SRS;
class QTimer;

class SYSBackEnd : public QObject
{
    Q_OBJECT
public:



    explicit SYSBackEnd(QObject *parent = nullptr);
    ~SYSBackEnd();
    SRS& getBackEnd(){return *backend;};

    enum class ArgType{ACQ_LEN,ACQ_RATE,RT_ENABLE,DLY_LEN,TRIG_MODE,CH_FUNC_TYPE,
                      CH_INPUT_TYPE,CH_SENSITIVITY,TRIG_LEVEL,CH_UNIT,
                      Q_VALUE,D_RATIO,REF_F,BIAS_F,BIAS_M};
    //all must read the doc carefully before dev



    void connectToHost(QHostAddress add);
    void startAcqusition();
    void stopAcqusition();

    void startRTAcqusition();
    void stopRTAcqusition();
    void setActiveChannel(int ch,bool active=true);

   //for test

   void readAccelerateFromFile(const QString& path);
   void readTimeZoneFromFile(const QString& path);

   std::shared_ptr<QVector<QPointF>> getRawData(int channel=0);
   bool                                  stopGetingRawData();

   class  mDeviceInfo {
   public:

       mDeviceInfo(const DeviceInfo& ano);

   public:
       unsigned long Id;
       unsigned char Ip[4];
       unsigned char Type;//0机箱、1模块
     //  QString groupName;
      // int     groupcardNum;



   };





   using DeviceInfoPtr_t=std::shared_ptr<mDeviceInfo>;
   using DeviceInfoList_t=std::vector<DeviceInfoPtr_t>;
   DeviceInfoList_t getDeviceList();

public slots:
   QVector<QPointF>  getAccelerateSequence();
   QVector<QPointF>  getTimeZoneSequence();
   void reCalculateSRSResult();

   void updateArgument(ArgType type,const QString& newer,int ch=0);
   void init();


signals:
   //rec  1 for accelerate ,rec 2 as timezone -1 as error
    void dataProcessFinished(int rec,int channel=0);//there might be multi-channels in the future
    void readDataReady(const QVector<QPointF>& points,int channel=0);



    void RTDataReadyForAcquire(int ch=0);

    void acqusitionStarted(bool);


    bool requesRTtAcqusitionStop();

    QMap<QString,QString> requestSRSArguments(int channel=0);
    void                  sendSampledRawData(std::shared_ptr<QVector<QPointF>>,int ch);
    QVector<mTargetPoint>         requesetTargetSequence();

    void sendStateBar(SrsStateBar bar,int ch=0);





private:

    std::shared_ptr<SRS> backend;

    QVector<QPointF>      accerlateSequence;
    QVector<QPointF>      timeZoneSequence;

    std::shared_ptr<PVIBApi> device;

    QTimer* sendOutTimer;


    bool isAcqusitionStarted=false;


    std::array<std::shared_ptr<QVector<QPointF>>,4> channelLists;


    void processSamplingData(int ch=0);
    void calculateSRSResult(std::shared_ptr<QVector<QPointF>> sequence);

    double xAxisBound=300;

    std::shared_ptr<QVector<QPointF>> lastSequence=nullptr;
};

