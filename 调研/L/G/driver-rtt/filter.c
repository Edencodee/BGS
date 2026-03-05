#include "main.h"

/* 1. kalman variable ----------------------------------------------------------------------- */
volatile float g_xLast[KFN] = {0.0f};
volatile float g_pLast[KFN] = {0.0f};
volatile int g_measCnt[KFN] = {0};
volatile float g_rawPP[KFN] = {250.0f, 25.0f, 15.0f, 10.0f};
/* 2. iir variable -------------------------------------------------------------------------- */
#define FC_5 1
#if FC_5
const float b[3] = {0.0009446f, 0.0018893f, 0.0009446f};				/* 500:fs  5:fc N:2 */
const float a[3] = {1.0f, -1.9111970f, 0.9149758f};
#elif FC_3
const float b[3] = {0.00034604f, 0.00069208f, 0.00034604f};				/* 500:fs  3:fc N:2 */
const float a[3] = {1.0f, -1.94669754f, 0.94808171f};
#elif FC_7
const float b[3] = {0.00182013f, 0.00364026f, 0.00182013f};				/* 500:fs  7:fc N:2 */
const float a[3] = {1.0f, -1.87574557f, 0.88302609f};
#elif FC_10
const float b[3] = {0.00362168f, 0.00724336f, 0.00362168f};				/* 500:fs 10:fc N:2 */
const float a[3] = {1.0f, -1.82269493f, 0.83718165f};
#elif FC_20
const float b[3] = {0.01335920f, 0.02671840f, 0.01335920f};				/* 500:fs 20:fc N:2 */
const float a[3] = {1.0f, -1.64745998f, 0.70089680f};
#elif FC_35
const float b[3] = {0.03657483682f, 0.07314967364f, 0.03657483682f};	/* 500:fs 20:fc N:2 */
const float a[3] = {1.0f, -1.390895247f, 0.5371946096f};
#else
const float b[3] = {0.06745528f, 0.13491055f, 0.06745528f};				/* 500:fs 20:fc N:2 */
const float a[3] = {1.0f, -1.14298046f, 0.41280159f};
#endif

static float fx[3] = {700.0f, 700.0f, 700.0f};
static float fy[3] = {700.0f, 700.0f, 700.0f};
/* 3. kalmann filter ------------------------------------------------------------------------ */
static int ChangeCheck(float last, float val, float diff, int index)
{
	static bool s_isChangeLastP0[KFN] = {false};
	static bool s_isChangeLastN0[KFN] = {false};
	static bool s_isChangeLastP1[KFN] = {false};
	static bool s_isChangeLastP2[KFN] = {false};
	static bool s_isChangeLastN1[KFN] = {false};
	static bool s_isChangeLastN2[KFN] = {false};
	static bool s_isChangeLastP3[KFN] = {false};
	static bool s_isChangeLastP4[KFN] = {false};
	static bool s_isChangeLastP5[KFN] = {false};
	static bool s_isChangeLastN3[KFN] = {false};
	static bool s_isChangeLastN4[KFN] = {false};
	static bool s_isChangeLastN5[KFN] = {false};

	float temp = last - val;

	if (index < kF3) {
//		bool isChangeP1 = false;
//		bool isChangeN1 = false;
		bool isChangeP1 = (temp >  diff / 1.25f);
		bool isChangeN1 = (temp < -diff / 1.25f);

		bool isChangeP2 = false;
		bool isChangeN2 = false;
		if (index < kF2) {
			isChangeP2 = (temp >  diff / 2.0f );
			isChangeN2 = (temp < -diff / 2.0f );
		}

		bool isChangeP3 = false;
		bool isChangeN3 = false;
		if (index < kF1) {
			isChangeP3 = (temp >  diff / 2.5f );
			isChangeN3 = (temp < -diff / 2.5f );
		}

		if ((isChangeP1 && s_isChangeLastP0[index]) || \
		    (isChangeN1 && s_isChangeLastN0[index]) || \
		    (isChangeP2 && s_isChangeLastP1[index] && s_isChangeLastP2[index]) || \
		    (isChangeN2 && s_isChangeLastN1[index] && s_isChangeLastN2[index]) || \
		    (isChangeP3 && s_isChangeLastP3[index] && s_isChangeLastP4[index] && s_isChangeLastP5[index]) || \
		    (isChangeN3 && s_isChangeLastN3[index] && s_isChangeLastN4[index] && s_isChangeLastN5[index])) {
			s_isChangeLastP0[index] = false;
			s_isChangeLastN0[index] = false;
			s_isChangeLastP1[index] = false;
			s_isChangeLastP2[index] = false;
			s_isChangeLastN1[index] = false;
			s_isChangeLastN2[index] = false;
			s_isChangeLastP3[index] = false;
			s_isChangeLastP4[index] = false;
			s_isChangeLastP5[index] = false;
			s_isChangeLastN3[index] = false;
			s_isChangeLastN4[index] = false;
			s_isChangeLastN5[index] = false;
			return -1;
		}

		s_isChangeLastP0[index] = isChangeP1;
		s_isChangeLastN0[index] = isChangeN1;
		s_isChangeLastP2[index] = s_isChangeLastP1[index];
		s_isChangeLastP1[index] = isChangeP2;
		s_isChangeLastN2[index] = s_isChangeLastN1[index];
		s_isChangeLastN1[index] = isChangeN2;
		s_isChangeLastP5[index] = s_isChangeLastP4[index];
		s_isChangeLastP4[index] = s_isChangeLastP3[index];
		s_isChangeLastP3[index] = isChangeP3;
		s_isChangeLastN5[index] = s_isChangeLastN4[index];
		s_isChangeLastN4[index] = s_isChangeLastN3[index];
		s_isChangeLastN3[index] = isChangeN3;
	}

	return fabsf(temp) > diff ? 1 : 0;
}

static void KalmanInit(float val, int index)
{
	g_xLast[index] = val;
	g_pLast[index] = 1.0f;
}

float KalmanFilter(float val, float Q, float R, int index)
{
	float xMid = g_xLast[index];
	float pMid = g_pLast[index] + Q;
	float div = pMid + R;
//	if (fabsf(div) < 1e-6f) {
//		return g_xLast[index];
//	}
	float kg = pMid / div;
	float xNow = xMid + kg * (val - xMid);
	float pNow = (1 - kg) * pMid;

	g_pLast[index] = pNow;
	g_xLast[index] = xNow;

	return xNow;
}
void KalmanFilterS(float* val, float Q, float R, int index)
{
	static float s_last[KFN] = {0.0f};
	static bool s_isFiltInit[KFN] = {true};
	float temp = *val;

	if (index == kF4) {
		if (ChangeCheck(s_last[index], temp, 10.0f, index)) {
			KalmanInit(temp, index);
		}
	} else if (index == kF0) {
		int change = ChangeCheck(s_last[index], temp, g_rawPP[index], index);
		if (change == -1) {
			temp = (temp + s_last[index]) / 2;
		}
		if (change != 0) {
			static int32_t s_innerCnt = 0;
			s_innerCnt++;

			KalmanInit(temp, kF0);
			s_isFiltInit[kF0] = true;
		}
		if (change == 1) {
			g_measCnt[kF0] = 0;
			g_measCnt[kF1] = 0;
			g_measCnt[kF2] = 0;
			g_measCnt[kF3] = 0;
		}
	} else if (index == kF1) {
		if (s_isFiltInit[kF0]) {
			s_isFiltInit[kF0] = false;
			KalmanInit(temp, kF1);
			s_isFiltInit[kF1] = true;
		} else if (ChangeCheck(s_last[index], temp, g_rawPP[kF1], index) != 0) {
			temp = (temp + s_last[index]) / 2;
			KalmanInit(temp, kF1);
			s_isFiltInit[kF1] = true;
		}
	} else if (index == kF2) {
		if (s_isFiltInit[kF1]) {
			s_isFiltInit[kF1] = false;
			KalmanInit(temp, kF2);
			s_isFiltInit[kF2] = true;
		} else if (ChangeCheck(s_last[index], temp, g_rawPP[kF2], index) != 0) {
			temp = (temp + s_last[index]) / 2;
			KalmanInit(temp, kF2);
			s_isFiltInit[kF2] = true;
		}
	} else if (index == kF3)  {
		if (s_isFiltInit[kF2]) {
			s_isFiltInit[kF2] = false;
			KalmanInit(temp, kF3);
		} else if (ChangeCheck(s_last[index], temp, g_rawPP[kF3], index) != 0) {
			temp = (temp + s_last[index]) / 2;
			KalmanInit(temp, kF3);
		}
	}

	*val = KalmanFilter(temp, Q, R, index);
	s_last[index] = *val;
}
/* 4. IIR filter --------------------------------------------------------------------------- */
void IIR_Init (float val)
{
	fx[2] = val;
	fx[1] = val;
	fx[0] = val;
	fy[0] = val;
	fy[1] = val;
	fy[2] = val;
}

float IIR_Filter (float val)
{
	fx[2] = fx[1];
	fx[1] = fx[0];
	fx[0] = val;

	fy[0] = fy[1];
	fy[1] = fy[2];

	fy[2] = b[0] * (fx[0] + fx[2] + (fx[1] * 2.0f)) \
	        - a[1] * fy[1] - a[2] * fy[0];

	return fy[2];
}
