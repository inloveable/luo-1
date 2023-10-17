#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include"publicDefs.hpp"
#include "sysbackend.hpp"
#include <QObject>
#include<QMap>
#include <memory>

struct targetpoint;
class ExperimentDocument;


class DataManager : public QObject
{
    Q_OBJECT

    SINGLETON_DECLARE(DataManager)
public:
    explicit DataManager(QObject *parent = nullptr);


    void init();

    SYSBackEnd* getSysBackEnd(){return backEnd;};

    void openExDocument(QWidget* mainwindow);
    void saveExDocument(QWidget* mainwindow);
    void createExDocument(QWidget* mainwindow);
    void writeRawDataToDocument(QWidget* mainwindow,QVector<QPointF> rawData);
    void writeProcessedDataToDocument(QWidget* mainwindow,QVector<QPointF> processed);
    QString exportProcessedData(QString prefix);
    QString exportSampledData(QString prefix);



    QVector<QVector<QString>>& getChannelInfo(const QString& index);
    bool                       setChannelInfoFile(const QString& filePath);
    void                       saveChannelInfoFile( const QString& filePath);
    void                       changeChannelValue(const QString& table,int index,
                                                  const QString& v1,const QString& v2);
    void                       changeChannelValueEx(const QString& table,int index,
                                                    const QList<QString>&);
    void                       changeExArgValueEx(const QString& table,int index,
                                                  const QList<QString>&);
    //special designed func...
    void                       updateExArgAtOnce(const QVector<QString>& args);
    void                       updateExArgSettingInfo(const QList<QString>& args);//update relative file
    //ex setting and display are under the same document
    QVector<QString>& getExSettingInfo(const QString& index);
    QVector<QVector<QString>>& getDisplayInfo(const QString& index);
    bool                       setExSettingFile(const QString& filePath);
    void                       saveExSettingFile(const QString& fileName);
    void                       changeDisplaySettingValue(const QString& table,
                                                    int index,const QList<QString>& value);
    void                       changeDisplaySettingValue(const QString& table,
                                                    int index,int index2,const QString& value);
    void                       updateMainChart();

    QMap<QString,QString> getSRSArguments(int channel);

    QString ChannelArgTemplateFilePath;
    QString ExConfiTemplateFilePath;
    QString appCachePath;


    QMap<QString,QString> unitMap;


    QString& getTimeZoneArg(const QString& index){return timeZoneArgs.find(index).value();};
    void     setTimeZoneArg(const QString& index,const QString& newer){timeZoneArgs[index]=newer;};


signals:
      void                       changeProjectUnit(const QString& type,const QString& newer);
      void                       sendRTData(int ch,std::shared_ptr<QVector<QPointF>>& vec);

     std::shared_ptr<QVector<QPointF>> requestRawData(int ch);
     void updateBackEndArgument(SYSBackEnd::ArgType index,const QString& newer,int ch=0);

private:



    QString ChannelArgFile="";
    QString ExConfigFile="";
    QString ExDataFile="";

    QMap<QString,QVector<QVector<QString>>>* mapToChannelInfo=nullptr;
    QMap<QString,QVector<QVector<QString>>>* mapToDisplaySettingInfo=nullptr;
    QMap<QString,QVector<QString>>* mapToExSettingInfo=nullptr;

    void readFromArgFile();
    void readFromExSettingFile();

    QThread* dataThread=nullptr;
    SYSBackEnd* backEnd=nullptr;
    ExperimentDocument* currentDocument=nullptr;

    QVector<QVector<QString>> forRec;
    QVector<QString> forRec1;

    QMap<QString,QString> timeZoneArgs;

    void updateBackEndArguments();


signals:

   void onSampledRawDataSent(std::shared_ptr<QVector<QPointF>>s,int ch=0);

 public:


   void replaceTargetPoint(QVector<mTargetPoint>& points){refPoints=points;};
   QVector<mTargetPoint> getRefPoints(){return refPoints;};
 private:
   void acqusitionStarted(bool ok);
   QTimer *realTimeTimer;

   QVector<mTargetPoint> refPoints;


};



#endif // DATAMANAGER_H
