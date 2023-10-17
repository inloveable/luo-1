#include "iir.hpp"
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <vector>
using namespace std;
using namespace chrono;
IIR::IIR()
{
    //ctor
    m_pNum = NULL;
    m_pDen = NULL;
    m_pW = NULL;
    m_num_order = -1;
    m_den_order = -1;
    m_N = 0;
};
 
/** \brief 将滤波器的内部状态清零，滤波器的系数保留
 * \return
 */
void IIR::reset()
{
    for(int i = 0; i < m_N; i++)
    {
        m_pW[i] = 0.0;
    }
}
/** \brief
 *
 * \param num 分子多项式的系数，升序排列,num[0] 为常数项
 * \param m 分子多项式的阶数
 * \param den 分母多项式的系数，升序排列,den[0] 为常数项
 * \param m 分母多项式的阶数
 * \return
 */
void IIR::setPara(double num[], int num_order, double den[], int den_order)
{
    delete[] m_pNum;
    delete[] m_pDen;
    delete[] m_pW;
    m_num_order = num_order;
    m_den_order = den_order;
    m_N = max(num_order, den_order) + 1;
    m_pNum = new double[m_N];
    m_pDen = new double[m_N];
    m_pW = new double[m_N];
    for(int i = 0; i < m_N; i++)
    {
        m_pNum[i] = 0.0;
        m_pDen[i] = 0.0;
        m_pW[i] = 0.0;
    }
    for(int i = 0; i <= num_order; i++)
    {
         m_pNum[i] = num[i];
    }
    for(int i = 0; i <= den_order; i++)
    {
        m_pDen[i] = den[i];
    }
}
/** \brief 计算 IIR 滤波器的时域响应，不影响滤波器的内部状态
 * \param data_in 为滤波器的输入，0 时刻之前的输入默认为 0，data_in[M] 及之后的输入默认为data_in[M-1]
 * \param data_out 滤波器的输出
 * \param M 输入数据的长度
 * \param N 输出数据的长度
 * \return
 */
void IIR::resp(double data_in[], int M, double data_out[], int N)
{
    int i, k, il;
    for(k = 0; k < N; k++)
    {
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            if( k - i >= 0)
            {
                il = ((k - i) < M) ? (k - i) : (M - 1);
                data_out[k] = data_out[k] + m_pNum[i] * data_in[il];
            }
        }
        for(i = 1; i <= m_den_order; i++)
        {
            if( k - i >= 0)
            {
                data_out[k] = data_out[k] - m_pDen[i] * data_out[k - i];
            }
        }
    }
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data 输入数据
 * \return 滤波后的结果
 */
double IIR::filter(double data)
{
    m_pW[0] = data;
    for(int i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
    {
        m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
    }
    data = 0.0;
    for(int i = 0; i <= m_num_order; i++)
    {
        data = data + m_pNum[i] * m_pW[i];
    }
    for(int i = m_N - 1; i >= 1; i--)
    {
        m_pW[i] = m_pW[i-1];
    }
    return data;
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data[] 传入输入数据，返回时给出滤波后的结果
 * \param len data[] 数组的长度
 * \return
 */
void IIR::filter(double data[], int len)
{
    int i, k;
    for(k = 0; k < len; k++)
    {
        m_pW[0] = data[k];
        for(i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
        {
            m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
        }
        data[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            data[k] = data[k] + m_pNum[i] * m_pW[i];
        }
 
        for(i = m_N - 1; i >= 1; i--)
        {
            m_pW[i] = m_pW[i-1];
        }
    }
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data_in[] 输入数据
 * \param data_out[] 保存滤波后的数据
 * \param len 数组的长度
 * \return
 */
void IIR::filter(double data_in[], double data_out[], int len)
{
    int i, k;
    for(k = 0; k < len; k++)
    {
        m_pW[0] = data_in[k];
        for(i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
        {
            m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
        }
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            data_out[k] = data_out[k] + m_pNum[i] * m_pW[i];
        }
 
        for(i = m_N - 1; i >= 1; i--)
        {
            m_pW[i] = m_pW[i-1];
        }
    }
}
/** \brief 滤波函数，采用直接II型结构
 *
 * \param data_in 输入数据
 * \param data_out 保存滤波后的数据
 * \return
 */



void IIR::filter(std::vector<double> &data_in, std::vector<double> &data_out)
{
    int i, k;

    for(k = 0; k < data_in.size(); k++)
    {
        m_pW[0] = data_in[k];
        for(i = 1; i <= m_den_order; i++) // 先更新 w[n] 的状态
        {
            m_pW[0] = m_pW[0] - m_pDen[i] * m_pW[i];
        }
        data_out[k] = 0.0;
        for(i = 0; i <= m_num_order; i++)
        {
            data_out[k] = data_out[k] + m_pNum[i] * m_pW[i];
        }
 
        for(i = m_N - 1; i >= 1; i--)
        {
            m_pW[i] = m_pW[i-1];
        }
    }

}

void IIR::filter_test()
{
    char inputfile [] = "/home/qfpku/Dt_SRS/input.txt";    // 从文件 input.txt 中读入原始数据
    char outputfile [] = "output.txt";    // 将结果输出到文件 output.txt 中
    const int N = 100000;
    double input_a [N] = {}, filter_a [N] = {}, input_t [N] = {};
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
    int num = 200000;
    while ((fscanf (input, "%lf%lf", input_t + i, input_a + i)) != EOF)
        {
        i ++;
        }
    n = i;    // number of points

    //set the coeff of iir filter
    double b[3] = {0.0006724, 0.00011058, -0.00061669};
    double a[3] = {1.0, -1.99854463, 0.99871091};
    double fn =  119.86456615013473;
    IIR filter;
    filter.setPara(b, 2, a, 2);
    typedef std::chrono::high_resolution_clock Clock;
    auto pre_time = Clock::now();
    filter.filter(input_a, filter_a, n);
    printf("the max out is:%lf  \n", *max_element(filter_a, filter_a + n));
    auto pos_time = Clock::now();
    printf ("success filte, use time:");
    std::cout <<std::chrono::duration_cast<std::chrono::nanoseconds>(pos_time - pre_time).count()<< '\n';
    
    filter.reset();
 
    // for (int i = 0; i < len; i++)
    // {
    //     cout << x[i] <<", " << y[i]<<  endl;
    // }
}
