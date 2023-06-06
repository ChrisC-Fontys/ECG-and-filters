#ifndef Filters_H
#define Filters_H

typedef struct filtertype
{
	float *Filtertemp;
	float *filterout;
	short ordernum;
	float *coef;
	float *filtergain;
}filtertype;

// make structures for all filter types
struct filtertype LPF;
struct filtertype Notch;
struct filtertype HPF;

void SecondOrderFilter(float *x, float *p_fCoef,float *y, float*gain, short n,short coefnum);
void Shiftleftdata(float data_In[], int size);

float Usefilter(float *Filter_in, float*filter_temp, float*filter_coef, short ordernum, float*filtergain);

void Intitialize_filters();

#endif  // Filters_H
