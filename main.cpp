


#include "mainwindow.h"

#include <QApplication>
#include<QDir>
#include<glog/logging.h>
#include"datamanager.h"


void directoryInit();

int main(int argc, char *argv[])
{



        QApplication a(argc, argv);
        FLAGS_alsologtostderr = 1;

        FLAGS_log_dir = (QApplication::applicationDirPath() + "/logDir").toStdString();
        FLAGS_colorlogtostderr = true;//是否启用不同颜色显示(如果终端支持)
        google::SetLogFilenameExtension(".txt");
        google::InitGoogleLogging(argv[0]);

        DataManager::GetInstance()->init();


        directoryInit();
        MainWindow w;



     return a.exec();
}

void directoryInit()
{
    QDir appdir{QApplication::applicationDirPath()};
    if(!appdir.exists("appCaches"))
    {
        appdir.mkdir("appCaches");
    }
    if(!appdir.exists("images"))
    {
        appdir.mkdir("images");
    }
    if(!appdir.exists("printBuffers"))
    {
        appdir.mkpath(appdir.absolutePath()+"/printBuffers/doc");
        appdir.mkpath(appdir.absolutePath()+"/printBuffers/pdf");
    }
    if(!appdir.exists("logDir"))
    {
        appdir.mkdir("logDir");
    }

}
