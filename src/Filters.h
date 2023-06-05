#ifndef SRC_FILTERS_H_
#define SRC_FILTERS_H_



#endif /* SRC_FILTERS_H_ */


float SecondOrderFilter( float p_dataArray[3], float filterCoefficients[5]);
void Shiftleftdata(float data_In[], int size);

float Usefilter(float *Filter_in, float*filter_temp, float*filter_coef, short ordernum, float*filtergain);
