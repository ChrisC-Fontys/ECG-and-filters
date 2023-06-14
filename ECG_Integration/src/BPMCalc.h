/*
 * BPMCalc.h
 *
 *  Created on: 16 mei 2023
 *      Author: Chris
 */

#ifndef SRC_BPMCALC_H_
#define SRC_BPMCALC_H_



#endif /* SRC_BPMCALC_H_ */

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
/***************************** TYPEFDEFS ***********************************/


/***************************** CONSTANTS ***********************************/
#define MINUTE 60
/************************** Function Prototypes ****************************/

void SetPeakThreshold(int p_threshold);
int AutosetPeakThreshold(int p_percentage, int p_secondsActive, float f);
int PeakDetection(int p_data, int p_avgAmount);
void MeasurementAveraging(unsigned long *p_avgArray, int p_avgAmount);

float GetFrequency(unsigned long p_timeDelta);
int GetBPM(unsigned long p_timeDelta);
