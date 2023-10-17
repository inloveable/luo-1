// Calculate Shock Response Spectrum (SRS) from accelerometer data 
// using Smallwood ramp invariant method (http://www.vibrationdata.com/ramp_invariant/DS_SRS1.pdf)
#pragma once
// Author: Detail
// Date: 2022.11.01
// Version: 0.1

// Credits: 
// - Tom Irvine wrote a similar python module, and I am borrowing the idea to use scipy.signal.lfilter
// - David Smallwood developed the algorithm
#include <iostream>
#include<vector>
#include "math_util.h"


class SRS
{
public:

    void SRS_test ();
    void update_timesignal();
    void srs_update();

    
    void set_Q(double Q_in) {Q = Q_in;};
    void set_c(double c_in) {c = c_in;};
    void set_bias(double bias_in);//
    void set_srs_type(int type) {srs_type = type;};
    void set_biasmode(bool biasmode);
    void import_t(time_signal t_in) {t_origin = t_in;};
    void input_target_srs(std::vector<targetpoint> target_points) {srs_target_points = target_points;};
    void set_freq(double f_min, double f_max, double oct_step);
    void set_ref_freq(double ref_freq) {srs_ref_freq = ref_freq;};
    

    response_spectrum& get_srs_result() {return srs_result;};
    srs_statebar srs_statebar() {struct srs_statebar result = statebar; return result;};
    
//常数设置以及内部变量的声明
private:
    double Q = 10; //衰减因子
    double c = 0.05; //阻尼系数（c 和 Q 是一个东西，c = 1 / (2Q)）
    double bias = 0;
    double srs_ref_freq = 100; //参考频率
    int srs_type = 0; //srs计算的响应谱幅值类型，0代表绝对值，1和-1代表最大值和最小值
    bool bias_mode = false;  //bias mode为 true 时代表自动去除bias，即采用平均值作为bias；false则采取设置的bias
    std::vector<double> freq_vec;  //计算的频率列表
    time_signal t_origin;
    time_signal t_use;
    response_spectrum srs_result;
    struct srs_statebar statebar;
    std::vector<targetpoint> srs_target_points;

//内部函数的声明
private:
    void run_srs_analysis();
};
