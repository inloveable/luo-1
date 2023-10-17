#include "datamanager.h"
#include "experimentdocument.hpp"
#include <chrono>
#include<glog/logging.h>

#include "qobjectdefs.h"
#include"sysbackend.hpp"

#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QThread>
#include<QApplication>
#include<QTimer>
#include<QFileDialog>
#include<QMessageBox>
#include<QInputDialog>
#include <memory>
#include "math_util.h"
SINGLETON_IMPLEMENT(DataManager)


#define TEST 0

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{  


}
void DataManager::realDestroy()
{
    if(mapToChannelInfo)
    {
     delete mapToChannelInfo;
    qDebug()<<"destroyed datamanager";
    }
    ;
    if(mapToDisplaySettingInfo)
    {
        delete mapToDisplaySettingInfo;
    }
    if(mapToExSettingInfo)
     delete mapToExSettingInfo;
    if(currentDocument)
        currentDocument->deleteLater();

    connect(dataThread,&QThread::finished,backEnd,&QObject::deleteLater);
    dataThread->quit();
    dataThread->wait();
    //delete backEnd;
    //delete dataThread;
    dataThread->deleteLater();
    if(m_pInstance)
     delete m_pInstance;

}
void DataManager::init()
{
    ChannelArgTemplateFilePath=ChannelArgFile=QApplication::applicationDirPath()
            +"/appCaches/defaults/defaultChannelArg.json";
    ExConfiTemplateFilePath=ExConfigFile=QApplication::applicationDirPath()
            +"/appCaches/defaults/defaultExSettings.json";
    appCachePath=QApplication::applicationDirPath()+"/appCaches";

    unitMap["m"]="1";
    unitMap["cm"]="0.01";
    unitMap["mm"]="0.001";
    unitMap["Ft"]="0.3048";
    unitMap["ln"]="0.0254";
    unitMap["m/s"]="1";
    unitMap["cm/s"]="0.01";
    unitMap["mm/s"]="0.001";
    unitMap["Ft/s"]="0.3048";
    unitMap["ln/s"]="0.0254";
    unitMap["g"]="9.8";
    unitMap["m/s^2"]="1";
    unitMap["cm/s^2"]="0.01";
    unitMap["mm/s^2"]="0.001";
    unitMap["Ft/s^2"]="0.3048";
    unitMap["ln/s^2"]="0.0254";
    unitMap["N"]="1";
    unitMap["LBF"]="4.44822";
    unitMap["kg"]="1";
    unitMap["lbs"]="0.4536";
    unitMap["Gram"]="0.001";
    unitMap["Ounce"]="0.02835";
    unitMap["Ton"]="1000";
    unitMap["%"]="...";
    unitMap["ratio"]="...";
    unitMap["db"]="...";
    unitMap["V"]="1";
    unitMap["mV"]="0.001";

    timeZoneArgs["pulseType"]="半正弦波";
    timeZoneArgs["standardType"]="IEC";
    timeZoneArgs["shockMachineType"]="振动机";
    timeZoneArgs["upLimit"]="20.00";
    timeZoneArgs["lowLimit"]="20.00";
    timeZoneArgs["acceleratePeek"]="3000";
    timeZoneArgs["pulseWidth"]="2844";

    backEnd=new SYSBackEnd;
    dataThread=new QThread;

    connect(backEnd,&SYSBackEnd::acqusitionStarted,
            this,&DataManager::acqusitionStarted);
    connect(backEnd,&SYSBackEnd::requestSRSArguments,
            this,&DataManager::getSRSArguments,Qt::BlockingQueuedConnection);
    connect(backEnd,&SYSBackEnd::sendSampledRawData,
            this,&DataManager::onSampledRawDataSent);
    connect(this,&DataManager::updateBackEndArgument,backEnd,&SYSBackEnd::updateArgument);
    connect(this,&DataManager::requestRawData,backEnd,
            &SYSBackEnd::getRawData,Qt::BlockingQueuedConnection);
    connect(backEnd,&SYSBackEnd::requesetTargetSequence,
            this,&DataManager::getRefPoints,Qt::BlockingQueuedConnection);


    dataThread->start();
    backEnd->moveToThread(dataThread);
    

    QMetaObject::invokeMethod(backEnd,"init");
    //backEnd->init();
    qDebug()<<"dataManager thread id:"<<this->thread()->currentThreadId();
    qDebug()<<"back End id:"<<backEnd->thread()->currentThreadId();
    qDebug()<<"datathread id:"<<dataThread->currentThreadId();



   #if TEST


    ExperimentDocument document;
    QVector<QPointF> vec{QPointF{4,5},QPointF{5,6},QPointF{6,7}};
    document.addprocessedDataToBuffer(vec);
    auto str=document.experimentName="test";
    //str=document.saveToLocalFile("./appCaches");

    document.exportProcessedData("./");

   #endif
}

void DataManager::acqusitionStarted(bool ok)
{
    if(ok)
    {
       //

        connect(backEnd,&SYSBackEnd::RTDataReadyForAcquire,this,[=](int ch)
        {
            qDebug()<<"data manager::getting raw data";
            auto rec=requestRawData(ch);
            qDebug()<<"data manager stop getting raw data";
            emit sendRTData(2,rec);

        });
    }
    else
    {
        //stop...
    }
}

QVector<QVector<QString>>& DataManager::getChannelInfo(const QString& index)
{
    if(mapToChannelInfo==nullptr)
    {
        auto before=std::chrono::high_resolution_clock::now();
        mapToChannelInfo=new QMap<QString,QVector<QVector<QString>>>;
        readFromArgFile();
        auto after=std::chrono::high_resolution_clock::now();

        LOG(INFO)<<"time of first load channelInfo:"<<std::chrono::duration_cast
                   <std::chrono::nanoseconds>(after-before).count();
    }

    if(mapToChannelInfo->find(index)==mapToChannelInfo->end())
    {
        return forRec;
    }

    return mapToChannelInfo->find(index).value();
}

void DataManager::readFromArgFile()
{
    QFile channelConfigFile{ChannelArgFile};
    channelConfigFile.open(QIODevice::ReadOnly);

    QByteArray arr=channelConfigFile.readAll();
    QJsonDocument doc=QJsonDocument::fromJson(arr);

    QJsonObject l0=doc.object();//level0

    QJsonObject l1=l0.begin()->toObject();

    auto iter=l1.find("InputChannel");
    mapToChannelInfo->insert("InputChannel",QVector<QVector<QString>>());
    QJsonArray jsArray=iter->toArray();
    for(auto&& i:jsArray)
    {
        QJsonObject l2=i.toObject();
        QVector<QString> vec;
        vec.push_back(l2.value("Type").toString());
        vec.push_back(l2.value("CombineWay").toString());
        vec.push_back(l2.value("SensorType").toString());
        vec.push_back(l2.value("Value").toString());
        vec.push_back(l2.value("Unit").toString());
        (*mapToChannelInfo)["InputChannel"].push_back(vec);
    }

    iter=l1.find("Flur");
    mapToChannelInfo->insert("Flur",QVector<QVector<QString>>());
    jsArray=iter->toArray();
    for(auto&& i:jsArray)
    {
        QJsonObject l2=i.toObject();
        QVector<QString> vec;
        vec.push_back(l2.value("state").toString());
        vec.push_back(l2.value("stopFrequency").toString());
        vec.push_back(l2.value("OTBAttenuation").toString());
        vec.push_back(l2.value("method").toString());
        vec.push_back(l2.value("rank").toString());
        vec.push_back(l2.value("flurRate").toString());
        (*mapToChannelInfo)["Flur"].push_back(vec);
    }

    iter=l1.find("dampingRatio");
    mapToChannelInfo->insert("dampingRatio",QVector<QVector<QString>>());
    jsArray=iter->toArray();
    for(auto&& i:jsArray)
    {
        QJsonObject l2=i.toObject();
        QVector<QString> vec;
        vec.push_back(l2.value("dratio").toString());
        vec.push_back(l2.value("qvalue").toString());
        (*mapToChannelInfo)["dampingRatio"].push_back(vec);

    }

    iter=l1.find("callibrate");
    mapToChannelInfo->insert("callibrate",QVector<QVector<QString>>());
    jsArray=iter->toArray();
    for(auto&& i:jsArray)
    {
        QJsonObject l2=i.toObject();
        QVector<QString> vec;
        vec.push_back(l2.value("ATOP").toString());
        vec.push_back(l2.value("outlier").toString());
        (*mapToChannelInfo)["callibrate"].push_back(vec);
    }

    iter=l1.find("projectUnit");
    mapToChannelInfo->insert("projectUnit",QVector<QVector<QString>>());
    jsArray=iter->toArray();
    for(auto&& i:jsArray)
    {
        QJsonObject l2=i.toObject();
        QVector<QString> vec;
        vec.push_back(l2.value("Type").toString());
        vec.push_back(l2.value("ShowUnit").toString());
        vec.push_back(l2.value("TransformFactor").toString());
        (*mapToChannelInfo)["projectUnit"].push_back(vec);
    }




}


bool DataManager::setChannelInfoFile(const QString& filePath)
{
    QFile file{filePath};

    if(!file.open(QIODevice::ReadOnly))
    {
        LOG(WARNING)<<"can't open file,file path:"<<filePath.toStdString();
        return false;
    }
    QJsonParseError err;
    QJsonDocument::fromJson(QByteArray{file.readAll()},&err);
    if(!(err.error==QJsonParseError::NoError))
    {
        LOG(WARNING)<<"error when setChannel json:"<<
                      err.errorString().toStdString()<<"file path:"<<filePath.toStdString();
        return false;
    }

    file.close();
    this->ChannelArgFile=filePath;
    if(mapToChannelInfo)
    {
        delete mapToChannelInfo;
        mapToChannelInfo=nullptr;
    }

    return true;
}

void DataManager::changeChannelValue(const QString& table,
                                     int index,const QString& v1,
                                     const QString& v2)
{
    if(table=="projectUnit")
    {
        auto& t=mapToChannelInfo->find(table).value();
        auto& t1=t[index];
        t1[1]=v1;
        t1[2]=v2;
        emit changeProjectUnit(t1[0],t1[1]);
    }
    else if(table=="callibrate"||table=="dampingRatio")
    {
        auto& t=mapToChannelInfo->find(table).value();
        auto& t1=t[index];
        t1[0]=v1;
        t1[1]=v2;
    }
}

void DataManager::changeExArgValueEx(const QString& table,int index,
                                     const QList<QString>& arg)
{
  if(table=="ShockReplayChartSettings")
  {

      (*mapToExSettingInfo)["pulseType"][0]=arg[0];
      (*mapToExSettingInfo)["standardType"][0]=arg[1];
      (*mapToExSettingInfo)["shockMachineType"][0]=arg[2];
      (*mapToExSettingInfo)["upLimit"][0]=arg[3];
      (*mapToExSettingInfo)["lowLimit"][0]=arg[4];
      (*mapToExSettingInfo)["acceleratePeek"][0]=arg[5];
      (*mapToExSettingInfo)["pulseWidth"][0]=arg[6];
      //warning:all orders must obey to the default json order:./defaults/defaultExSettings.json;
      //so as others;
      return;
  }
  else if(table=="ShockReplayExSettings")
  {
      (*mapToExSettingInfo)["ShockReplayExSettings"][0]=arg[0];
      (*mapToExSettingInfo)["ShockReplayExSettings"][1]=arg[1];
      (*mapToExSettingInfo)["ShockReplayExSettings"][2]=arg[2];
      (*mapToExSettingInfo)["ShockReplayExSettings"][3]=arg[3];
      (*mapToExSettingInfo)["ShockReplayExSettings"][4]=arg[4];
      (*mapToExSettingInfo)["ShockReplayExSettings"][5]=arg[5];
      (*mapToExSettingInfo)["ShockReplayExSettings"][6]=arg[6];
      (*mapToExSettingInfo)["ShockReplayExSettings"][7]=arg[7];
      (*mapToExSettingInfo)["ShockReplayExSettings"][8]=arg[8];
  }
  else if(table=="ExperimentSettings") //index = 1 regulur
  {
      if(index==1)
      {
          (*mapToExSettingInfo)["FramePointCount"][0]=arg[0];
          (*mapToExSettingInfo)["SampleFrequency"][0]=arg[1];
          (*mapToExSettingInfo)["ShockTime"][0]=arg[2];
          (*mapToExSettingInfo)["AllWaveWidth"][0]=arg[3];
          (*mapToExSettingInfo)["MainWaveWidth"][0]=arg[4];
          (*mapToExSettingInfo)["PeekSearchRangeMin"][0]=arg[5];
          (*mapToExSettingInfo)["PeekSearchRangeMax"][0]=arg[6];
          (*mapToExSettingInfo)["TimeTrigger"][0]=arg[7];
      }
      else if(index==2)
      {
          (*mapToExSettingInfo)["AllInput"][0]=arg[0];
          (*mapToExSettingInfo)["AllInput"][1]=arg[1];
          (*mapToExSettingInfo)["AllInput"][2]=arg[2];

          (*mapToExSettingInfo)["FirstInput"][0]=arg[3];
          (*mapToExSettingInfo)["FirstInput"][1]=arg[4];
          (*mapToExSettingInfo)["FirstInput"][2]=arg[5];

          (*mapToExSettingInfo)["SecondInput"][0]=arg[6];
          (*mapToExSettingInfo)["SecondInput"][1]=arg[7];
          (*mapToExSettingInfo)["SecondInput"][2]=arg[8];

          (*mapToExSettingInfo)["ThirdInput"][0]=arg[9];
          (*mapToExSettingInfo)["ThirdInput"][1]=arg[10];
          (*mapToExSettingInfo)["ThirdInput"][2]=arg[11];

          (*mapToExSettingInfo)["FouthInput"][0]=arg[12];
          (*mapToExSettingInfo)["FouthInput"][1]=arg[13];
          (*mapToExSettingInfo)["FouthInput"][2]=arg[14];
      }
      /*else if(index==2)
      {

      }*/
  }
}

void DataManager::changeChannelValueEx(const QString& table,int index,
                                       const QList<QString>& arg)
{

    auto& t=mapToChannelInfo->find(table).value();
    auto& t1=t[index];

    for(int i=0;i<arg.size();i++)
    {
        t1[i]=arg[i];
    }


}

void DataManager::saveChannelInfoFile(const QString& fileName)
{

    QFile channelConfigFile{ChannelArgFile};
    channelConfigFile.open(QIODevice::ReadOnly);

    QByteArray arr=channelConfigFile.readAll();
    QJsonDocument doc=QJsonDocument::fromJson(arr);

    QJsonObject l0=doc.object();//level0
    QJsonObject l1=l0.begin()->toObject();

    auto iter=l1.find("InputChannel");
    auto mapIter=mapToChannelInfo->find("InputChannel");
    QJsonArray jsArray=iter->toArray();

    for(int i=0;i<4;i++)
    {
        QJsonObject l2=jsArray[i].toObject();
        l2["Type"]=mapIter.value()[i][0];
        l2["CombineWay"]=mapIter.value()[i][1];
        l2["SensorType"]=mapIter.value()[i][2];
        l2["Value"]=mapIter.value()[i][3];
        l2["Unit"]=mapIter.value()[i][4];
        jsArray[i] = l2;
    }
    *iter=jsArray;

    iter=l1.find("Flur");
    mapIter=mapToChannelInfo->find("Flur");
    jsArray=iter->toArray();
    for(int i=0;i<4;i++)
    {
       QJsonObject l2=jsArray[i].toObject();
       l2["state"]=mapIter.value()[i][0];
       l2["stopFrequency"]=mapIter.value()[i][1];
       l2["OTBAttenuation"]=mapIter.value()[i][2];
       l2["method"]=mapIter.value()[i][3];
       l2["rank"]=mapIter.value()[i][4];
       l2["flurRate"]=mapIter.value()[i][5];
       jsArray[i] = l2;

    }
    *iter=jsArray;

    iter=l1.find("projectUnit");
    mapIter=mapToChannelInfo->find("projectUnit");
    jsArray=iter->toArray();
    for(int i=0;i<4;i++)
    {
       QJsonObject l2=jsArray[i].toObject();
       l2["Type"]=mapIter.value()[i][0];
       l2["ShowUnit"]=mapIter.value()[i][1];
       l2["TransformFactor"]=mapIter.value()[i][2];
       jsArray[i] = l2;
    }
    *iter=jsArray;

    iter=l1.find("callibrate");
    mapIter=mapToChannelInfo->find("callibrate");
    jsArray=iter->toArray();
    for(int i=0;i<4;i++)
    {
       QJsonObject l2=jsArray[i].toObject();
       l2["ATOP"]=mapIter.value()[i][0];
       l2["outlier"]=mapIter.value()[i][1];
       jsArray[i] = l2;
    }
    *iter=jsArray;

    iter=l1.find("dampingRatio");
    mapIter=mapToChannelInfo->find("dampingRatio");
    jsArray=iter->toArray();
    for(int i=0;i<4;i++)
    {
       QJsonObject l2=jsArray[i].toObject();
       l2["dratio"]=mapIter.value()[i][0];
       l2["qvalue"]=mapIter.value()[i][1];
       jsArray[i] = l2;
    }
    *iter=jsArray;





    l0["ChannelArguments"]=l1;
    doc.setObject(l0);
    auto bits=doc.toJson();


    QFile jsFile{fileName};


    jsFile.open(QIODevice::WriteOnly);
    jsFile.write(bits);
    jsFile.close();
}

void DataManager::saveExSettingFile(const QString& fileName)
{
    LOG(INFO)<<"saving Ex arg file:"<<fileName.toStdString()<<"...";
    QFile channelConfigFile{ExConfiTemplateFilePath};
    channelConfigFile.open(QIODevice::ReadOnly);

    QByteArray arr=channelConfigFile.readAll();
    QJsonDocument doc=QJsonDocument::fromJson(arr);

    QJsonObject l0=doc.object();//level0
    //WTF am I writing??
    QJsonObject l1=l0["ExperimentSettings"].toObject();

    for(auto iter=l1.begin();iter!=l1.end();++iter)
    {
        if(iter->isDouble())
        {
            iter.value()=(*mapToExSettingInfo)[iter.key()][0];
        }
        else if(iter->isArray())
        {
            QJsonArray arr=iter->toArray();
            for(int j=0;j<iter->toArray().size();++j)
            {
                arr[j]=(*mapToExSettingInfo)[iter.key()][j];
            }
            iter.value()=arr;
        }
    }

    l0["ExperimentSettings"]=l1;


    l1=l0["ChartsSetting"].toObject();
    QJsonArray disObject=l1["DisplaySettings"].toArray();

    for(int i=0;i<4;++i)
    {
       auto obj=disObject[i].toObject();
       obj["Selected"]=(*mapToDisplaySettingInfo)["DisplaySettings"][i][0];
       obj["Color"]=(*mapToDisplaySettingInfo)["DisplaySettings"][i][1];
       obj["Width"]=(*mapToDisplaySettingInfo)["DisplaySettings"][i][2];
       obj["Type"]=(*mapToDisplaySettingInfo)["DisplaySettings"][i][3];
       disObject[i]=obj;
    }
    l1["DisplaySettings"]=disObject;
    l0["ChartsSetting"]=l1;


    l1=l0.find("ShockReplayChartSettings")->toObject();

    l1["pulseType"]=(*mapToExSettingInfo)["pulseType"][0];
    l1["standardType"]=(*mapToExSettingInfo)["standardType"][0];
    l1["shockMachineType"]=(*mapToExSettingInfo)["shockMachineType"][0];
    l1["upLimit"]=(*mapToExSettingInfo)["upLimit"][0];
    l1["lowLimit"]=(*mapToExSettingInfo)["lowLimit"][0];
    l1["acceleratePeek"]=(*mapToExSettingInfo)["acceleratePeek"][0];
    l1["pulseWidth"]=(*mapToExSettingInfo)["pulseWidth"][0];

    l0["ShockReplayChartSettings"]=l1;

    l1=l0.find("ShockReplayExSettings")->toObject();

    l1["refFrequency"]=(*mapToExSettingInfo)["ShockReplayExSettings"][0];
    l1["min_fre"]=(*mapToExSettingInfo)["ShockReplayExSettings"][1];
    l1["max_fre"]=(*mapToExSettingInfo)["ShockReplayExSettings"][2];
    l1["oct_step"]=(*mapToExSettingInfo)["ShockReplayExSettings"][3];
    l1["shockType"]=(*mapToExSettingInfo)["ShockReplayExSettings"][4];
    l1["samplingTime"]=(*mapToExSettingInfo)["ShockReplayExSettings"][5];
    l1["shockTime"]=(*mapToExSettingInfo)["ShockReplayExSettings"][6];
    l1["sampleFrequency"]=(*mapToExSettingInfo)["ShockReplayExSettings"][7];
    l1["restoreTime"]=(*mapToExSettingInfo)["ShockReplayExSettings"][8];

    l0["ShockReplayExSettings"]=l1;


    doc.setObject(l0);
    auto bits=doc.toJson();

    QFile jsFile{fileName};


    jsFile.open(QIODevice::WriteOnly);
    jsFile.write(bits);
    jsFile.close();



    //further to go...
    //call backEnd;


}


QVector<QVector<QString>>& DataManager::getDisplayInfo(const QString& index)
{
    if(mapToExSettingInfo==nullptr)
    {
        auto before=std::chrono::high_resolution_clock::now();
        mapToDisplaySettingInfo=new QMap<QString,QVector<QVector<QString>>>;
        mapToExSettingInfo=new QMap<QString,QVector<QString>>;
        readFromExSettingFile();
        auto after=std::chrono::high_resolution_clock::now();

        LOG(INFO)<<"time of first load ExInfo:"<<std::chrono::duration_cast
                   <std::chrono::nanoseconds>(after-before).count();
    }

    if(mapToDisplaySettingInfo->find(index)==mapToDisplaySettingInfo->end())
    {
        return forRec;
    }

    return mapToDisplaySettingInfo->find(index).value();
}

QVector<QString>& DataManager::getExSettingInfo(const QString& index)
{
    if(mapToExSettingInfo==nullptr)
    {
        auto before=std::chrono::high_resolution_clock::now();
        mapToDisplaySettingInfo=new QMap<QString,QVector<QVector<QString>>>;
        mapToExSettingInfo=new QMap<QString,QVector<QString>>;
        readFromExSettingFile();
        auto after=std::chrono::high_resolution_clock::now();

        LOG(INFO)<<"time of first load ExInfo:"<<std::chrono::duration_cast
                   <std::chrono::nanoseconds>(after-before).count();
    }

    if(mapToExSettingInfo->find(index)==mapToExSettingInfo->end())
    {
        return forRec1;
    }

    return mapToExSettingInfo->find(index).value();
}

void DataManager::readFromExSettingFile()
{
    QFile channelConfigFile{ExConfigFile};
    channelConfigFile.open(QIODevice::ReadOnly);

    QByteArray arr=channelConfigFile.readAll();
    QJsonDocument doc=QJsonDocument::fromJson(arr);

        QJsonObject l0=doc.object();//level0

        QJsonObject l1=l0["ChartsSetting"].toObject();

        QJsonArray displayObject=l1["DisplaySettings"].toArray();
        QJsonArray mouseObject=l1["MouseSettings"].toArray();

        mapToDisplaySettingInfo->insert("DisplaySettings",QVector<QVector<QString>>(4));
        for(int i=0;i<displayObject.size();++i)
        {
            QJsonObject obj=displayObject[i].toObject();
            (*mapToDisplaySettingInfo)["DisplaySettings"][i].push_back(obj["Selected"].toString());
            (*mapToDisplaySettingInfo)["DisplaySettings"][i].push_back(obj["Color"].toString());
            (*mapToDisplaySettingInfo)["DisplaySettings"][i].push_back(obj["Width"].toString());
            (*mapToDisplaySettingInfo)["DisplaySettings"][i].push_back(obj["Type"].toString());
        }

        l1=l0["ExperimentSettings"].toObject();

        for(auto begin=l1.begin();begin!=l1.end();++begin)
        {
            if(begin.value().isDouble())
            {
                QVector<QString> vec;
                vec.push_back(QString("%1")
                              .arg(begin.value().toDouble()));
                mapToExSettingInfo->insert(begin.key(),
                                          vec);
            }
            else if(begin.value().isArray())
            {
                auto iter=mapToExSettingInfo->insert(begin.key(),QVector<QString>{});
                auto array=begin.value().toArray();
                // Rate RubberTime RubberPoint;
                for(auto&& j:array)
                {
                    if(j.isDouble())
                    {
                        iter.value().push_back(QString("%1").arg(j.toDouble()));
                    }
                }

            }
        }

        //further to go...

        QJsonObject shockReplyObject=l0["ShockReplayChartSettings"].toObject();

        for(auto begin=shockReplyObject.begin();begin!=shockReplyObject.end();++begin)
        {
                QVector<QString> vec;
                vec.push_back(QString("%1")
                              .arg(begin.value().toString()));
                mapToExSettingInfo->insert(begin.key(),
                                          vec);
        }

        QJsonObject shockReplyExObject=l0["ShockReplayExSettings"].toObject();


        QVector<QString> vec;

        vec.push_back(shockReplyExObject["refFrequency"].toString());
        vec.push_back(shockReplyExObject[ "min_fre"].toString());
        vec.push_back(shockReplyExObject["max_fre"].toString());
        vec.push_back(shockReplyExObject["oct_step"].toString());
        vec.push_back(shockReplyExObject["shockType"].toString());
        vec.push_back(shockReplyExObject["samplingTime"].toString());
        vec.push_back(shockReplyExObject["shockTime"].toString());
        vec.push_back(shockReplyExObject["sampleFrequency"].toString());
        vec.push_back(shockReplyExObject["restoreTime"].toString());


        mapToExSettingInfo->insert("ShockReplayExSettings",
                                   vec);


}


void DataManager::updateExArgAtOnce(const QVector<QString>& values)
{
    if(values.size()!=30)
    {
        LOG(WARNING)<<"Ex Arg size no equals to 23,some where wrong...";
        return;
    }
    //specialized,be careful...
    (*mapToExSettingInfo)["FramePointCount"][0]=values[0];
    (*mapToExSettingInfo)["SampleFrequency"][0]=values[1];
    (*mapToExSettingInfo)["ShockTime"][0]=values[2];
    (*mapToExSettingInfo)["AllWaveWidth"][0]=values[3];
    (*mapToExSettingInfo)["MainWaveWidth"][0]=values[4];
    (*mapToExSettingInfo)["PeekSearchRangeMin"][0]=values[5];
    (*mapToExSettingInfo)["PeekSearchRangeMax"][0]=values[6];
    (*mapToExSettingInfo)["TimeTrigger"][0]=values[7];

    (*mapToExSettingInfo)["AllInput"][0]=values[8];
    (*mapToExSettingInfo)["AllInput"][1]=values[9];
    (*mapToExSettingInfo)["AllInput"][2]=values[10];

    (*mapToExSettingInfo)["FirstInput"][0]=values[11];
    (*mapToExSettingInfo)["FirstInput"][1]=values[12];
    (*mapToExSettingInfo)["FirstInput"][2]=values[13];

    (*mapToExSettingInfo)["SecondInput"][0]=values[14];
    (*mapToExSettingInfo)["SecondInput"][1]=values[15];
    (*mapToExSettingInfo)["SecondInput"][2]=values[16];

    (*mapToExSettingInfo)["ThirdInput"][0]=values[17];
    (*mapToExSettingInfo)["ThirdInput"][1]=values[18];
    (*mapToExSettingInfo)["ThirdInput"][2]=values[19];



    (*mapToExSettingInfo)["FouthInput"][0]=values[20];
    (*mapToExSettingInfo)["FouthInput"][1]=values[21];
    (*mapToExSettingInfo)["FouthInput"][2]=values[22];

    (*mapToExSettingInfo)["pulseType"][0]=values[23];
    (*mapToExSettingInfo)["standardType"][0]=values[24];
    (*mapToExSettingInfo)["shockMachineType"][0]=values[25];
    (*mapToExSettingInfo)["upLimit"][0]=values[26];
    (*mapToExSettingInfo)["lowLimit"][0]=values[27];
    (*mapToExSettingInfo)["acceleratePeek"][0]=values[28];
    (*mapToExSettingInfo)["pulseWidth"][0]=values[29];


}


bool DataManager::setExSettingFile(const QString& filePath)
{
    QFile file{filePath};

    if(!file.open(QIODevice::ReadOnly))
    {
        LOG(WARNING)<<"can't open file,file path:"<<filePath.toStdString();
        return false;
    }
    QJsonParseError err;
    QJsonDocument::fromJson(QByteArray{file.readAll()},&err);
    if(!(err.error==QJsonParseError::NoError))
    {
        LOG(WARNING)<<"error when setChannel json:"<<
                      err.errorString().toStdString()<<"file path:"<<filePath.toStdString();
        return false;
    }

    file.close();
    this->ChannelArgFile=filePath;
    if(mapToDisplaySettingInfo)
    {
        delete mapToExSettingInfo;
        delete mapToDisplaySettingInfo;
        mapToExSettingInfo=nullptr;
        mapToDisplaySettingInfo=nullptr;
    }

    return true;
}


void DataManager::changeDisplaySettingValue(const QString& table,
                                       int index,int index1,const QString& value)
{
    auto iter=mapToDisplaySettingInfo->find(table);
    iter.value()[index][index1]=value;
}

void DataManager::changeDisplaySettingValue(const QString& table,
                                       int index,const QList<QString>&r)
{
    auto iter=mapToDisplaySettingInfo->find(table);
    iter.value()[index]=r;
}

void DataManager::updateBackEndArguments()
{
   using aType=SYSBackEnd::ArgType;
   emit updateBackEndArgument(aType::Q_VALUE,(*mapToChannelInfo)["dampingRatio"][3][1]);
   emit updateBackEndArgument(aType::D_RATIO,(*mapToChannelInfo)["dampingRatio"][3][0]);

}



QMap<QString,QString> DataManager::getSRSArguments(int channel)
{
    QMap<QString,QString> rec;
    auto& channelArgs=this->getChannelInfo("dampingRatio");

    rec["dratio"]=channelArgs[channel][0];
    rec["qvalue"]=channelArgs[channel][1];

    channelArgs=this->getChannelInfo("callibrate");

    if(channelArgs[channel][0]=="自动")
    {
        rec["outlier"]="NULL";
        rec["ATOP"]="true";
    }
    else
    {
        rec["outlier"]=channelArgs[channel][1];
        rec["ATOP"]="false";
    }


    auto& exArgs=this->getExSettingInfo("ShockReplayExSettings");
    int srs_type;
    if(exArgs[4]=="绝对值")
    {
        srs_type=0;
    }else if(exArgs[4]=="最大值")
    {
        srs_type=1;
    }else if(exArgs[4]=="最小值")
    {
        srs_type=-1;
    }

    rec["shockType"]=QString::number(srs_type);
    rec["min_fre"]=exArgs[1];
    rec["max_fre"]=exArgs[2];

    auto oct=exArgs[3].split("/");
    double upper=oct[0].toDouble();
    double lowwer=oct[1].toDouble();
    rec["oct_step"]=QString::number(upper/lowwer);

    rec["refFrequency"]=exArgs[0];

    return rec;
}

void DataManager::openExDocument(QWidget* mainwindow)
{
    if(currentDocument)
    {
        auto result=QMessageBox::question(mainwindow,"提醒","当前已打开一个实验，是否覆盖？");
        if(result!=QMessageBox::Button::Yes)
        {
            return;
        }
        currentDocument->deleteLater();
        currentDocument=nullptr;
    }
   auto file=QFileDialog::getOpenFileName(mainwindow,"打开实验","选择实验文件","*.SRSEx");
   if(file=="")
       return;
   bool ok;
   currentDocument=ExperimentDocument::readFromLocalFile(file,ok);
   currentDocument->moveToThread(dataThread);
}

void DataManager::saveExDocument(QWidget* mainwindow)
{
    if(!currentDocument)
    {
        QMessageBox::information(mainwindow,"提示","当前没有打开实验，请先打开或新建实验");
        return;
    }
    QString prefix=QFileDialog::getExistingDirectory(mainwindow,"./");
    QMetaObject::invokeMethod(currentDocument,
                              "saveToLocalFile",Q_ARG(QString,prefix));

}

void DataManager::createExDocument(QWidget* mainwindow)
{
    auto name=QInputDialog::getText(mainwindow,"输入实验名称","实验名称");
    if(name=="")
    {
        return;
    }

    if(currentDocument)
    {
        currentDocument->deleteLater();
    }
    currentDocument=new ExperimentDocument();
    currentDocument->experimentName=name;

    currentDocument->moveToThread(dataThread);

}

void DataManager::writeProcessedDataToDocument(QWidget* mainwindow,
                                               QVector<QPointF> data)
{
    if(currentDocument)
    QMetaObject::invokeMethod(currentDocument,
                              "addprocessedDataToBuffer",
                              Q_ARG(QVector<QPointF>,data));
}

void DataManager::writeRawDataToDocument(QWidget* mainwindow,
                                               QVector<QPointF> data)
{
    if(currentDocument)
    QMetaObject::invokeMethod(currentDocument,
                              "addrawDataToBuffer",
                              Q_ARG(QVector<QPointF>,data));
}

QString DataManager::exportProcessedData(QString prefix)
{

    if(currentDocument)
    {
       QString rec;
       QMetaObject::invokeMethod(currentDocument,
                              "exportProcessedData",Q_RETURN_ARG(QString,rec),
                              Q_ARG(QString,prefix));
       return rec;
    }
    LOG(INFO)<<"current document nullptr,returnning...";
    return "";//默认返回空字符串为错误，可以作为错误处理
}

QString DataManager::exportSampledData(QString prefix)
{
    if(currentDocument)
    {
       QString rec;
       QMetaObject::invokeMethod(currentDocument,
                              "exportSampledData",Q_RETURN_ARG(QString,rec),
                              Q_ARG(QString,prefix));
       return rec;
    }
    LOG(INFO)<<"current document nullptr,returnning...";
    return "";
}

::targetpoint mTargetPoint::toTargetPoint()
{
   ::targetpoint rec;
    rec.accel=y;
    rec.freq=x;
    rec.down_limit=lowMiss;
    rec.up_limit=upMiss;
    rec.left_slope=0;
    rec.right_slope=0;
    return rec;
}
