#include "sysbackend.hpp"

#include "3rdparty/ApiPvib2_0.h"
#include "math_util.h"
#include "qdebug.h"
#include "qthread.h"
#include "qtimer.h"
#include"srs.h"
#include<QFile>
#include <memory>
#include"pvibapi.hpp"
#include<glog/logging.h>
#include<QTime>

SYSBackEnd::SYSBackEnd(QObject *parent)
    : QObject{parent}
{

}

void SYSBackEnd::init()
{
    qDebug()<<"currentThread id backend:"<<this->thread()->currentThreadId();
    backend=std::make_shared<SRS>();;
    device=std::make_shared<PVIBApi>();

    for(int i=0;i<4;++i)
    {
        channelLists[i]=std::make_shared<QVector<QPointF>>();
    }

    sendOutTimer=new QTimer();
    sendOutTimer->setInterval(70);

    connect(sendOutTimer,&QTimer::timeout,this,[=]()
    {
       if(device->getSampleStatue()==-2)
       {
           qDebug()<<"data sampling finished";
           processSamplingData(0);
           device->setSampleStatue(-1);
       }
       for(int i=0;i<4;++i)
       {
           int size=device->getChRTDataSize(i);
           if(size>=3000)
           {

               auto data=device->getRawData(i);
               bool isFrist=true;
               int preSize=0;
               if(channelLists[i]->size()!=0)
               {
                   isFrist=false;
                   preSize=channelLists[i]->size();
               }

               for(int j=0;j<data->size();++j)
               {

                   channelLists[i]->push_back((*data)[j]);

               }
                double rank=channelLists[i]==0?3000:channelLists[i]->size();
                double dx=this->xAxisBound/rank;
                if(!isFrist)
                {
                   for(int j=0;j<data->size();++j)
                   {

                       channelLists[i]->pop_front();
                   }
                }

               double x=0;
               for(auto& p:*(channelLists[i]))
               {
                   p.setX(x);
                   x+=dx;
               }

               emit this->RTDataReadyForAcquire(i);
           }
       }
      //  qDebug()<<"channel "<<0<<"size:"<<size;
    },Qt::DirectConnection);

    sendOutTimer->start();

}

 SYSBackEnd::~SYSBackEnd()
{

   sendOutTimer->stop();
   delete sendOutTimer;
   qDebug()<<"send out timer destoryed";
}

 void SYSBackEnd::connectToHost(QHostAddress host)
 {
     auto rec=device->connectToHost(host,0);
     emit acqusitionStarted(rec);
     if(rec)
     {
         sendOutTimer->start();
     }


 }

 void SYSBackEnd::reCalculateSRSResult()
 {
     if(lastSequence==nullptr)
     {
         LOG(INFO)<<"last sequence is nullptr returning...";
         return;
     }
     calculateSRSResult(lastSequence);
 }


 void SYSBackEnd::calculateSRSResult(std::shared_ptr<QVector<QPointF>> sequence)
 {
     auto argumentMap=emit requestSRSArguments();
     if(argumentMap.empty())
     {
         LOG(WARNING)<<"argument map empty";
     }

     this->backend->set_Q(argumentMap["qvalue"].toDouble());
     this->backend->set_c(argumentMap["dratio"].toDouble());
     this->backend->set_srs_type(argumentMap["shockType"].toInt());

     this->backend->set_ref_freq(argumentMap["refFrequency"].toDouble());
     this->backend->set_freq(argumentMap["min_fre"].toDouble(),
             argumentMap["max_fre"].toDouble(),argumentMap["oct_step"].toDouble());

     bool open;
     if(argumentMap["ATOP"]=="true")
     {
        open=true;
     }
     else
     {
         open=false;
         this->backend->set_bias(argumentMap["outlier"].toDouble());
     }
     this->backend->set_biasmode(open);

     std::vector<double> xCord,yCord;
     for(auto& i:*sequence)
     {
         xCord.emplace_back(i.rx());
         yCord.emplace_back(i.ry());
     }

     time_signal arg{0};
     arg.N=sequence->size();
     arg.time=xCord;
     arg.accel=yCord;

     auto targetSequence=emit requesetTargetSequence();
     std::vector<targetpoint> refs;
     if(targetSequence.empty())
     {
         LOG(WARNING)<<"requestTargetSequence FAILED";
     }
     else
     {
          for(auto&& i:targetSequence)
          {
              refs.push_back(i.toTargetPoint());
          }
     }

     this->backend->input_target_srs(refs);
     this->backend->import_t(arg);
     this->backend->update_timesignal();
     this->backend->srs_update();

     auto& result=this->backend->get_srs_result();

     QVector<QPointF> tmp;
     for(int i=0;i<result.accel.size();++i)
     {
         tmp.emplace_back(QPointF{result.freq[i],result.accel[i]});
     }

     this->accerlateSequence=tmp;
     qDebug()<<"tmp size:"<<tmp.size()<<" accelerateSequence size:"<<this->accerlateSequence.size();
     emit dataProcessFinished(1);

     SrsStateBar bar;
     auto srsBar=this->backend->srs_statebar();
     bar.setDampRate(srsBar.damp);
     bar.setMaxFre(srsBar.max.x);
     bar.setExceed(srsBar.exceed);
     bar.setMaxVal(srsBar.max.y);
     bar.setMinFre(srsBar.min.x);
     bar.setMinVal(srsBar.min.y);


     emit sendStateBar(bar);
 }


 void SYSBackEnd::processSamplingData(int ch)
 {
     sendOutTimer->stop();
     auto sequence=std::make_shared
             <QVector<QPointF>>(std::move(device->getSampledData()));

     isAcqusitionStarted=false;

     emit sendSampledRawData(sequence,ch);

     calculateSRSResult(sequence);

     lastSequence=sequence;



 }

 void SYSBackEnd::startRTAcqusition()
 {
     if(isAcqusitionStarted)
     {
         return;
     }
     int rec=device->startRTAcquisition();
     LOG(INFO)<<"RT acqusition start result:"<<rec;
     if(rec<0)
     {
         return;
     }
     isAcqusitionStarted=true;
     connect(this,&SYSBackEnd::requesRTtAcqusitionStop,this,[=]()
     {
         device->setRT_EN(false);
         isAcqusitionStarted=false;
     });
     sendOutTimer->start();
 }

 void SYSBackEnd::startAcqusition()
 {
      device->startRealAcqusition();

 }


QVector<QPointF> SYSBackEnd::getAccelerateSequence()
{
    return accerlateSequence;
}

QVector<QPointF> SYSBackEnd::getTimeZoneSequence()
{
    return timeZoneSequence;
}

void SYSBackEnd::stopAcqusition()
{
   device->stopRealAcqusition();
}

void SYSBackEnd::stopRTAcqusition()
{
    auto rec=emit requesRTtAcqusitionStop();
    if(rec)
    {
        LOG(INFO)<<"rtacqusition top success";
    }
    else
    {
         LOG(INFO)<<"rtacqusition top failed";
    }
}

void SYSBackEnd::updateArgument(ArgType type,const QString& newer,int ch)
{
    if(!device->isBackEndReady())
        {
            LOG(INFO)<<"DEVICE NOT READY  unable to set argument";
            return;
        }
    device->setArgumentFlag(true,ch);
    if(type==ArgType::ACQ_LEN)
    {
      device->setACQ_LEN(newer.toUInt(),ch);
    }
    else if(type==ArgType::ACQ_RATE)
    {
        device->setACQ_RATE(newer.toInt(),ch);
    }else if(type==ArgType::DLY_LEN)
    {
        device->setDelay_len(newer.toUInt(),ch);
    }else if(type==ArgType::CH_FUNC_TYPE)
    {
        device->setCH_FUNC_TYPE(newer.toInt(),ch);
    }else if(type==ArgType::CH_SENSITIVITY)
    {
        device->setCH_Sensitivity(newer.toDouble(),ch);
    }else if(type==ArgType::CH_INPUT_TYPE)
    {
        device->setCH_INPUT_TYPE(newer.toInt(),ch);
    }else if(type==ArgType::TRIG_LEVEL)
    {
        device->setTrig_Level(newer.toDouble(),ch);
    }

}
void SYSBackEnd::readAccelerateFromFile(const QString& path)
{
    QFile inputFile{path};
    if(!inputFile.exists())
    {
        qDebug()<<"file invaild.";
        return;
    }

    inputFile.open(QIODevice::ReadOnly);

    QString l;
    while(!inputFile.atEnd())
    {
        l=inputFile.readLine();
        auto list=l.split(" ",Qt::SkipEmptyParts);
        QPointF f{list[0].toDouble(),list[1].toDouble()};
        this->accerlateSequence.push_back(f);
    }

    emit dataProcessFinished(1);


}

void SYSBackEnd::readTimeZoneFromFile(const QString& path)
{
    QFile inputFile{path};
    if(!inputFile.exists())
    {
        qDebug()<<"file invaild.";
        return;
    }

    inputFile.open(QIODevice::ReadOnly);

    QString l;
    while(!inputFile.atEnd())
    {
        l=inputFile.readLine();
        auto list=l.split(" ",Qt::SkipEmptyParts);
        QPointF f{list[0].toDouble(),list[1].toDouble()};
        this->timeZoneSequence.push_back(f);
    }

    emit dataProcessFinished(2);
}

SYSBackEnd::mDeviceInfo::mDeviceInfo(const DeviceInfo& ano)
{
    this->Id=ano.Id;
    for(int i=0;i<4;++i)
    {
        this->Ip[i]=ano.Ip[i];
    }
    this->Type=ano.Type;
}



SYSBackEnd::DeviceInfoList_t SYSBackEnd::getDeviceList()
{
    auto dList=this->device->getDeviceInfo();
    SYSBackEnd::DeviceInfoList_t rec;
    for(auto&&d:dList)
    {
        rec.push_back(std::make_shared<mDeviceInfo>(*d));
    }
    return rec;
}

bool SYSBackEnd::stopGetingRawData()
{
   return this->device->stopAcquisition();
}

std::shared_ptr<QVector<QPointF>> SYSBackEnd::getRawData(int channel)
{
    return this->channelLists[channel];
}
















