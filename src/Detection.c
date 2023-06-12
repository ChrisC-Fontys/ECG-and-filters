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



unsigned long timeDelta;
float TimeSeconds;
XGpio LED, buzzer;

void Detectionsystem(int bpm,unsigned long long timeDelta){
    XGpio_Initialize(&buzzer,XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_Initialize(&LED,XPAR_AXI_GPIO_1_DEVICE_ID);
    TimeSeconds = timeDelta/COUNTS_PER_SECOND;



 if ((bpm < 60) | (bpm > 100)){
     if (bpm <60){
         printf("BPM too low seek medical attention \n");
         XGpio_DiscreteWrite(&buzzer,1,1);
              XGpio_DiscreteWrite(&LED,1,1);
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

 if ((TimeSeconds>(bpm/60)1.2 )|(TimeSeconds<(bpm/60)0.8)){
     printf("heart beat is irregular");
     XGpio_DiscreteWrite(&buzzer,1,1);
          XGpio_DiscreteWrite(&LED,1,1);
 }
 else{
     printf("Heart beat is periodic");
     XGpio_DiscreteWrite(&buzzer,1,0);
     XGpio_DiscreteWrite(&LED,1,0);
 }
}
