#pragma once
#include <iostream>
#include<vector>


//定义信号点结构体
struct signal_point
{
    double x;  //x值，对应于时域信号的时间点，或者频率域信号的频率
    double y;  //y值，对应于加速度
};

//定义时域信号结构体
struct time_signal
{
    int N;  //N(采样点个数)
    std::vector<double> time;  //时间序列
    std::vector<double> accel;  //加速度值
};

//定义响应谱结构体
struct response_spectrum
{
    std::vector<double> freq;  //频率序列
    std::vector<double> accel;  //加速度值
};

//定义响应谱状态栏显示的数据的结构体
typedef struct srs_statebar
{
    double damp;  //阻尼比
    signal_point max;  //srs曲线中的最大值对应的点
    signal_point min;  //srs曲线中的最小值对应的点
    double exceed; //超出容差的百分比
}Srs_statebar,*Ptr_Srs_statebar;

//定义fft频谱结构体
typedef struct frequency_spectrum
{
    std::vector<double> freq;  //频率序列
    std::vector<double> accel;  //加速度值
}Frequency_spectrum,*Ptr_Frequency_spectrum;

typedef struct targetpoint
{
    double freq;
    double accel;
    double left_slope;
    double right_slope;
    double up_limit;
    double down_limit;
} Targetpoint,*PtrTargetPoint;

double get_sample_interval(std::vector<double> input);
double computey_twopoint_logline(signal_point point_1, signal_point point_2);
