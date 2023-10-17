//
// 快速傅立叶变换 Fast Fourier Transform
// By rappizit@yahoo.com.cn
// 2007-07-20
// 版本 2.0
// 改进了《算法导论》的算法，旋转因子取 ωn-kj  (ωnkj 的共轭复数)
// 且只计算 n / 2 次，而未改进前需要计算 (n * lg n) / 2 次。
// 
 #pragma once
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "math_util.h"

class fft
{
public:
    fft();
    ~fft();
    void FFT_test ();
    void import_t(time_signal t_in) {t_origin = t_in;};
    void set_filter(double low_freq, double high_freq);

//常数设置以及内部变量的声明
private:
    static const int N = 102400;
    const float PI = 3.1416;
    float *xreal;
    float *ximag;
    int n;

    time_signal t_origin;
    time_signal t_use;
    frequency_spectrum fft_result;

//内部函数的声明
private:
    void solve_fft(float xreal [], float ximag [], int n);
    void  solve_ifft (float xreal [], float ximag [], int n);
    
};

