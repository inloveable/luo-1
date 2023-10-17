#include "math_util.h"
#include <math.h>

double get_sample_interval(std::vector<double> input)
{
    //计算一段序列的平均采样间隔
    int n = 0;
    double sum = 0;
    for (int i = 1; i < input.size(); i++) {
        double delta = input[i] - input[i - 1];
        sum += delta; 
        n++;
    }
    double output = sum / n;
    return output;
}

double computey_twopoint_logline(signal_point point_1, signal_point point_2, double input_x)
{
    //两个点point_1, point_2能够给定一个log坐标下的直线
    //对于输入的input_x，计算其在直线上对应的output_y

    double k = log(point_2.y / point_1.y) / log(point_2.x / point_1.x); //k为log坐标下的斜率
    return point_1.y * pow((input_x / point_1.x), k);
}