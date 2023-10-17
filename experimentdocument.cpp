#include "experimentdocument.hpp"
#include "datamanager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include<glog/logging.h>
#include<QApplication>
#include <memory>
#include<QFile>
#define DATE_FORMAT "yy-MM-dd-hh-mm-ss"
//buffer data are written into txts.and txts was initialized during first file created.
ExperimentDocument::ExperimentDocument(QObject *parent)
    : QObject{parent},createTime{QDateTime::currentDateTime()},
      appCachePath{QApplication::applicationDirPath()+"/appCaches/"}
{

    experimentName=QString{"默认实验_"+createTime.toString(DATE_FORMAT)};

}

ExperimentDocument* ExperimentDocument::readFromLocalFile(QString& file,
                                                          bool& ok,QObject* parent)
{
    ExperimentDocument* document=new ExperimentDocument(parent);

    document->originJsonFilePath=file;

    ok=true;
    return document;
}

QString ExperimentDocument::saveToLocalFile(QString prefix)
{


   auto dataManager=DataManager::GetInstance();

   QFile file{appCachePath+"defaults/ExDocumentTemplate.json"};
   file.open(QIODevice::ReadWrite);

   QByteArray buffer=file.readAll();
   file.close();
   QJsonDocument document=QJsonDocument::fromJson(buffer);


   QJsonObject rootObject=document.object();
   auto object=rootObject["ExperimentProperties"].toObject();


   object["ExperimentName"]=experimentName;
   object["CreateTime"]=createTime.toString();
   object["SampleCount"]=experimentSampleCount;
   object["ExperimentSettingFile"]=experimentSettingFile;
   object["ChannelArgSettingFile"]=channelArgFile;

   rootObject["ExperimentProperties"]=object;
   if(sampledBufferFile)
   {
       sampledBufferFile->close();
       std::vector<std::vector<QPair<double,double>>> table;
       std::vector<QPair<double,double>> vec;
       QFile tmpFile{sampledBufferFile->fileName()};
       tmpFile.open(QIODevice::ReadOnly);
       while(!tmpFile.atEnd())
       {
           QByteArray arr;
           arr=tmpFile.readLine();
           if(arr.size()>1)//if not empty line
           {
               arr.chop(1);//remove '\n'
           }
           else if(arr.size()==1)
           {
              table.emplace_back(std::move(vec));
           }
           else
           {
               continue;
           }
           QString tmp=QString::fromUtf8(arr);
           auto numbers=tmp.split(" ");
           if(numbers.size()!=2)
           {
               LOG(WARNING)<<"PARSE ERROR WHEN READ";
               continue;
           }
           vec.emplace_back(QPair<double,double>{numbers[0].toDouble(),numbers[1].toDouble()});
       }
       tmpFile.close();
       object=rootObject["SampledData"].toObject();
       QJsonArray xArray;//=object["XAarry"].toArray();
       QJsonArray yArray;//=object["YAarry"].toArray();

       for(int i=0;i<table.size();++i)
       {
           QJsonArray xBuffer;
           QJsonArray yBuffer;
           for(int j=0;j<table[i].size();++j)
           {
               auto& pair=table[i][j];
               xBuffer.push_back(pair.first);
               yBuffer.push_back(pair.second);
           }
           xArray.push_back(xBuffer);
           yArray.push_back(yBuffer);
       }

       object["XArray"]=xArray;
       object["YArray"]=yArray;
       rootObject["SampledData"]=object;
       sampledBufferFile->open(QIODevice::WriteOnly);
   }
    if(processedBufferFile)
   {
        processedBufferFile->close();
       std::vector<std::vector<QPair<double,double>>> table;
       std::vector<QPair<double,double>> vec;
       QFile tmpFile{processedBufferFile->fileName()};
       tmpFile.open(QIODevice::ReadOnly);

       while(!tmpFile.atEnd())
       {
           QByteArray arr;
           arr=tmpFile.readLine();
           if(arr.size()>1)//if not empty line
           {
               arr.chop(1);//remove '\n'
           }
           else if(arr.size()==1&&(!vec.empty()))
           {
              table.emplace_back(std::move(vec));
           }
           else
           {
               continue;
           }
           QString tmp=QString::fromUtf8(arr);
           auto numbers=tmp.split(" ");
           if(numbers.size()!=2)
           {
               LOG(WARNING)<<"PARSE ERROR WHEN READ";
               continue;
           }
           vec.emplace_back(QPair<double,double>{numbers[0].toDouble(),numbers[1].toDouble()});
       }
       table.emplace_back(std::move(vec));
       tmpFile.close();

       object=rootObject["ProcessData"].toObject();
       QJsonArray xArray;//=object["XArray"].toArray();
       QJsonArray yArray;//=object["YArray"].toArray();

       for(int i=0;i<table.size();++i)
       {
           QJsonArray xBuffer;
           QJsonArray yBuffer;
           for(int j=0;j<table[i].size();++j)
           {
               auto& pair=table[i][j];
               xBuffer.push_back(pair.first);
               yBuffer.push_back(pair.second);
           }
           xArray.push_back(xBuffer);
           yArray.push_back(yBuffer);
       }

       object["XArray"]=xArray;
       object["YArray"]=yArray;
       rootObject["ProcessData"]=object;
       processedBufferFile->open(QIODevice::WriteOnly);
   }

   document.setObject(rootObject);
   QByteArray fileBuffer=document.toJson();

   QFile realSaveFile{prefix+"/"+experimentName+"_"+createTime.toString(DATE_FORMAT)+".SRSEx"};
   realSaveFile.open(QIODevice::WriteOnly);
   realSaveFile.write(fileBuffer);
   realSaveFile.close();
   return realSaveFile.fileName();

}

void ExperimentDocument::addprocessedDataToBuffer(QVector<QPointF> data)
{
    if(!processedBufferFile)
    {
        processedBufferFile=new QFile(this);
        processedBufferFile->setFileName(appCachePath+"processedBuffer.txt");
        processedBufferFile->open(QIODevice::WriteOnly);
        QFile origin{originJsonFilePath};
        if(origin.exists())
        initProcessedFromFile(origin);
    }

    processedBufferFile->write("\n");
    int count=0;
    for(auto&& i:data)
    {
        auto rec=processedBufferFile->write(QString{"%1 %2\n"}.arg(i.x()).arg(i.y()).toUtf8());
        if(rec)
            count++;
    }
    LOG(INFO)<<"processed data write count:"<<count;

    experimentSampleCount++;
}

void ExperimentDocument::initProcessedFromFile(QFile& file)
{
    file.open(QIODevice::ReadOnly);

    QByteArray buffer=file.readAll();
    QJsonDocument document=QJsonDocument::fromJson(buffer);

    QJsonObject rootObject=document.object();
    QJsonObject object=rootObject["ProcessData"].toObject();

    QJsonArray  arrroot=object["XArray"].toArray();
    QVector<QVector<double>> xVals;
    for(auto&& arr:arrroot)
    {
        QVector<double> xVal;
        for(auto&& i:arr.toArray())
        {
            xVal.emplace_back(i.toDouble());
        }
        xVals.push_back(std::move(xVal));
    }

    arrroot=object["YArray"].toArray();

    QVector<QVector<double>> yVals;

    for(auto&& arr:arrroot)
    {
        QVector<double> yVal;
        for(auto&& i:arr.toArray())
        {
            yVal.emplace_back(i.toDouble());
        }
        yVals.emplace_back(std::move(yVal));
    }

    for(int Index=0;Index<xVals.size();++Index)
    {
        for(int i=0;i<xVals[Index].size();++i)
        {
            QString tmp=QString("%1 %2").arg(xVals[Index][i],yVals[Index][i]);
            processedBufferFile->write(QString(tmp+"\n").toUtf8());
        }
        processedBufferFile->write("\n");
        experimentSampleCount++;//increase only when meet processed data
    }

    file.close();
}

ExperimentDocument::~ExperimentDocument()
{
   if(sampledBufferFile)
   {
       sampledBufferFile->close();
       sampledBufferFile->remove();
   }
   if(processedBufferFile)
   {
       processedBufferFile->close();
       processedBufferFile->remove();
   }
}

void ExperimentDocument::addrawDataToBuffer(QVector<QPointF> data)
{
    if(!sampledBufferFile)
    {
        sampledBufferFile=new QFile(this);
        sampledBufferFile->setFileName(appCachePath+"rawDataBuffer.txt");
        sampledBufferFile->open(QIODevice::WriteOnly);
        QFile origin{originJsonFilePath};
        if(origin.exists())
        initRawDataFromFile(origin);
    }
    sampledBufferFile->write("\n");

    int count=0;
    for(auto&& i:data)
    {
        auto rec=sampledBufferFile->write(QString{"%1 %2\n"}.arg(i.x()).arg(i.y()).toUtf8());
        if(rec)
            count++;
    }
    LOG(INFO)<<"sampledBufferFile data write count:"<<count;
}
void ExperimentDocument::initRawDataFromFile(QFile& file)
{
    file.open(QIODevice::ReadOnly);

    QByteArray buffer=file.readAll();
    QJsonDocument document=QJsonDocument::fromJson(buffer);

    QJsonObject rootObject=document.object();

    QJsonObject object=rootObject["SampledData"].toObject();
    QJsonArray  arrroot=object["XArray"].toArray();
    QVector<QVector<double>> xVals;
    for(auto&& arr:arrroot)
    {
        QVector<double> xVal;
        for(auto&& i:arr.toArray())
        {
            xVal.emplace_back(i.toDouble());
        }
        xVals.push_back(std::move(xVal));
    }

    arrroot=object["YArray"].toArray();

    QVector<QVector<double>> yVals;

    for(auto&& arr:arrroot)
    {
        QVector<double> yVal;
        for(auto&& i:arr.toArray())
        {
            yVal.emplace_back(i.toDouble());
        }
        yVals.emplace_back(std::move(yVal));
    }

    for(int Index=0;Index<xVals.size();++Index)
    {
        for(int i=0;i<xVals[Index].size();++i)
        {
            QString tmp=QString("%1 %2").arg(xVals[Index][i],yVals[Index][i]);
            sampledBufferFile->write(QString(tmp+"\n").toUtf8());
        }
        sampledBufferFile->write("\n");
    }
    file.close();
}

QString ExperimentDocument::exportProcessedData(QString prefix)
{
    if(!processedBufferFile)
    {
        LOG(INFO)<<"no processedBuffer storage.";
        return "";
    }
    processedBufferFile->close();
    QString fileName=prefix+this->experimentName+"processed.txt";
    processedBufferFile->copy(fileName);

    processedBufferFile->open(QIODevice::WriteOnly);

    return fileName;

}

QString ExperimentDocument::exportSampledData(QString prefix)
{
    if(!sampledBufferFile)
    {
        LOG(INFO)<<"no processedBuffer storage.";
        return "";
    }
    sampledBufferFile->close();
    QString fileName=prefix+this->experimentName+"sampled.txt";
    sampledBufferFile->copy(fileName);

    sampledBufferFile->open(QIODevice::WriteOnly);

    return fileName;
}


