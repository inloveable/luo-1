#include "srs.h"
#include "iir.hpp"
#include "qmath.h"
#include <math.h>

#include <algorithm>
using namespace std;

void SRS::SRS_test ()
{
    //
    char inputfile [] = "/home/qfpku/Dt_SRS/fft/input.txt";    // 从文件 input.txt 中读入原始数据
    char outputfile [] = "output.txt";    // 将结果输出到文件 output.txt 中
    const int N = 100000;
    double input_a [N] = {}, input_t [N] = {};
    int n, i;
    FILE *input, *output;
    printf ("begin open file. ");
    if (!(input = fopen (inputfile, "r")))
        {
        printf ("Cannot open file. ");
        exit (1);
        }
    if (!(output = fopen (outputfile, "w")))
        {
        printf ("Cannot open file. ");
        exit (1);
        }
    
    i = 0;
    //写入输入信号
    time_signal t_in;
    while ((fscanf (input, "%lf%lf", input_t + i, input_a + i)) != EOF)
    {
        t_in.time.push_back(input_t[i]);
        t_in.accel.push_back(input_a[i]);
        i ++;
    }
    n = i;    // number of points
    t_in.N = n;
    //设置SRS参数并计算
    double c_in = 0.05;  //阻尼
    double f_min = 10;  //起始频率
    double f_max = 1e4;  //终止频率
    double oct_step = 1.0 / 12.0;  //倍频步长
    SRS test;
    test.set_c(c_in);
    test.import_t(t_in);
    test.set_freq(f_min, f_max, oct_step);
    test.update_timesignal();
    test.srs_update();
    response_spectrum result = test.get_srs_result();
    printf("result size is %ld", result.freq.size());
    for (int i = 0; i < result.freq.size(); i ++) {
        fprintf (output, "%lf    %lf\n", result.freq[i], result.accel[i]);
    }

}

void SRS::run_srs_analysis()
{
    //
    double damp = c;
    double s_to_ms = 1e-3;
    double dt = get_sample_interval(t_use.time) * s_to_ms;
    signal_point max_point;
    max_point.y = -1e10;
    signal_point min_point;
    min_point.y = 1e10;
    for (int i = 0; i < freq_vec.size(); i++){
        double wn = 2 * M_PI * freq_vec[i];
        double wd = wn * sqrt(1.0 - c * c);
        double E = exp(-damp * wn * dt);
        double K = dt * wd;
        double C = E * cos(K);
        double S = E * sin(K);
        double S_prime = S / K;
        double b [3] = {}, a [3] = {};

        b[0] = 1.0 - S_prime;
        b[1] = 2.0 * (S_prime - C);
        b[2] = E * E - S_prime;
        a[0] = 1.0;
        a[1] = -2 * C;
        a[2] = E * E;

        
        std::vector<double> input = t_use.accel;
        std::vector<double> output = input;

        //
        IIR filter;
        filter.setPara(b, 2, a, 2);
        filter.filter(input, output);

        double max_g = *max_element(output.begin(), output.end());
        double min_g = *min_element(output.begin(), output.end());
        if (max_g < abs(min_g)) {
            max_g = abs(min_g);
        }
        srs_result.accel.push_back(max_g);
        srs_result.freq.push_back(freq_vec[i]);

        // record the max and min values
        if (max_g > max_point.y) {
            max_point.x = freq_vec[i];
            max_point.y = max_g;
        }
        if (max_g < min_point.y) {
            min_point.x = freq_vec[i];
            min_point.y = max_g;
        }
    }
    //save the statebar
    statebar.max = max_point;
    statebar.min = min_point;
    statebar.damp = c;
    statebar.exceed = 0;
}

void SRS::set_bias(double bias_in) 
{
    bias = bias_in;
}
    
void SRS::set_biasmode(bool biasmode) 
{
    bias_mode = biasmode;
}

void SRS::set_freq(double f_min, double f_max, double oct_step)
{
    freq_vec.clear();
    // 设置要计算的频率列表，f_min为起始频率， f_max为终止频率，
    // oct_step为备频步长
    double f = f_min;
    
    while (f < f_max)
    {
        freq_vec.push_back(f);
        f = f * pow(2, oct_step);
    }
    freq_vec.push_back(f_max);

}

void SRS::update_timesignal()
{
    //改变时域信号参数时，对时域信号进行更新
    t_use = t_origin;
}

void SRS::srs_update()
{
    //改变响应谱计算参数时，对响应谱结果进行更新
    run_srs_analysis();
}

