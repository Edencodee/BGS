#include "main.h"

__IO static int g_rawDiff = 100;
__IO static int32_t g_ambientSize = 8;
__IO static int32_t g_status = 0;
__IO static int32_t g_measCnt = -1;

__IO static int g_xLast = 0;
__IO static int g_pLast = 10;

static inline int ChangeCheck(int last, int val, int diff, bool isPrecies) 
{
	int temp = last - val;
	
	static bool s_isChangeLastP0 = false;
	static bool s_isChangeLastN0 = false;
	static bool s_isChangeLastP1 = false;
	static bool s_isChangeLastP2 = false;
	static bool s_isChangeLastN1 = false;
	static bool s_isChangeLastN2 = false;
	static bool s_isChangeLastP3 = false;
	static bool s_isChangeLastP4 = false;
	static bool s_isChangeLastP5 = false;
	static bool s_isChangeLastN3 = false;
	static bool s_isChangeLastN4 = false;
	static bool s_isChangeLastN5 = false;
	
	int level1, level2, level3;
	if (isPrecies) {
		level1 =  (diff << 2) / 5;
		level2 =  (diff << 1) / 5;
		level3 =   diff / 5;
	} else {
		level1 =  (diff << 3) / 5;
		level2 =  (diff << 2) / 5;
		level3 =  (diff << 1) / 5;
	}
	
	bool isChangeP1 = (temp >  level1);
	bool isChangeN1 = (temp < -level1);
	bool isChangeP2 = (temp >  level2);
	bool isChangeN2 = (temp < -level2);
	bool isChangeP3 = (temp >  level3);
	bool isChangeN3 = (temp < -level3);
	
	if ((isChangeP1 && s_isChangeLastP0) || \
		(isChangeN1 && s_isChangeLastN0) || \
		(isChangeP2 && s_isChangeLastP1 && s_isChangeLastP2) || \
		(isChangeN2 && s_isChangeLastN1 && s_isChangeLastN2) || \
		(isChangeP3 && s_isChangeLastP3 && s_isChangeLastP4 && s_isChangeLastP5) || \
		(isChangeN3 && s_isChangeLastN3 && s_isChangeLastN4 && s_isChangeLastN5)) 
	{
		s_isChangeLastP0 = false;
		s_isChangeLastN0 = false;
		s_isChangeLastP1 = false;
		s_isChangeLastP2 = false;
		s_isChangeLastN1 = false;
		s_isChangeLastN2 = false;
		s_isChangeLastP3 = false;
		s_isChangeLastP4 = false;
		s_isChangeLastP5 = false;
		s_isChangeLastN3 = false;
		s_isChangeLastN4 = false;
		s_isChangeLastN5 = false;
		
//		if (isPrecies) {
			return -1;
//		}
	}
	
	s_isChangeLastP0 = isChangeP1;
	s_isChangeLastN0 = isChangeN1;
	s_isChangeLastP2 = s_isChangeLastP1;
	s_isChangeLastP1 = isChangeP2;
	s_isChangeLastN2 = s_isChangeLastN1;
	s_isChangeLastN1 = isChangeN2;
	s_isChangeLastP5 = s_isChangeLastP4;
	s_isChangeLastP4 = s_isChangeLastP3;
	s_isChangeLastP3 = isChangeP3;
	s_isChangeLastN5 = s_isChangeLastN4;
	s_isChangeLastN4 = s_isChangeLastN3;
	s_isChangeLastN3 = isChangeN3;
	
	
	int abs = temp > 0? temp : -temp;
	return abs > diff ? 1 : 0;
}

static inline void FilterInit(int val)
{
	g_xLast = val;
	g_pLast = 0;
}

static int KalmanFilter(int val, int Q, int R)
{
	int xMid = g_xLast;
	int pMid = g_pLast + Q;
	int kg = (pMid << 10) / (pMid + R);
	int xNow;
	int temp = val - xMid;
	if (temp > 0) {
		xNow = xMid + (kg *  temp >> 10);
	} else {
		xNow = xMid - (kg * -temp >> 10);
	}
//	int xNow = xMid + (kg * (val - xMid) / 1024);
	int pNow = (1024 - kg) * pMid >> 10;

	g_pLast = pNow;
	g_xLast = xNow;

	return xNow;
}	

void KalmanFilterS(int* val, int Q, int R) 
{
	static int s_last = 0;
	static int s_lastTemp = 0;
	static int s_lastStatus = 0;
	static bool s_lastAmbEn = true;
	static int s_lastChange = 0;
	int temp = *val;
	
	if (g_rawDiff < BG_NOISE) {
		g_rawDiff = BG_NOISE;
	}
	
	bool ambEn = ((g_status == 0) || (temp < LONG_DST));
	int diff = (temp > s_lastTemp)? (temp - s_lastTemp) : (s_lastTemp - temp);
	bool ambient = (ambEn && ((diff > (s_lastTemp >> 2)) || (diff > (temp >> 2))));
	
	int change = ChangeCheck(s_last, temp, g_rawDiff, ambEn);
	if (change == -1) {
		temp = (temp + s_last) >> 1;
	}
//	printf("%d\t", change);

	bool isInit = false;
	if ((change != 0) || (s_lastStatus != g_status) || ambient) {		
		if (ambEn || s_lastAmbEn) {
			g_rawDiff = temp * NOISE_RATIO;
			FilterInit(temp);
			isInit = true;
		} else {
			if ((change > 0) && (s_lastChange > 0)) {
				g_rawDiff = 1000;
				FilterInit((temp + s_lastTemp) >> 1);
				isInit = true;
				g_measCnt = 0;
			}
			if (change < 0) {
				g_rawDiff = 1000;
				FilterInit(temp);
				isInit = true;
				g_measCnt = 0;
			}
		}
	}
	s_lastChange = change;
	s_lastTemp = temp;
	s_lastStatus = g_status;
	s_lastAmbEn = ambEn;
	
	if (!isInit) {
		*val = KalmanFilter(temp, Q, R);
	}
	s_last = *val; 
}

void GetPP (uint16_t sig)
{
	static uint16_t s_max, s_min;
	
	if (g_measCnt > 0) {
		if (++g_measCnt > g_ambientSize) {
			g_measCnt = -1;
//					g_rawDiff = 12 * (s_max - s_min) >> 3;
			g_rawDiff = 10 * (s_max - s_min) >> 3;
		} else {
			if (sig > s_max) {
				s_max = sig;
			} 
			if (sig < s_min) {
				s_min = sig;
			}
		}
	} else if (g_measCnt == 0) {
		g_measCnt++;
		s_max = sig;
		s_min = sig;
	}
}
