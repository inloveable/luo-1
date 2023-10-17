
#include<QDateTime>
#include <array>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include"glog/logging.h"
#include "pvibapi.hpp"
#include"3rdparty/ApiPvib2_0.h"
#include "qhostaddress.h"
#include<QBuffer>
using RetDA_t=std::vector<std::shared_ptr<DeviceInfo>>;
//device array
PVIBApi::PVIBApi()
    :std::enable_shared_from_this<PVIBApi>{}
{
    double v=0;
    for(int i=0;i<xCordMap.size();++i)
    {
        xCordMap[i]=v;
        v+=0.1;
    }

     init();
}
PVIBApi::~PVIBApi()
{
    isContinuingRealTime=false;
    globalFlag=false;

    qDebug()<<"waiting for my threads...";
    if(lockFreeBuffer)
    {
        delete lockFreeBuffer;
    }
    qDebug()<<"end waiting...";
    CloseDll();
    LOG(INFO)<<"3rD PARTY DLL CLOSED";
}

bool PVIBApi::connectToHost(QHostAddress ip,int port)
{
     auto infoList=getDeviceInfo();
    bool found=false;
    qDebug()<<ip;
    std::shared_ptr<DeviceInfo> dstDevice=nullptr;

    LOG(INFO)<<"is device found:"<<found;

    if(infoList.size()==0)
    {
        LOG(INFO)<<"device not found";
        return false;
    }
    qDebug()<<"info list size"<<infoList.size();
    dstDevice=infoList[0];
    currentDevice.Id=dstDevice->Id;

    char buffer[32]={0};
    PVIB_STATUS groupStatus =GetGroupName(currentDevice.Id,0,buffer);
    if (groupStatus != PVIB_OK)
    {
        LOG(WARNING)<<("获取组名异常\n");
        return false;
    }
    currentDevice.Group=buffer;

    GROUP_INFO groupInf = { 0 };
    if (PVIB_OK != GetGroupInfo(currentDevice.Id, 0, &groupInf))
    {

        printf("获取设备信息异常\n");
        return false;
    }
    if (groupInf.CardNum == 0)//组内卡数判断
    {
        printf("获取设备信息异常\n");
        return false;
    }


    //只取第一个卡
    //取出所有的通道，但以标志位决定;
        CARD_PARM cardParam = { 0 };
        GetCardParm(currentDevice.Id, groupInf.CardSlotIdx[0], &cardParam);
        paras.push_back(std::make_shared<CARD_PARM>(cardParam));
        for (int ch = 0; ch < groupInf.ChNum[0]; ch++)
        {
            CH_PARM chparam = { 0 };//默认设置所有参数为0
            GetCardChParm(currentDevice.Id, groupInf.CardSlotIdx[0], ch, &chparam);
            chParas.push_back(std::make_shared<CH_PARM>(chparam));
        }
        LOG(INFO)<<"achieved cara channel num:"<<chParas.size();


    currentDevice.CardNum=groupInf.CardNum;
    currentDevice.channelCount=1;
    isDeviceReady=true;


    std::thread samplingThread{&PVIBApi::waitForSamplingThread,this};
    samplingThread.detach();



    return true;
}


void PVIBApi::waitForSamplingThread()
{
    while(globalFlag)
    {
         for(int i=0;i<4;++i)
         {
             if(argumentChangeFlagSet[i])
             {
                 updateProperty(i);
                 argumentChangeFlagSet.flip(i);
             }
         }

         if(!isSampling||!isDeviceReady)
         {
             std::this_thread::sleep_for(std::chrono::seconds(2));
             continue;
         }
         if(sampleStatus!=0)
         {
             std::this_thread::sleep_for(std::chrono::seconds(1));
             continue;
         }
         int dataBufLen = 2 * dataInfo->ChNum * 1024*64;//下载数据buf长度(假设设备是8通道16位数据宽度)
         unsigned char* dataBuf = new unsigned char[dataBufLen];//数据buf
         auto rec=DownLoadCardData(currentDevice.Id, groupInfo->CardSlotIdx[0],
                                         0, dataBufLen, dataBuf);
         if(rec!=PVIB_OK)
         {
             LOG(WARNING)<<"get card data not sucess";
             continue;
         }
         for (int pos = 0; pos < dataBufLen; pos += 2 * dataInfo->ChNum) {
             //顺序取出8个通道数据
             for (int ch = 0; ch < 8; ch++) {
                 if(!markSet[ch])
                 {
                     continue;
                 }
                 short v = 0;

                 unsigned char buf[2] = { dataBuf[pos + ch * 2 + 1],dataBuf[pos + ch * 2] };
                 memcpy(&v, buf, 2);
                 double vCh = v*dataInfo->DeltaY[ch];
                 sampledBuffer[ch].push_back(QPointF{static_cast<qreal>(pos/16)*dataInfo->DeltaX,vCh});

             }
         }
        delete[] dataBuf;
         LOG(INFO)<<"sample data finished";
         break;
    }
}

bool PVIBApi::init()
{
    markSet[0]=true;//set channel one to avalible
    PVIB_STATUS openStatus = OpenDll();
    if (PVIB_OK != openStatus)
    {
        qDebug()<<"error code:"<<openStatus;
        printf("device open failed!\n");
        return false;
    }
    else
    {
        printf("device open success!\n");
        return true;
    }
}

RetDA_t PVIBApi::getDeviceInfo()
{
    DeviceInfo devInfos[64]{{0}};
    int searchNum;
    PVIB_STATUS searchStatus = GetDeviceInfo(devInfos, 64, searchNum);
    if (PVIB_OK !=  searchStatus)
    {
        LOG(WARNING)<<"error when detect device.";
        return RetDA_t{};
    }

    if (searchNum == 0)
    {
        LOG(INFO)<<"search 0 device:"<<QDateTime::currentDateTime().
                   toLocalTime().toString().toStdString();
        return RetDA_t{};
    }

    currentDeviceNum=searchNum;

    RetDA_t rec;
    for(int i=0;i<searchNum;++i)
    {
         auto sPtr=std::make_shared<DeviceInfo>(devInfos[i]);
         rec.push_back(sPtr);
    }
    return rec;
}

bool PVIBApi::updateProperty(int channel)
{
    if(!isDeviceReady)//actually the corresponding parameters are ready
    {
        LOG(INFO)<<"updateProperty failed...device not ready";
        return false;
    }

    unsigned int groupNum = 0;
    PVIB_STATUS groupStatus = GetGroupCount(currentDevice.Id, &groupNum);
    if (groupStatus != PVIB_OK)
    {

        return false;
    }

    GROUP_INFO groupInf = { 0 };
    GetGroupInfo(currentDevice.Id, 0, &groupInf);

    for (int i = 0; i < groupInf.CardNum; i++)
    {
        PVIB_STATUS error=SetCardParm(currentDevice.Id, groupInf.CardSlotIdx[i], *paras[0]);
        if(error!=PVIB_OK)
        {
            LOG(WARNING)<<"error when set card parm";
        }

        error=SetCardChParm(currentDevice.Id, groupInf.CardSlotIdx[i], channel, *chParas[channel]);
        if(error!=PVIB_OK)
        {
            LOG(WARNING)<<"error when set card ch parm";
        }

    }
    return true;
}

int PVIBApi::startRealAcqusition(int channel)
{
    if (PVIB_OK != StartGroupAcq(currentDevice.Id, 0))
    {printf("688\n");
        return -1;}


      isSampling=true;
      return true;
}

int PVIBApi::startRTAcquisition()
{
   if(!isDeviceReady)
   {
       LOG(INFO)<<"device not ready unable to start acqusition";
       return -1;
   }


    PVIB_STATUS error;

   GROUP_INFO groupInf = { 0 };
   error=GetGroupInfo(currentDevice.Id, 0, &groupInf);

   if(error!=PVIB_OK)
   {
       LOG(WARNING)<<"group info access failed,error :"<<error;
       return -2;
   }

   groupInfo=std::make_shared<GROUP_INFO>(groupInf);

   //Set Card  Param
   CARD_PARM cardParam = { 0 };
   if(PVIB_OK != GetCardParm(currentDevice.Id, groupInf.CardSlotIdx[0], &cardParam))
   {
       LOG(WARNING)<<"Card Parm Get error :";
       return -3;
   }
   cardParam.AcqClkIdx = 3;//设置最大采样率
   cardParam.AcqLen = 1024*64;//设置采样点长度1024个(若设备是8通道16位数据宽度，总长度为2*8*1024字节)
   cardParam.DlyLen = 1024*20;
   cardParam.ClkSrc = 1;//时钟源选择总线
   cardParam.GoSrc = 1;
   cardParam.RtEn = 1;//打开实时采集
   cardParam.TrgMode = 5;//设置内触发上升沿模式
   if (PVIB_OK != SetCardParm(currentDevice.Id, groupInf.CardSlotIdx[0], cardParam))
    {
       LOG(WARNING)<<"Card Parm Set error :";
       return -3;
    }

   //Set Card Channle Param
   for (int ch = 0; ch < groupInf.ChNum[0]; ch++)
   {
       CH_PARM chparam = { 0 };//默认设置所有参数为0
       chparam.ChFuncType = 0;//电压模式
       chparam.LpIdx = 0;//设置低通滤波为直通
       chparam.ChK = 1;//通道K系数
       chparam.V_RangeIdx = 0;//设置量程第一个档
       chparam.Sensitivity = 0.1;//设置灵敏度
       chparam.ChUnit[0] = 'g' ;  // 标定单位
       chparam.TrgChEn = 1;//通道内触发允许
       chparam.TrgLv0 = 327.6;//设置通道触发电平1
   //eq: TrgLv0 =（实际触发量级*灵敏度）* （32768/电压量程）；
   //触发量级单位与标定单位一致（标定单位为'g'，则设置的触发量级也是单位为'g'），电压量程默认为0档10v
       if (PVIB_OK != SetCardChParm(currentDevice.Id, groupInf.CardSlotIdx[0], ch, chparam))
       {
           LOG(WARNING)<<"Card Ch Parm Set error :";
           return -3;
       }
   }
   //data info is commen in 8 channels;
   DataHeadINFO dataHead = { 0 };
   error=GetDataHead(currentDevice.Id, groupInf.CardSlotIdx[0], &dataHead);

   if(error!=PVIB_OK)
   {
        LOG(WARNING)<<"DATA HEAD access failed,error :"<<error;
        return -3;
   }
    //start collection
   error=StartGroupAcq(currentDevice.Id, 0);

  // if(error!=PVIB_OK)
  // {
  //      LOG(WARNING)<<"Start group,error :"<<error;
   //     return -3;
   //}


   dataInfo=std::make_shared<DataHeadINFO>(dataHead);

   //achived;

   isContinuingRealTime=true;
   if(lockFreeBuffer)
   {
       delete lockFreeBuffer;
    }
   lockFreeBuffer=new moodycamel::ReaderWriterQueue<DataPack>;
   std::thread reader{&PVIBApi::dataGetThread,shared_from_this()};
   std::thread consumer{&PVIBApi::dataProcessThread,shared_from_this()};

   reader.detach();
   consumer.detach();

   //now dangerous,be careful


   return 0;

}

void PVIBApi::dataGetThread()
{
    constexpr int dataRTBufLen=64*1024;
    uchar    dataRTBuf[dataRTBufLen];
    int status=-1;
    //Get realtime data when collecting
    {
        while(true&&isContinuingRealTime)//under what condition?
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            PVIB_STATUS error;
            error=DownLoadRealData(currentDevice.Id, groupInfo->CardSlotIdx[0], dataRTBufLen, dataRTBuf);
            if(error!=PVIB_OK)
            {
                LOG(INFO)<<"sleeping for real Data...error code"<<error;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                continue;
            }
            double x=0;
            for (int pos = 0; pos < dataRTBufLen; pos += 2 * 8)
            {
                //顺序取出8个通道数据
                std::array<double,8> xArray;
                std::array<double,8> yArray;
                for (int ch = 0; ch < 1; ch++)
                {
                    if(!this->markSet[ch])
                    {
                        xArray[ch]=0;
                        yArray[ch]=0;
                        continue;//continue when channel not needed;
                    }

                    short val = 0;
                    //假设windows系统小端字节顺序处理数据 ?
                    unsigned char buf[2] = { dataRTBuf[pos + ch * 2 + 1],dataRTBuf[pos + ch * 2] };
                    memcpy(&val, buf, 2);
                    double channelCurrentValue = val*this->dataInfo->DeltaY[ch];


                    xArray[ch]=pos*this->dataInfo->DeltaX;
                    yArray[ch]=channelCurrentValue*1000;
    //                if (channelCurrentValue>1.0)
    //                {

    //                    LOG(INFO)<<"big channelCurrentValue:"<<channelCurrentValue;
    //                }


                    //unsafe
                }
                if(lockFreeBuffer->size_approx()>10*MAXIMUM_BUFFER_POINT_COUNT)
                {
                    continue;
                }
                lockFreeBuffer->enqueue(DataPack{xArray,yArray});

            }
            if (PVIB_OK != GetGroupStatus(currentDevice.Id, sampleStatus, 0))
            {
                LOG(WARNING)<<"GetGroupStatus :";
                isContinuingRealTime=false;
            }
            if (sampleStatus==0)
            {
                qDebug()<<"samping finished";
                isContinuingRealTime=false;
            }
        }
    }
    LOG(INFO)<<"get realtime data exited successfully";
    LOG(INFO)<<"begin to download the sample data";
    //begin to download the sample data
    {
        int dataBufLen = 2 * dataInfo->ChNum * 1024*64;//下载数据buf长度(假设设备是8通道16位数据宽度)
        unsigned char* dataBuf = new unsigned char[dataBufLen];//数据buf
        auto rec=DownLoadCardData(currentDevice.Id, groupInfo->CardSlotIdx[0],
                                        0, dataBufLen, dataBuf);
        if(rec!=PVIB_OK)
        {
            LOG(WARNING)<<"get card data not sucess";
        }



        for (int pos = 0; pos < dataBufLen; pos += 2 * dataInfo->ChNum) {
                //顺序取出8个通道数据
                for (int ch = 0; ch < 8; ch++) {
                    if(!markSet[ch])
                    {
                        continue;
                    }
                    short v = 0;
                    //假设windows系统小端字节顺序处理数据
                    unsigned char buf[2] = { dataBuf[pos + ch * 2 + 1],dataBuf[pos + ch * 2] };
                    memcpy(&v, buf, 2);
                    //通道ch的物理量值为vCh
                    double vCh = v*dataInfo->DeltaY[ch];
                    // printf("%lf\n", vCh);
                    //处理物理量值vCh

                    sampledBuffer[ch].push_back(QPointF{static_cast<qreal>(pos/16)*dataInfo->DeltaX,vCh});
                    //...

                }
            }
        qDebug()<<"channel one sampled data size:"<<sampledBuffer[0].size();
        delete[] dataBuf;
        sampleStatus=-2;

    }
    LOG(INFO)<<"sample data finished";
}


void PVIBApi::dataProcessThread()
{
    qDebug()<<"current channel count:"<<currentDevice.channelCount;
    while(true&&isContinuingRealTime)
    {
        //if(lockFreeBuffer->size_approx()<MAXIMUM_BUFFER_POINT_COUNT)
        //{
          //  std::this_thread::sleep_for(std::chrono::seconds(3));
            //continue;
       // }
        //qDebug()<<"lockFreeBuffer size:"<<lockFreeBuffer->size_approx();

        //std::lock_guard<std::mutex> gurad(this->readMutex);
        //qDebug()<<"raw data size{"<<0<<"] "<<rawData[0];

        for(int i=0;i<MAXIMUM_BUFFER_POINT_COUNT;i++)
        {

            DataPack pack;
            auto rec=lockFreeBuffer->try_dequeue(pack);
           // qDebug()<<"try dequeue result:"<<rec;
            if(!rec)
            {
                continue;
            }
            for(int j=0;j<this->currentDevice.channelCount;++j)
            {
                if(!markSet[j])
                {
                    continue;
                }
                if(i<3000)
                {
                  auto val=pack.toPoint(j).y();
                  rawData[j].push_back(QPointF{xCordMap[i],val});
                }
                else
                {
                    break;
                }

                if(rawData[j].size()==3000)
                {
                  //  std::lock_guard<std::mutex> gurad(this->readMutex);
                    rawData_temp[j]=std::move(rawData[j]);
                }

                if(rawData[j].size()>2*MAXIMUM_BUFFER_POINT_COUNT+1)
                {
//                    rawData[j].clear();
//                    rawData_temp[j]=rawData[j];
                    rawData[j].remove(0, 3000);
                }
                //qDebug()<<"raw data size{"<<i<<"] "<<rawData[j].size();

            }
            //unsafe require further test;
            //why not test?
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    }
    LOG(INFO)<<"data process thread exited successfully";  
}


QPointF PVIBApi::DataPack::toPoint(int channel) const
{
    return QPointF{xValue[channel],yValue[channel]};
}

std::shared_ptr<QVector<QPointF>> PVIBApi::getRawData(int channel)
{
    auto tmp=std::make_shared<QVector<QPointF>>(std::move(rawData_temp[channel]));
    rawData_temp[channel].clear();
    return tmp;
}

void PVIBApi::setACQ_LEN(uint length,int ch)
{
    paras[ch]->AcqLen=length;
}

void PVIBApi::setACQ_RATE(int rate,int ch)
{

    paras[ch]->AcqClkIdx=rate;
}

void PVIBApi::setDelay_len(uint length,int ch)
{

    paras[ch]->DlyLen=length;
}

void PVIBApi::setCH_INPUT_TYPE(bool arg,int ch)
{
     if(!markSet[ch])return;
    chParas[ch]->ChInputType=arg;
}

void PVIBApi::setCH_FUNC_TYPE(int stall,int ch)
{
    if(!markSet[ch])return;
    chParas[ch]->ChFuncType=stall;
}

void PVIBApi::setCH_Sensitivity(double val,int ch)
{
     if(!markSet[ch])return;
     chParas[ch]->Sensitivity=val;

}
void PVIBApi::setTrig_Level(double val,int ch)
{
    if(!markSet[ch])return;
    chParas[ch]->TrgLv0=val;
    LOG(WARNING)<<"failed setting trig level argument wrong type...";
}

void PVIBApi::setRT_EN(bool arg)
{
    if(arg&&isContinuingRealTime==false)
    {
        startRTAcquisition();
    }
    isContinuingRealTime=arg;
}

bool PVIBApi::stopAcquisition()
{

    isContinuingRealTime=false;

    return true;
}

void PVIBApi::stopRealAcqusition(int channel)
{
    this->isSampling=false;
}

void PVIBApi::updataCardParameters()
{
    this->setArgumentFlag(false,0);
}


