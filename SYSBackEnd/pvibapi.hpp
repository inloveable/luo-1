#pragma once





#include "qhostaddress.h"
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include<vector>
#include<QPointF>
#include<bitset>
#include<thread>
#include<optional>
#include"3rdparty/readerwriterqueue.h"
struct DeviceInfo;
struct _CARD_PARM;
struct _CH_PARM ;
struct _DataHeadInfo;
struct _GROUP_INFO;



//any attempt to use this class should only be  based on smart_ptr
class PVIBApi:
        public std::enable_shared_from_this<PVIBApi>
{
public:
    PVIBApi();
    PVIBApi(const PVIBApi& a);
    PVIBApi(const PVIBApi&& a)=delete;
    PVIBApi& operator=(const PVIBApi&&)=delete;
    PVIBApi& operator=(const PVIBApi&)=delete;

    ~PVIBApi();

    bool init();
    bool connectToHost(QHostAddress add,int ip);
    bool isBackEndReady(){return this->isDeviceReady;};

    int getDeviceNum()const {return currentDeviceNum;};
    std::vector<std::shared_ptr<DeviceInfo>> getDeviceInfo();

    //theoretically 8channels at maximum...
    std::bitset<8>& isChannelRealReady();




    void setArgumentFlag(bool flag,int channel){argumentChangeFlagSet[channel]=flag;};
    void setACQ_LEN(uint length,int ch=0);
    void setACQ_RATE(int rate,int ch=0);
    void setDelay_len(uint length,int ch=0);
    void setCH_INPUT_TYPE(bool arg,int ch=0);
    void setCH_FUNC_TYPE(int stall,int ch=0);
    void setCH_Sensitivity(double value,int ch=0);
    void setRT_EN(bool arg);//real time switch;
    void setTrig_Level(double val,int ch=0);




    int getChRTDataSize(int ch=0){return rawData_temp[ch].size();};


    bool updateProperty(int channel);
    int startRTAcquisition();
    bool stopAcquisition();
    void setActiveChannel(int channel,bool active=true)
    {
        markSet[channel]=active;
    };

    int startRealAcqusition(int channel=0);
    void stopRealAcqusition(int channel=0);
    int getSampleStatue(){return sampleStatus;};
    void setSampleStatue(int val){sampleStatus=val;};
    QVector<QPointF> getSampledData(int ch=0){return sampledBuffer[ch];};


    //int startAcquisition_NoRealTime(int channel);

    std::shared_ptr<QVector<QPointF>> getRawData(int channel=0);
    bool isChannelRawDataReady(int index)
    {
        return rawData[index].size()<MAXIMUM_BUFFER_POINT_COUNT;
    };
    //differ than last func this func
    //should be used for processing gathered data(not automatically gathered)
    std::shared_ptr<std::vector<QPointF>> getRealData(int channel=0);
    //what's the difference?
private:


   std::vector<std::shared_ptr<_CARD_PARM>>paras;
   std::vector<std::shared_ptr<_CH_PARM>>  chParas;
   std::shared_ptr<_DataHeadInfo> dataInfo;
   std::shared_ptr<_GROUP_INFO>   groupInfo;



    int currentDeviceNum=0;

    bool isDeviceReady=false;


    struct{
        int         Id;
        std::string Ip;
        std::string Group;
        int         CardNum;
        ulong        channelCount;
        int         currentChannel=0;
    }currentDevice;

    std::mutex readMutex;
    std::array<QVector<QPointF>,8> rawData;
    std::array<QVector<QPointF>,8> rawData_temp;
    std::bitset<8> markSet{false};
    std::bitset<4> argumentChangeFlagSet{false};


    class  DataPack{
    public:
        DataPack():xValue{{0}},yValue{{0}}{};
        DataPack(std::array<double,8> x,std::array<double,8> y): xValue{x},yValue{y}
         {
         }

        DataPack( DataPack&& ano)=default;
        DataPack(const DataPack& ano)=default;
        DataPack& operator=(DataPack&&)=default;
        DataPack& operator=(DataPack&)=default;
        QPointF toPoint(int channel=0) const;

    private:
        std::array<double,8>  xValue;
        std::array<double,8>  yValue;

    };

    moodycamel::ReaderWriterQueue<DataPack>* lockFreeBuffer=nullptr;
    std::array<QVector<QPointF>,4>      sampledBuffer;
    std::mutex                               sampledLock;


    void dataGetThread();
    void dataProcessThread();


    void waitForSamplingThread();
    bool globalFlag=true;

    //dataGetThread           dataProcessThread                       when reaches a max size
    //                                     QPointF
    //read to->lockFreeBuffer<- read from -------->send to-->rawData;   send out raw data;

    //processed  data is represented with points and should be consumed by srs through sysbackend


    static const int MAXIMUM_BUFFER_POINT_COUNT=4096;

    bool isContinuingRealTime=false;
    bool isSampling=false;
    int  sampleStatus=-1;

    void updataCardParameters();



    std::array<double,3000> xCordMap;
};

