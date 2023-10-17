#pragma once

#include <QObject>
#include<QDateTime>
#include <memory>

class QFile;
class ExperimentDocument : public QObject
{
    Q_OBJECT
public:
    explicit ExperimentDocument(QObject *parent = nullptr);
    ~ExperimentDocument();

    QString experimentName;
    int     experimentSampleCount=0;
    QString channelArgFile;
    QString experimentSettingFile;
    static ExperimentDocument* readFromLocalFile(QString& file,bool& ok,
                                                 QObject* parent=nullptr);

  public slots:
    QString saveToLocalFile(QString prefix);

    QString exportSampledData(QString prefix);
    QString exportProcessedData(QString prefix);




   void addrawDataToBuffer(QVector<QPointF> rawData);//sampled
   void addprocessedDataToBuffer(QVector<QPointF> processed);//srsed


signals:

private:

   QDateTime createTime;
   QFile*    sampledBufferFile=nullptr;
   QFile*    processedBufferFile=nullptr;


   QString originJsonFilePath="";
   QString appCachePath;

   void initRawDataFromFile(QFile& file);
   void initProcessedFromFile(QFile& file);
};

