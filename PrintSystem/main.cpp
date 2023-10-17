#include <QApplication>
#include<QFile>
#include"pdfwriter.h"
#include "qnamespace.h"
#include "qpoint.h"
#include<QWidget>
#include<QUrl>
#include<QDesktopServices>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    {
        QFile local{"test2.pdf"};
        QPixmap map{"th.jpg"};

        PDFWriter writer(&local);
        writer.drawTitle("冲击响应谱试验");

        writer.paintStraghtHLine(QPointF(1000,500),500);

        QVector<QVector<QString>> table;
        table<<QVector<QString>{"输入通道","类型","耦合方式","传感器类型","灵敏度","低通滤波","高等滤波","通道偏置"};
        table<<QVector<QString>{"1","测试","电荷","加速度","0.135 pC/(g)","关,--,--,--,--","关,0.7","自动,--"};
        table<<QVector<QString>{"2","测试","电荷","加速度","0.135 pC/(g)","关,--,--,--,--","关,0.7","自动,--"};
        table<<QVector<QString>{"3","测试","电荷","加速度","0.135 pC/(g)","关,--,--,--,--","关,0.7","自动,--"};
        table<<QVector<QString>{"4","测试","电荷","加速度","0.135 pC/(g)","关,--,--,--,--","关,0.7","自动,--"};
        QVector<int> vwidth{1000,400,400,400,400};
        QVector<int> hwidth{795,530,795,995,1115,1655,1325,1000};


        writer.paintBoltText(QRectF{500,500,2000,500},"输入通道参数",Qt::AlignLeft);
        writer.paintTable(table,QRectF{500,1000,8500,2600},hwidth,vwidth);

        table.clear();
        vwidth.clear();
        hwidth.clear();

        for(int i=0;i<14;++i)
        {
            vwidth<<285;
        }
        hwidth<<4250<<4250;
        table<<QVector<QString>{"冲击响应类型","绝对值"};
        table<<QVector<QString>{"倍频分辨率","1/12"};
        table<<QVector<QString>{"下限频率","50 Hz"};
        table<<QVector<QString>{"上限频率","10000 Hz"};
        table<<QVector<QString>{"采样频率","96000 Hz"};
        table<<QVector<QString>{"帧点数","16384"};
        table<<QVector<QString>{"采样时间","100 ms"};
        table<<QVector<QString>{"冲击机类型","振动机"};
        table<<QVector<QString>{"冲击时间","2 s"};
        table<<QVector<QString>{"触发源","量级触发"};
        table<<QVector<QString>{"触发通道","所有输入通道"};
        table<<QVector<QString>{"触发量级","400 g"};
        table<<QVector<QString>{"触发延时","-50 %"};
        table<<QVector<QString>{"压力设置上限","900 Kpa"};
        writer.paintBoltText(QRectF{500,4000,2000,500},"目标谱表格",Qt::AlignLeft);
        writer.paintTable(table,QRectF{500,4500,8500,4000},hwidth,vwidth);

        table.clear();
        vwidth.clear();
        hwidth.clear();
        vwidth<<400<<400<<400<<400;
        for(int i=0;i<7;++i)
        {
            hwidth<<1214;
        }

        table<<QVector<QString>{"序号","频率(Hz)","加速度(g)","左斜率(dB/Oct)",
               "右斜率(dB/Oct)","中断上限(dB)","中断下限(dB)"};
        table<<QVector<QString>{"1","50","50","","","6","-6"};
        table<<QVector<QString>{"2","1000","5000","","","6","-6"};
        table<<QVector<QString>{"3","10000","5000","","","6","-6"};

        writer.paintBoltText(QRectF{500,9000,2000,500},"输入通道参数",Qt::AlignLeft);
        writer.paintTable(table,QRectF{500,9500,8500,1600},hwidth,vwidth);

        writer.addNewPage();

        writer.paintBoltText(QRectF{500,500,2000,500},"冲击响应谱",Qt::AlignLeft);
        writer.paintPixmap(QRect{500,1000,8500,5000},map);

        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"序号","命令","参数","脉冲数"};
        table<<QVector<QString>{"1","手动模式","--","100"};
        hwidth<<531<<2655<<2655<<2655;
        vwidth<<500<<500;
        writer.paintBoltText(QRectF{500,6500,2000,500},"计划表",Qt::AlignLeft);
        writer.paintTable(table,QRectF{500,7000,8500,1000},hwidth,vwidth);

        writer.paintBoltText(QRectF{500,8500,2000,500},"试验窗口图片",Qt::AlignLeft);
        writer.paintPixmap(QRect{500,9000,8500,4500},map);

        writer.addNewPage();
        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"控制气压","992Kpa","目标气压","250Kpa"};
        table<<QVector<QString>{"空气簧压力","110Kpa","当前帧数","5"};
        table<<QVector<QString>{"输出脉冲数","4","剩余脉冲数","96"};
        table<<QVector<QString>{"数据文件保存时间","2022-09-19-11","",""};

       for(int i=0;i<4;++i)
       {
           hwidth<<2125;
           vwidth<<500;
       }

        writer.paintTable(table,QRectF{500,500,8500,2000},hwidth,vwidth);
        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"名称","阻尼比","最大值(g)","频率(Hz)",
               "最小值(g)","频率(Hz)","状态描述","超谱百分比%"};
        table<<QVector<QString>{"目标谱(f)","--","5000","1000",
               "50","50","--","--"};
        table<<QVector<QString>{"SRS1(f)","0.05","8871.05","8543",
               "51.9644","66.742","超出容差 0%","90.5828"};
        vwidth<<500<<500<<500;
        hwidth<<1062<<1062<<1062<<1062<<1062<<1062<<1062<<1062;
        writer.paintTable(table,QRectF{500,3000,8500,1500},hwidth,vwidth);
        writer.paintPixmap(QRect{500,5000,8500,4500},map);

        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"控制气压","992Kpa","目标气压","250Kpa"};
        table<<QVector<QString>{"空气簧压力","110Kpa","当前帧数","5"};
        table<<QVector<QString>{"输出脉冲数","4","剩余脉冲数","96"};
        table<<QVector<QString>{"数据文件保存时间","2022-09-19-11","",""};

       for(int i=0;i<4;++i)
       {
           hwidth<<2125;
           vwidth<<500;
       }

        writer.paintTable(table,QRectF{500,10000,8500,2000},hwidth,vwidth);

        writer.addNewPage();
        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"帧序号","输入通道","加速度峰值","加速度峰值误差"
               ,"脉冲宽度","脉冲宽度误差","速度变化量","速度变化量误差"};
        table<<QVector<QString>{"--","理想波形","30","--"
               ,"28.44","--","5.32301","--"};
        table<<QVector<QString>{"5","1","3682.88","12176.3"
               ,"0.291667","-98.9745","-6.14749","-215.489"};

       for(int i=0;i<3;++i)
       {
           vwidth<<500;
       }
        hwidth={800,1000,1200,1200,1200,1200,1200,1200};

        writer.paintTable(table,QRectF{500,500,9000,1500},hwidth,vwidth);

        table.clear();
        vwidth.clear();
        hwidth.clear();

        table<<QVector<QString>{"输入通道","加速度峰值-平均值(g)","脉冲宽度-平均值(ms)","速度变化量-平均值(m/s)"};
        table<<QVector<QString>{"1","4770.74","0.290164","-0.482333"};
        hwidth<<700<<2701<<2701<<2701;
        vwidth<<500<<500;

        writer.paintTable(table,QRectF{500,2500,8500,1000},hwidth,vwidth);

    }




    QDesktopServices::openUrl(QUrl::fromLocalFile("test2.pdf"));



    QWidget w;
    w.show();
    return a.exec();
}
