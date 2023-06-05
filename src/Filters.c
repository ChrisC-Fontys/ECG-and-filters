#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"

void SecondOrderFilter(float *x, float *p_fCoef,float *y, float*gain, short n,short coefnum)
{
	(y[n])= gain[(n-2)/3]*(-(p_fCoef[coefnum]) * (y[n-1]) - (p_fCoef[coefnum+1])* (y[n-2])+(x[n+3])+ (p_fCoef[coefnum+2])* (x[n+1]) + (p_fCoef[coefnum+3])* (x[n+1]));
}

void Shiftleftdata(float *data_In, int size)
{
	for (int i = 1; i < size; i++)
			{
				data_In[i-1] = data_In[i];
			}
}

// for even orders above 2nd order only
float Usefilter(float *Filter_in, float*filter_temp, float*filter_coef, short ordernum, float*filtergain)
{
	// calculate amount of iterations for loop
	short interation= (ordernum/2)-2;

	//filter_in is not in temp, so first apply a second order filter normally
	SecondOrderFilter(Filter_in,filter_coef,filter_temp,filtergain,2,0);

	// the apply the other second orders
	for (int i=0; i==interation; i++)
	{
		SecondOrderFilter(filter_temp,filter_coef, filter_temp,filtergain, i*3+2,i*4+4);
	}

	return(filter_temp[11]);
}
