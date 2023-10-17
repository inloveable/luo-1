/**< IIR 滤波器实现 */
#pragma once
#include <complex>
#include <iostream>
#include<vector>

class IIR
{
public:
    IIR();
    void reset();
    void setPara(double num[], int num_order, double den[], int den_order);
    void resp(double data_in[], int m, double data_out[], int n);
    double filter(double data);
    void filter(double data[], int len);
    void filter(std::vector<double> &data_in, std::vector<double> &data_out);
    void filter(double data_in[], double data_out[], int len);
    void filter_test();
protected:
    private:
    double *m_pNum;
    double *m_pDen;
    double *m_pW;
    int m_num_order;
    int m_den_order;
    int m_N;
};
 
class IIR_BODE
{
private:
    double *m_pNum;
    double *m_pDen;
    int m_num_order;
    int m_den_order;
    std::complex<double> poly_val(double p[], int order, double omega);
public:
    IIR_BODE();
    void setPara(double num[], int num_order, double den[], int den_order);
    std::complex<double> bode(double omega);
    void bode(double omega[], int n, std::complex<double> resp[]);
};
