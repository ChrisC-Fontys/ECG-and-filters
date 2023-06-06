#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"
#include "Filters.h"

// define the order number for each filter
#define LPF_ordernumber 8
#define Notch_ordernumber 4
#define HPF_ordernumber 8

// set all the constants for each filter
const float LPF_b1	 =  2, LPF_b2 = 1;
const float LPF_A_a1 = -1.79396184525177,	LPF_A_a2 = 0.886283112007014, 	LPF_gain_A = 0.023080316688810567637979431765415938571;
const float LPF_B_a1 = -1.62340569764100,	LPF_B_a2 = 0.706949765682682, 	LPF_gain_B = 0.020886017010420438594353598205088928808;
const float LPF_C_a1 = -1.51329076583890,	LPF_C_a2 = 0.591168074568205,	LPF_gain_C = 0.019469327182325198155599110805269447155;
const float LPF_D_a1 = -1.45970625437687,	LPF_D_a2 = 0.534825984961611, 	LPF_gain_D = 0.018779932646185860944942902506227255799;

const float BRF_b1 	 = -1.93187813428560307,BRF_b2	 = 1;
const float BRF_A_a1 = -1.92271809793349013,BRF_A_a2 = 0.992520964225917846,BRF_gain_A = 0.996304442806574264;
const float BRF_B_a1 = -1.9267332485410309,	BRF_B_a2 = 0.992724132428439709,BRF_gain_B = 0.996304442806574264;

const float HPF_b1 	 = -2, HPF_b2 = 1;
const float HPF_A_a1 = -1.99928185537008662,HPF_A_a2 = 0.999285212575926618,HPF_gain_A = 0.999641766986503311;
const float HPF_B_a1 = -1.99796244598246098,HPF_B_a2 = 0.997965800972740902,HPF_gain_B = 0.998982061738800442;
const float HPF_C_a1 = -1.99695378965628279,HPF_C_a2 = 0.996957142952821118,HPF_gain_C = 0.998477733152275948;
const float HPF_D_a1 = -1.99640833339891732,HPF_D_a2 = 0.996411685779522438,HPF_gain_D = 0.998205004794609940;

//const float gain_lowpassA;

/************************** Variable Definitions ****************************/
// sets parameters for all filters
							// 2nd order A coefficients		// 2nd order B coefficients		// 2nd order C coefficients		// 2nd order D coefficients
float coef_lowpass[16] 	= {LPF_A_a1,LPF_A_a2,LPF_b1,LPF_b2,LPF_B_a1,LPF_B_a2,LPF_b1,LPF_b2,LPF_C_a1,LPF_C_a2,LPF_b1,LPF_b2,LPF_D_a1,LPF_D_a2,LPF_b1,LPF_b2};
float gain_lowpass[4]  	= {LPF_gain_A ,LPF_gain_B ,LPF_gain_C ,LPF_gain_D};
// 2nd order A coefficients		// 2nd order B coefficients		// 2nd order C coefficients
float coef_notch[8]		= {BRF_A_a1,BRF_A_a2,BRF_b1,BRF_b2,BRF_B_a1,BRF_B_a2,BRF_b1,BRF_b2};
float gain_notch[2]		= {BRF_gain_A,BRF_gain_B};

// 2nd order A coefficients		// 2nd order B coefficients		// 2nd order C coefficients		// 2nd order D coefficients
float coef_highpass[16] = {HPF_A_a1,HPF_A_a2,HPF_b1,HPF_b2,HPF_B_a1,HPF_B_a2,HPF_b1,HPF_b2,HPF_C_a1,HPF_C_a2,HPF_b1,HPF_b2,HPF_D_a1,HPF_D_a2,HPF_b1,HPF_b2};
	// gain A			// gain B			 // gain C				// gain D
float gain_highpass[4]	= {HPF_gain_A,HPF_gain_B,HPF_gain_C,HPF_gain_D};

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


// function to initialize all the filters
void Intitialize_filters()
{
	// first we make all the parameters for each filters and set the temporary values to zero
	LPF.ordernum = LPF_ordernumber;
	LPF.filterout =(float*)calloc(3,sizeof(float));
	LPF.Filtertemp = (float*)calloc((LPF.ordernum/2*3),sizeof(float));
	LPF.coef = coef_lowpass;
	LPF.filtergain = gain_lowpass;


	// notch filter
	Notch.filterout =(float*)calloc(3,sizeof(float));
	Notch.Filtertemp = (float*)calloc((Notch.ordernum/2*3),sizeof(float));
	Notch.coef = coef_notch;
	Notch.filtergain = gain_notch;
	Notch.ordernum = Notch_ordernumber;

	HPF.ordernum = HPF_ordernumber;
	// high-pass filter
	HPF.filterout =(float*)calloc(3,sizeof(float));
	HPF.Filtertemp = (float*)calloc((HPF.ordernum/2*3),sizeof(float));
	HPF.coef = coef_highpass;
	HPF.filtergain = gain_highpass;
}
