#include "main.h"

VL53LX_Dev_t                   dev;
VL53LX_DEV                     Dev = &dev;

__IO uint16_t distance;

	int status;
	VL53LX_MultiRangingData_t MultiRangingData;
	VL53LX_CalibrationData_t pCalibrationData;
	VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
	uint8_t NewDataReady=0;
	int j;

void VL53L1CB_Init(void)
{
	uint8_t byteData;
	uint16_t wordData;
	Dev->i2c_slave_address = 0x52;

	VL53LX_RdByte(Dev, 0x010F, &byteData);
	VL53LX_RdByte(Dev, 0x0110, &byteData);
	VL53LX_RdWord(Dev, 0x010F, &wordData);

	status = VL53LX_WaitDeviceBooted(Dev);
	status = VL53LX_DataInit(Dev);
	status = VL53LX_SetCalibrationData(Dev,&pCalibrationData);
	status = VL53LX_SetDistanceMode(Dev,VL53LX_DISTANCEMODE_LONG);
	status = VL53LX_SetXTalkCompensationEnable(Dev, ENABLE);
	status = VL53LX_SetMeasurementTimingBudgetMicroSeconds(Dev,30000);
	status = VL53LX_StartMeasurement(Dev);

	if(status){
		printf("VL53LX_StartMeasurement failed: error = %d \n", status);
	while(1);
	}
}
/* Ranging loop*/
int MeasureTask(void)
{
	status = VL53LX_GetMeasurementDataReady(Dev, &NewDataReady);
	delay_1ms(1); // 1 ms polling period, could be longer.
	if((!status)&&(NewDataReady!=0)){
		status = VL53LX_GetMultiRangingData(Dev, pMultiRangingData);
		if(j!=0)printf("\n                     ");
		printf("D=%5dmm", pMultiRangingData->RangeData[j].RangeMilliMeter);
		printf ("\n");
		distance = pMultiRangingData->RangeData[j].RangeMilliMeter; g_sreg.distance = distance;
		if (status==0){
			status = VL53LX_ClearInterruptAndStartMeasurement(Dev);
			FeedDog();
		}
//		/* signal noise */
//		GetPP(g_sreg.distance);
//		/* filter */
//		int dist = g_sreg.distance;
//		bool ambEn = (g_sreg.rangeStatus == 0) || (g_sreg.distance < LONG_DST);
//		KalmanFilterS(&dist, FILT_Q, ambEn ? FILT_R : (FILT_R << 4));
//		g_sreg.distance = dist;
//		printf("D2=%5dmm", g_sreg.distance);
	}
	return 0;
}
