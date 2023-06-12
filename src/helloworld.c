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
#include "xparameters.h"
#include "sleep.h"
#include "xstatus.h"
#include "stdlib.h"
#include "xtime_l.h"

#include "xadcps.h"
//#include "xgpio.h"

#include "Filters.h"
#include "ADCcode.h"
#include "BPMCalc.h"
#include "Detection.h"
/************************** Constant Definitions ****************************/

#define XADC_DEVICE_ID 		XPAR_XADCPS_0_DEVICE_ID

#define SAMPLE_FREQUENCY 1200 //sample frequency is in Hz

/***************** Macros (Inline Functions) Definitions ********************/

#define printf xil_printf /* Small foot-print printf function */

/************************** Function Prototypes *****************************/

float ECGfilters();
void free_all();

/************************** Variable Definitions ****************************/

float *arrayECG;
int lastBPM;
/****************************************************************************/
/**************************        MAIN         *****************************/
/****************************************************************************/

int main()
{
	// Make a array to store the values from XAdc
	arrayECG = (float*)calloc(3,sizeof(float));

	const int avgAmount = 2;
	//AutosetPeakThreshold(10, 20);

	init_platform();
	print("Starting program...\n\r");

	// initialize all the filters
	Intitialize_filters();

    // configure the XAdc
	int Status;
    Status = XAdcConfig(XADC_DEVICE_ID);
    if (Status != XST_SUCCESS) {
    	print("configuring XADC unsuccessful\n\r");
    	return XST_FAILURE;
    }

    //AutosetPeakThreshold(0.8, 10);
    SetPeakThreshold(3000);

    while(1){

    	arrayECG[2] = XAdcGeTSampledValue(SAMPLE_FREQUENCY);
    	//printf("%0d.%03d Volts.\n\r", (int)(Voltagedata), XAdcFractionToInt(Voltagedata));

    	float Filters_output = ECGfilters();

    	//printf("%0d.%03d Volts.\n\r", (int)(Filters_output), XAdcFractionToInt(Filters_output));

    	int Filters_out = XAdcPs_VoltageToRaw(Filters_output);

	//if periodic detection needs to occur, uncomment the following:
	//avgAmount = 1;
    	unsigned long averagedTimeDifference = PeakDetection(Filters_out, avgAmount);
    	//printf("averaged time difference: %llu\n\r", averagedTimeDifference);

    	int BPM = GetBPM(averagedTimeDifference);

    	//if (lastBPM != BPM)
    	//{
    		printf("BPM: %d\n\r", BPM);
    		lastBPM=BPM;
    	//}
	if (avgAmount == 1) {
		Detectionsystem(BPM, avgeragedTimeDifference);
	}
    	Shiftleftdata(arrayECG, 3);
	}

    print("Program finished \n\r");

    free_all();
    cleanup_platform();
    return XST_SUCCESS;
}

// Filters the measured data and puts the ouput in a variable
float ECGfilters()
{
	// first we filter the data of the ECG using a 8th order low-pass with a cutoff frequency of 60Hz
	LPF.filterout[2] = Usefilter(arrayECG,LPF.Filtertemp,LPF.coef,LPF.ordernum,LPF.filtergain);

	// after that we implement the next filter, which is a 4th order Notch filter with a frequency range of 49-51 Hz
	Notch.filterout[2] = Usefilter(LPF.filterout,Notch.Filtertemp,Notch.coef,Notch.ordernum,Notch.filtergain);

	// Then we implement the last filter, which is a 8th order High-pass filter with a cutoff frequency of 0.35 Hz
	HPF.filterout[2] = Usefilter(Notch.filterout,HPF.Filtertemp,HPF.coef,HPF.ordernum,HPF.filtergain);

	// now we can shift all the data from each filter to save the previous result
	// Shift data from low-pass filter
    Shiftleftdata(LPF.Filtertemp,(LPF.ordernum/2*3));
   	Shiftleftdata(LPF.filterout,3);

  	// Shift data from notch filter
	Shiftleftdata(Notch.Filtertemp,(Notch.ordernum/2*3));
	Shiftleftdata(Notch.filterout,3);

	// Shift data from high-pass filter
	Shiftleftdata(HPF.Filtertemp,(HPF.ordernum/2*3));
	Shiftleftdata(HPF.filterout,3);

	return(HPF.filterout[2]);
}

//release all allocated memory in heap to avoid memory leaks
void free_all()
{
	free(LPF.Filtertemp);
	free(LPF.filterout);

	free(Notch.Filtertemp);
	free(Notch.filterout);

	free(HPF.Filtertemp);
	free(HPF.filterout);

	free(arrayECG);
}
