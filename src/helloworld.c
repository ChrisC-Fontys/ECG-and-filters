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

//XGpio dac;

/************************** Variable Definitions ****************************/
float *arrayECG;

// make structures for all filter types
struct filtertype LPF;
struct filtertype Notch;
struct filtertype HPF;

// 2nd order A							// 2nd order B							2nd order C								2nd order D
float coef_lowpass[16] 	= {-1.79396184525177,0.886283112007014,2,1,-1.62340569764100,0.706949765682682,2,1,-1.51329076583890,0.591168074568205,2,1,-1.45970625437687,0.534825984961611,2,1};
	// gain A									// gain B									// gain C									// gain D
float gain_lowpass[4]  	= {0.023080316688810567637979431765415938571 ,0.020886017010420438594353598205088928808 ,0.019469327182325198155599110805269447155 ,0.018779932646185860944942902506227255799};

	// 2nd order A												     // 2nd order B
float coef_notch[8]		= {-1.92271809793349013,0.992520964225917846,-1.93187813428560307,1,-1.9267332485410309,0.992724132428439709,-1.93187813428560307,1};
	// gain A			 // gain B
float gain_notch[2]		= {0.996304442806574264,0.996304442806574264};

	// 2nd order A									// 2nd order B									// 2nd order C									//2nd order D
float coef_highpass[16] = {-1.99928185537008662,0.999285212575926618,-2,1,-1.99796244598246098,0.997965800972740902,-2,1,-1.99695378965628279,0.996957142952821118,-2,1,-1.99640833339891732,0.996411685779522438,-2,1};
	// gain A			// gain B			 // gain C				// gain D
float gain_highpass[4]	= {0.999641766986503311,0.998982061738800442,0.998477733152275948,0.99820500479460994};

typedef struct filtertype
{
	float *Filtertemp;
	float *filterout;
	short ordernum;
	float *coef;
	float *filtergain;
}filtertype;


/****************************************************************************/
/**************************        MAIN         *****************************/
/****************************************************************************/

int main()
{

	// First, we make a array to store the values from ADC
	arrayECG =(float*)calloc(3,sizeof(float));

	// the we initialize all the filters
	//Intitialize_filters(LPF,Notch,HPF);
	// first we make all the parameters for each filters and set the temporary values to zero

	// low-pass filter
	LPF.filterout =(float*)calloc(3,sizeof(float));
	LPF.Filtertemp = (float*)calloc(12,sizeof(float));
	LPF.coef = coef_lowpass;
	LPF.filtergain = gain_lowpass;
	LPF.ordernum = 8;

	// notch filter
	Notch.filterout =(float*)calloc(3,sizeof(float));
	Notch.Filtertemp = (float*)calloc(12,sizeof(float));
	Notch.coef = coef_notch;
	Notch.filtergain = gain_notch;
	Notch.ordernum = 4;

	// high-pass filter
	HPF.filterout =(float*)calloc(3,sizeof(float));
	HPF.Filtertemp = (float*)calloc(12,sizeof(float));
	HPF.coef = coef_highpass;
	HPF.filtergain = gain_highpass;
	HPF.ordernum = 8;

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

    	// get data from the XADC depending on the sampling frequency
    	arrayECG[2] = XAdcGeTSampledValue(SAMPLE_FREQUENCY);
    	// print the voltage
    	//printf("%0d.%03d Volts.\n\r", (int)(Voltagedata), XAdcFractionToInt(Voltagedata));

    	// first we filter the data of the ECG using a 8th order low-pass with a cutoff frequency of 60Hz
    	LPF.filterout[2] = Usefilter(arrayECG,LPF.Filtertemp,LPF.coef,LPF.ordernum,LPF.filtergain);
    	// print the voltage as check-up
    	//printf("%0d.%03d Volts.\n\r", (int)(eightOrderLPF.filterout[2]), XAdcFractionToInt(eightOrderLPF.filterout[2]));

      	// after that we implement the next filter, which is a 4th order Notch filter with a frequency range of 49-51 Hz
    	Notch.filterout[2] = Usefilter(LPF.filterout,Notch.Filtertemp,Notch.coef,Notch.ordernum,Notch.filtergain);

      	// Then we implement the last filter, which is a 8th order High-pass filter with a cutoff frequency of 0.35 Hz
    	HPF.filterout[2] = Usefilter(Notch.filterout,HPF.Filtertemp,HPF.coef,HPF.ordernum,HPF.filtergain);

       	// now we can shift all the data from each filter to save the previous result
       	// Shift data from low-pass filter
       	Shiftleftdata(LPF.Filtertemp,12);
       	Shiftleftdata(LPF.filterout,3);

       	// Shift data from notch filter
       	Shiftleftdata(Notch.Filtertemp,12);
       	Shiftleftdata(Notch.filterout,3);

       	// Shift data from high-pass filter
      	Shiftleftdata(HPF.Filtertemp,12);
       	Shiftleftdata(HPF.filterout,3);

    	//shift the ECG data so we can put new data into it
    	Shiftleftdata(arrayECG, 3);
	}

    print("Program finished \n\r");

    // Finally now that we do not need the values made from calloc anymore, we have to free them to prevent overflowing the system
    free(LPF.Filtertemp);
    free(LPF.filterout);

    free(Notch.Filtertemp);
    free(Notch.filterout);

    free(HPF.Filtertemp);
    free(HPF.filterout);

    free(arrayECG);

    cleanup_platform();
    return XST_SUCCESS;
}
