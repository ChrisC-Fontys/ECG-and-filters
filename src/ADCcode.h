#ifndef ADCcode_H
#define ADCcode_H

int XAdcConfig(u16 XAdcDeviceId);
int XAdcFractionToInt(float FloatNum);

float XAdcGetValues();
float XAdcGeTSampledValue(int samplefrequency);

u16 XAdcGetRawData();

#endif  // ADCcode_H
