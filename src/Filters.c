#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"
#include "Filters.h"

/************************** Variable Definitions ****************************/
// set parameters for all filters



/******************************* Functions **********************************/

void SecondOrderFilter(float *x, float *p_fCoef,float *y, float*gain, short n,short coefnum)
{
	//only the first 2nd order filter is different (not in the temp array)
	if (n==0)
	{  	n=2;
		// then apply the difference equation of the filter
		(y[n])= -(p_fCoef[coefnum])*(y[n-1]) - (p_fCoef[coefnum+1])*(y[n-2]) + gain[0]*((x[n]) + (p_fCoef[coefnum+2])*(x[n-1]) + (p_fCoef[coefnum+3])*(x[n-2]));
	}
	else
	{
		// apply the difference equation of the filter (y[n+2], y[n+3], y[n+1] only because it is the same array. We do not try to use future values)
		(y[n+3])= -(p_fCoef[coefnum])*(y[n+2]) - (p_fCoef[coefnum+1])*(y[n+1])+ gain[((n-2)/3)+1]*((x[n]) + (p_fCoef[coefnum+2])*(x[n-1]) + (p_fCoef[coefnum+3])*(x[n-2]));
	}
}

void Shiftleftdata(float *data_In, int size)
{
	for (int i = 1; i < size; i++)
			{
				data_In[i-1] = data_In[i];
			}
}

// for even orders above 2nd order only with a final gain of 1
float Usefilter(float *Filter_in, float*filter_temp, float*filter_coef, short ordernum, float*filtergain)
{
	// calculate amount of iterations for loop
	short iteration= ((ordernum/2)-2);

	//filter_in is not in temp, so first apply a second order filter normally
	SecondOrderFilter(Filter_in,filter_coef,filter_temp,filtergain,0,0);
	//love u <3 (no homo)
	// the apply the other second orders
	if (iteration == 0)
	{
		SecondOrderFilter(filter_temp,filter_coef,filter_temp,filtergain,2,4);
	}
	else
	{
		for (int i = 0; i <= iteration; i++)
		{
			SecondOrderFilter(filter_temp,filter_coef, filter_temp,filtergain, i*3+2,i*4+4);
		}
	}
	return(filter_temp[(ordernum/2*3)-1]);
}

/*
// function to initialize all the filters
void Intitialize_filters(filtertype eightOrderLPF,filtertype fourthOrderNotch,filtertype eightOrderHPF)
{
	// first we make all the parameters for each filters and set the temporary values to zero

	// low-pass filter
	eightOrderLPF.filterout =(float*)calloc(3,sizeof(float));
	eightOrderLPF.Filtertemp = (float*)calloc(12,sizeof(float));
	eightOrderLPF.coef = coef_lowpass;
	eightOrderLPF.filtergain = gain_lowpass;
	eightOrderLPF.ordernum = 8;

	// notch filter
	fourthOrderNotch.filterout =(float*)calloc(3,sizeof(float));
	fourthOrderNotch.Filtertemp = (float*)calloc(12,sizeof(float));
	fourthOrderNotch.coef = coef_notch;
	fourthOrderNotch.filtergain = gain_notch;
	fourthOrderNotch.ordernum = 4;

	// high-pass filter
	eightOrderHPF.filterout =(float*)calloc(3,sizeof(float));
	eightOrderHPF.Filtertemp = (float*)calloc(12,sizeof(float));
	eightOrderHPF.coef = coef_lowpass;
	eightOrderHPF.filtergain = gain_lowpass;
	eightOrderHPF.ordernum = 8;
}
*/
