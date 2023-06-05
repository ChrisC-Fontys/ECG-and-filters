#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"

void SecondOrderFilter(float *x, float *p_fCoef,float *y, float*gain, short n,short coefnum)
{
	//only the first 2nd order filter is different (not in the temp array)
	if (n==0)
	{  	n=2;
		// first scale the input value
		//x[n] = gain[0]*x[n];
		//float test = x[n];
		// then apply the difference equation of the filter
		(y[n])= -(p_fCoef[coefnum]) * (y[n-1]) - (p_fCoef[coefnum+1])* (y[n-2])+(x[n])+ (p_fCoef[coefnum+2])* (x[n-1]) + (p_fCoef[coefnum+3])* (x[n-2]);
	}
	else
	{
		// scale the input value
		//x[n] = gain[((n-2)/3)+1] * x[n];
		//float test = x[n];
		// apply the difference equation of the filter (y[n+2], y[n+3], y[n+1] only because it is the same array. We do not try to use future values)
		(y[n+3])= (-(p_fCoef[coefnum]) * (y[n+2]) - (p_fCoef[coefnum+1])* (y[n+1])+ (x[n])+ (p_fCoef[coefnum+2])* (x[n-1]) + (p_fCoef[coefnum+3])* (x[n-2]));
	}
	//incorrect
	//(y[n])= gain[(n-2)/3]*(-(p_fCoef[coefnum]) * (y[n-1]) - (p_fCoef[coefnum+1])* (y[n-2])+(x[n+3])+ (p_fCoef[coefnum+2])* (x[n+1]) + (p_fCoef[coefnum+3])* (x[n+2]));
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

	// the apply the other second orders
	for (int i=0; i <= iteration; i++)
	{
		SecondOrderFilter(filter_temp,filter_coef, filter_temp,filtergain, i*3+2,i*4+4);
	}

	return(filter_temp[11]);
}
