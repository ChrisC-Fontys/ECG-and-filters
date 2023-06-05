void SecondOrderFilter(float *x, float *p_fCoef,float *y, float*gain, short n,short coefnum);
void Shiftleftdata(float data_In[], int size);

float Usefilter(float *Filter_in, float*filter_temp, float*filter_coef, short ordernum, float*filtergain);
