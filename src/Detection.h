#ifndef DetectionH
#define DetectionH

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

void Detectionsystem(int bpm,unsigned long long timeDelta);

#endif
