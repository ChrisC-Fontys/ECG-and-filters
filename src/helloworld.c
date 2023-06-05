/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xadcps.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"
#include "ADCcode.h"
#include "xgpio.h"
#include "Filters.h"
#include "stdlib.h"
/************************** Constant Definitions ****************************/

#define XADC_DEVICE_ID 		XPAR_XADCPS_0_DEVICE_ID
#define XGPIO_DEVICE_ID		XPAR_XGPIO_0_DEVICE_ID

#define SAMPLE_FREQUENCY 1200 //sample frequency is in Hz

/***************** Macros (Inline Functions) Definitions ********************/

#define printf xil_printf /* Small foot-print printf function */

/************************** Function Prototypes *****************************/

XGpio dac;

/************************** Variable Definitions ****************************/
//u16 RawADCdata;

/****************************************************************************/
/**************************        MAIN         *****************************/
/****************************************************************************/

//int arrayA[]={2,3,4,1,3};
//int arrayB[]={4,5,1,2,2};
//int result[5];
int sum;

typedef struct filtertype
{
	float *Filtertemp;
	float filterout[3];
	short ordernum;
	float *coef;
	float *filtergain;
}filtertype;

float coef_lowpass[16] 	= {2,1,-1.79396184525177,0.886283112007014,2,1,-1.62340569764100,0.706949765682682,2,1,-1.51329076583890,0.591168074568205,2,1,-1.45970625437687,0.534825984961611};
float gain_lowpass[4]  	= {1,2,3,4};
//const float coef_notch[8]		= {-1.93187813428560,1,-1.92271809793349,0.992520964225918,-1.93187813428560,1,-1.92673324854103,0.992724132428440};
//const float coef_highpass[16]	= {-1,-2,-1.99928185537009,0.999285212575927,-1,-2,-1.99796244598246,0.997965800972741,-1,-2,-1.99695378965628,0.996957142952821,-1,-2,-1.99640833339892,0.996411685779522};
int main()
{
	float *arrayECG;
	struct filtertype eightOrderLPF;
	//struct filtertype SecondOrderNotch;
	// first we make all the array's for the filters and set them to zero
	eightOrderLPF.Filtertemp =(float*)calloc(12,sizeof(float));
	arrayECG =(float*)calloc(3,sizeof(float));
	eightOrderLPF.coef = coef_lowpass;
	eightOrderLPF.filtergain = gain_lowpass;
	eightOrderLPF.ordernum =8;
	init_platform();
	print("Starting program...\n\r");

    // configure the XADC
	int Status;
    Status = XAdcConfig(XADC_DEVICE_ID);
    if (Status != XST_SUCCESS) {
    	print("configuring XADC unsuccessful\n\r");
    	return XST_FAILURE;
    }

    while(1){
    	//shift the ECG data before putting new data into it
    	Shiftleftdata(arrayECG, 3);

    	// get data from the XADC depending on the sampling frequency
    	arrayECG[2] = XAdcGeTSampledValue(SAMPLE_FREQUENCY);

    	// now we filter the data of the ECG
    	eightOrderLPF.filterout[2] = Usefilter(arrayECG,eightOrderLPF.Filtertemp,eightOrderLPF.coef,eightOrderLPF.ordernum,eightOrderLPF.filtergain);
    	shiftleftdata(eightOrderLPF.filterout,3);
    	shiftleftdata(eightOrderLPF.Filtertemp,12);

	}

    print("Program finished \n\r");
    free(eightOrderLPF.Filtertemp);
    cleanup_platform();
    return XST_SUCCESS;
}

// print the voltage
    	//printf("%0d.%03d Volts.\n\r", (int)(Voltagedata), XAdcFractionToInt(Voltagedata));

//SecondOrderFilter(arrayECG,cof_lowpassA,outputlowpassA);
//Shiftleftdata(outputlowpassA,3);
// first we apply a 8th order lowpass filter with a steeper cutoff at ...Hz
// Cascade the 8th order lowpass filter into four 2nd order lowpass filters
//outputlowpassA[n]= -2*outputlowpassA[n-1] - outputlowpassA[n-2] + arrayECG[n] - 1.79396184525177*arrayECG[n-1] + 0.886283112007014*arrayECG[n-2];
//outputlowpassB[n]= -2*outputlowpassB[n-1] - outputlowpassB[n-2] + arrayECG[n] - 1.62340569764100*arrayECG[n-1] + 0.706949765682682*arrayECG[n-2];
//outputlowpassC[n]= -2*outputlowpassC[n-1] - outputlowpassC[n-2] + arrayECG[n] - 1.51329076583890*arrayECG[n-1] + 0.591168074568205*arrayECG[n-2];
//outputlowpassD[n]= -2*outputlowpassD[n-1] - outputlowpassD[n-2] + arrayECG[n] - 1.45970625437687*arrayECG[n-1] + 0.534825984961611*arrayECG[n-2];

//calculate the result
//resultlowpass[2] = (outputlowpassA[2]) * (outputlowpassB[2]) * (outputlowpassC[2]) * (outputlowpassD[2]);

// get the output data into their respective array
//Shiftleftdata(outputlowpassA,3);
//Shiftleftdata(outputlowpassB,3);
//Shiftleftdata(outputlowpassC,3);
//Shiftleftdata(outputlowpassD,3);

//Shiftleftdata(resultlowpass,3);
