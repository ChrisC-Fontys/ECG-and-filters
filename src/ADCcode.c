#include <stdio.h>
#include "stdio.h"
#include "platform.h"
#include "xadcps.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"
#include "ADCcode.h"
#include "xtime_l.h"

#define printf xil_printf /* Small foot-print printf function */
static XAdcPs XAdcInst;      /* XADC driver instance */

/************************** Variable Definitions ****************************/
// select what adc channel you want to use (A0-A5)
const int adc_channel = 13;		// define all pins (A0 to A5)

XAdcPs *XAdcInstPtr = &XAdcInst;
XAdcPs_Config *ConfigPtr;

XTime newtime, previoustime;

u16 VccPintRawData;
float VccPintData;
float VCCPintData_Sampled;

/******************************* Functions **********************************/

//configure the XAdc
int XAdcConfig(u16 XAdcDeviceId){
	int Status;

	printf("	Entering the XAdc Polled Example. ");

	//Initialize the XAdc driver
	ConfigPtr = XAdcPs_LookupConfig(XAdcDeviceId);
	if (ConfigPtr == NULL){
		printf("XAdcPs_LookupConfig() returns XST_FAILURE");
		return XST_FAILURE;
	}

	XAdcPs_CfgInitialize(XAdcInstPtr, ConfigPtr, ConfigPtr->BaseAddress);

	Status = XAdcPs_SelfTest(XAdcInstPtr);
	if (Status != XST_SUCCESS) {
		printf("rXAdcPs_SelfTest() returns XST_FAILUREr");
		return XST_FAILURE;
	}

		XAdcPs_SetSequencerMode(XAdcInstPtr, XADCPS_SEQ_MODE_CONTINPASS);
		return XST_SUCCESS;
}
//get converted to voltage values from XAdc
float XAdcGetValues()
{
	XAdcPs_Reset(XAdcInstPtr);
	printf("\n\r   ----- New sample -----   \n\r");
	VccPintRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_AUX_MIN + adc_channel);
	VccPintData = XAdcPs_RawToVoltage(VccPintRawData)*1.12;
	return (VccPintData);
}

// get data from the XAdc depending on the sampling frequency
float XAdcGeTSampledValue(int samplefrequency)
{
	XTime_GetTime(&newtime);
	if ( newtime-previoustime >= COUNTS_PER_SECOND*1/samplefrequency)
	{
		XTime_GetTime(&previoustime);
		return VCCPintData_Sampled =  XAdcGetValues();
	}
}

// get the raw data from the XAdc
u16 XAdcGetRawData()
{
	XAdcPs_Reset(XAdcInstPtr);
	VccPintRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_AUX_MIN + adc_channel);
	return(VccPintRawData);
}


// convert the value into a decimal representation
int XAdcFractionToInt(float FloatNum)
{
	float Temp;

	Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}
	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
