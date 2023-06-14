#include <stdlib.h>
#include "stdio.h"
#include "platform.h"
#include "xil_printf.h"
#include "xadcps.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"
#include "ADCcode.h"
#include "xtime_l.h"
#include "BPMCalc.h"
#include "Detection.h"
#include "xgpio.h"



//unsigned long long timeDelta;
float TimeSeconds;


XGpio LED, buzzer;

void Detectionsystem(int bpm,unsigned long long timeDelta)
{
	XGpio_Initialize(&buzzer,XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_Initialize(&LED,XPAR_AXI_GPIO_1_DEVICE_ID);
	XGpio_SetDataDirection(&LED, 1, 0x0);
	XGpio_SetDataDirection(&buzzer, 1, 0x0);
	TimeSeconds = timeDelta/(float)COUNTS_PER_SECOND;
	//printf("periode tussen piek: %f\n\r", TimeSeconds);
	//printf("periode van 1 in counts: %llu\n\r", timeDelta);
	float upperboundry = (60/(float)bpm)*1.2;
	float lowerboundry = (60/(float)bpm)*0.8;
	//printf("uperboundry %f\n\r",upperboundry);
 if ((bpm < 45) | (bpm > 120)){
	 if (bpm <60){
		 if (bpm != 0){
		 printf("BPM too low seek medical attention \n");
		 XGpio_DiscreteWrite(&buzzer,1,1);
		 	 XGpio_DiscreteWrite(&LED,1,1);
		 }
	 }
	 else if(bpm > 100){
		 printf("BPM too high seek medical attention \n");
		 XGpio_DiscreteWrite(&buzzer,1,1);
		 XGpio_DiscreteWrite(&LED,1,1);
	 }
 }
 else{
	printf("BPM is healthy \n");
	     XGpio_DiscreteWrite(&buzzer,1,0);
	     XGpio_DiscreteWrite(&LED,1,0);
	 }

 if ((TimeSeconds>upperboundry) ){
	 printf("heart beat is irregular0 \n");
	     XGpio_DiscreteWrite(&buzzer,1,1);
	     XGpio_DiscreteWrite(&LED,1,1);
 }
 else if((TimeSeconds<lowerboundry)){
	 printf("heart beat is irregular1 \n");
	 	     XGpio_DiscreteWrite(&buzzer,1,1);
	 	     XGpio_DiscreteWrite(&LED,1,1);
 }
 else{
	 printf("Heart beat is periodic \n");
	 	 XGpio_DiscreteWrite(&buzzer,1,0);
	 	 XGpio_DiscreteWrite(&LED,1,0);
 }
}
