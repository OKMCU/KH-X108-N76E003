/**
  ******************************************************************************
  * @file    app_spray.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    20-SEP-2017
  * @brief   
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 OKMCU</center></h2>
  *
  * Licensed under OKMCU Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.okmcu.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "..\hal\hal_spray.h"
#include "..\hal\hal_freqhop.h"
#include "..\hal\hal_fan.h"
#include "..\app\app_spray.h"
#include "..\app\app_tasksched.h"
#include "..\app\app_events.h"
#include "..\app\app_water_chk.h"
#include "..\app\app_freqhop.h"

#include <stdio.h>

/** @addtogroup Template_Project
  * @{
  */

#if APP_SPRAY_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_SPRAY_WC_ALGO_EN            1
/* Private macro -------------------------------------------------------------*/
#define APP_SPRAY_ON_TIME               (uint32_t)5     //in ms. 1.7MHz on time in one period. Larger on time, larger power consumption
#define APP_SPRAY_OFF_TIME              (uint32_t)13    //in ms. 1.7MHz off time in one period. Larger off time, lower power consumption.
#define APP_SPRAY_WC_TIME               (uint32_t)1     //in ms. Water check time. 
#define APP_SPRAY_WC_BASELINE_DELTA_TH  (uint16_t)1000
#define APP_SPRAY_WC_TH_TRANSIENT_HI    (uint16_t)3440  //no water, if higher than 4.2V
#define APP_SPRAY_WC_TH_TRANSIENT_LO    (uint16_t)1064  //no water, if lower  than 1.3V 
#define APP_SPRAY_WC_TH_IIR_HI          (uint16_t)3440  //no water, if higher than 4.2V
#define APP_SPRAY_WC_TH_IIR_LO          (uint16_t)1064  //no water, if lower  than 1.3V 
#define APP_SPRAY_WC_TH_ABNORMAL_CNT_UNLOCK    (uint8_t)30     //no water, if abnormal count higher than this value, when frequency is not confirmed
#define APP_SPRAY_WC_TH_ABNORMAL_CNT_LOCK      (uint8_t)40     //no water, if abnormal count higher than this value, when frequency is confirmed
#define APP_SPRAY_WC_BASELINE_MIN_TRAIN (uint16_t)200   //minmimum training times for baseline
#define APP_SPRAY_WC_IIR_MIN_TRAIN      (uint32_t)100   //minimum IIR filter training times.
#define APP_SPRAY_WC_RESULT_BUF_SIZE    (uint8_t)100    //100 pulses last for about 1.5sec, should be enough to detect the no water tremble.
#define APP_SPRAY_WC_TH_DELTAINDEX_HI   (uint16_t)1500  //no water, if (max -min) > 1500, the lower the better but should not lead to false detection.
#define APP_SPRAY_WC_TH_DELTAINDEX_LO   (uint16_t)400   //no water, if (max -min) < 510, the larger the better but should not lead to false detection.
/* Private variables ---------------------------------------------------------*/
static xdata int8_t  sprayPowerCtrlTid = -1;
static xdata int8_t  stopWaterCheckTid = -1;
static xdata int8_t  intermissionModeTid = -1;
static xdata int8_t  minutesLeftTid = -1;
static xdata uint8_t minutesLeft = 0;
static xdata uint8_t onPeriodInSec = 0;
static xdata uint8_t offPeriodInSec = 0;
static xdata uint32_t resultCnt = 0;
static xdata uint16_t resultIIR = 0;
static xdata uint16_t baseline = 0;
static xdata uint16_t resultBuf[APP_SPRAY_WC_RESULT_BUF_SIZE] = {0};
static xdata uint8_t resultBufPtr = 0;
static xdata uint16_t dbgRecMinDeltaIndex = UINT16_MAX;
static xdata uint16_t dbgRecMaxDeltaIndex = 0;
static xdata uint8_t sprayOnTimeInMs = APP_SPRAY_ON_TIME;
static xdata uint8_t sprayOffTimeInMs = APP_SPRAY_OFF_TIME;
/* Private function prototypes -----------------------------------------------*/
static void appSprayTimerTask_StopCheckWater(void);
static void appSprayOnPeriod(void);
static void appSprayOffPeriod(void);
static bool appSprayIsWaterExist(void);
static void appSprayTimerTask_onPeriod(void);
static void appSprayTimerTask_offPeriod(void);
static void appSprayTimerTask_minutesCountDown(void);
static void appSprayOn(void);
static void appSprayOff(void);
/* Private functions ---------------------------------------------------------*/
static void appSprayTimerTask_StopCheckWater(void)
{
#if APP_WATER_CHK_EN > 0
    APP_WATERCHK_RESULT_t *result;
#endif /* APP_WATER_CHK_EN > 0 */

    stopWaterCheckTid = -1;
#if APP_WATER_CHK_EN > 0
    appWaterChkStop();
    result = appWaterChkGetResult();
#endif /* APP_WATER_CHK_EN > 0 */

    resultBuf[resultBufPtr++] = result->avg;
    if(resultBufPtr >= APP_SPRAY_WC_RESULT_BUF_SIZE)
    {
        resultBufPtr = 0;
    }
    resultCnt++;
    
    resultIIR = (resultIIR == 0) ? result->avg : (uint16_t)(((uint32_t)resultIIR*31 + result->avg)>>5);
    if(resultCnt < APP_SPRAY_WC_BASELINE_MIN_TRAIN/10)
    {
        baseline = (baseline == 0) ? result->avg : (uint16_t)(((uint32_t)baseline*7 + result->avg)>>3);
    }
    else
    {
        if(baseline > result->avg)
        {
            baseline--;
        }
        else if(baseline < result->avg)
        {
            baseline++;
        }
    }
    
    if(appSprayIsWaterExist() == FALSE)
    {
#if DEBUG_APP_SPRAY_EN > 0
        printf("Trigger no water event!\r\n");
#endif /* DEBUG_APP_SPRAY_EN > 0 */

#if APP_EVENTS_EN > 0
        appEventNoWater();
#endif /* APP_EVENTS_EN > 0 */
#if APP_FREQHOP_EN > 0
        if(appFreqHop_IsEnabled() == TRUE)
        {
            if(appFreqHop_LockFreq() == FALSE)
                appFreqHop_Reset();
        }
#endif /* APP_FREQHOP_EN > 0 */
        appSprayResetWaterChkData();
    }
    else
    {
#if APP_FREQHOP_EN > 0

        if(appFreqHop_IsEnabled() == TRUE)
        {
            if(appFreqHop_UpdateFreq() == TRUE)
            {
                appSprayResetWaterChkData();
            }
        }
#endif
    }
    
}



static void appSprayOnPeriod(void)
{

    halSprayOn();
    sprayPowerCtrlTid = appTaskSchedCreate((uint32_t)sprayOnTimeInMs, appSprayOffPeriod);
}

static void appSprayOffPeriod(void)
{
    halSprayOff();
#if APP_WATER_CHK_EN > 0
    appWaterChkStart();
#endif /* APP_WATER_CHK_EN > 0 */
    if(stopWaterCheckTid >= 0)
    {
        appTaskSchedDelete(stopWaterCheckTid);
    }
    stopWaterCheckTid = appTaskSchedCreate(APP_SPRAY_WC_TIME, appSprayTimerTask_StopCheckWater);
    sprayPowerCtrlTid = appTaskSchedCreate((uint32_t)sprayOffTimeInMs, appSprayOnPeriod);
}

static bool appSprayIsWaterExist(void)
{
    uint8_t i;
    uint16_t max = 0;
    uint16_t min = UINT16_MAX;
    uint8_t abnormalCnt = 0;
    int16_t delta = 0;
    uint8_t failId = 0;
    uint16_t deltaIndex;
    uint16_t effectiveCnt = 0;
    uint8_t abnormalCntTh;
    for(i = 0; i < APP_SPRAY_WC_RESULT_BUF_SIZE; i++)
    {
        if(resultBuf[i] != 0)
        {
            effectiveCnt++;
            if(resultBuf[i] > max)
            {
                max = resultBuf[i];
            }
            if(resultBuf[i] < min)
            {
                min = resultBuf[i];
            }
            if(resultBuf[i] > APP_SPRAY_WC_TH_TRANSIENT_HI || resultBuf[i] < APP_SPRAY_WC_TH_TRANSIENT_LO)
            {
                abnormalCnt++;
            }
        }
    }

    deltaIndex = max - min;

    if(appFreqHop_RoughTuneFinished() == TRUE)
    {
        if(sprayPowerCtrlTid >= 0)
        {
            if(resultCnt > APP_SPRAY_WC_IIR_MIN_TRAIN && resultCnt > APP_SPRAY_WC_BASELINE_MIN_TRAIN)
            {
                delta = resultIIR - baseline;
                if(delta < 0)
                {
                    delta = -delta;
                }
                if(delta > APP_SPRAY_WC_BASELINE_DELTA_TH)
                {
                    failId |= (1<<0);//0x01
                }
            }

            if(appFreqHop_FineTuneFinished() == TRUE)
            {
                if(resultCnt <= APP_SPRAY_WC_BASELINE_MIN_TRAIN)
                {
                    abnormalCntTh = appFreqHop_LockFreq() == TRUE ? APP_SPRAY_WC_TH_ABNORMAL_CNT_LOCK : APP_SPRAY_WC_TH_ABNORMAL_CNT_UNLOCK;
                    if(abnormalCnt > abnormalCntTh)
                    {
                        failId |= (1<<1);//0x02
                    }
                }
                else
                {
                    if(resultIIR > APP_SPRAY_WC_TH_IIR_HI|| resultIIR < APP_SPRAY_WC_TH_IIR_LO)
                    {
                        failId |= (1<<4);//0x10
                    }
                }
                
                if(resultCnt > APP_SPRAY_WC_RESULT_BUF_SIZE)
                {
                    dbgRecMinDeltaIndex = deltaIndex < dbgRecMinDeltaIndex ? deltaIndex : dbgRecMinDeltaIndex;
                    dbgRecMaxDeltaIndex = deltaIndex > dbgRecMaxDeltaIndex ? deltaIndex : dbgRecMaxDeltaIndex;
                    
                    if(appFreqHop_LockFreq() == FALSE)
                    {
                        if(deltaIndex < APP_SPRAY_WC_TH_DELTAINDEX_LO)
                        {
                            failId |= (1<<2);//0x04
                        }

                        if(deltaIndex > APP_SPRAY_WC_TH_DELTAINDEX_HI)
                        {
                            failId |= (1<<3);//0x08
                        }
                    }
                    else
                    {
                        dbgRecMinDeltaIndex = dbgRecMinDeltaIndex;
                        dbgRecMaxDeltaIndex = dbgRecMaxDeltaIndex;
                    }
                }
            }
        }
    }
    
    if(appFreqHop_Timeout() == TRUE)
    {
        failId |= (1<<6);//0x40
    }

    if(failId != 0)
    {
#if APP_SPRAY_WC_ALGO_EN > 0
        return FALSE;
#endif /* APP_SPRAY_WC_ALGO_EN > 0 */
    }
    
    return TRUE;
}

static void appSprayTimerTask_onPeriod(void)
{
    intermissionModeTid = appTaskSchedCreate((uint32_t)onPeriodInSec*1000, appSprayTimerTask_offPeriod);
    appSprayOn();
}

static void appSprayTimerTask_offPeriod(void)
{
    intermissionModeTid = appTaskSchedCreate((uint32_t)offPeriodInSec*1000, appSprayTimerTask_onPeriod);
    appSprayOff();
}

static void appSprayTimerTask_minutesCountDown(void)
{
    minutesLeftTid = -1;
    minutesLeft--;
    if(minutesLeft > 0)
    {
        minutesLeftTid = appTaskSchedCreate(60000, appSprayTimerTask_minutesCountDown);
    }
    else
    {
        if(intermissionModeTid >= 0)
        {
            appTaskSchedDelete(intermissionModeTid);
            intermissionModeTid = -1;
        }
        appSprayOff();
#if APP_EVENTS_EN > 0
        appEventNoWater();
#endif /* APP_EVENTS_EN > 0 */
    }
}

static void    appSprayOn   (void)
{
    if(sprayPowerCtrlTid < 0)
    {
#if HAL_FAN_EN > 0
        halFanOn();
#endif /* HAL_FAN_EN > 0 */
        sprayPowerCtrlTid = appTaskSchedCreate(0, appSprayOnPeriod);
    }
    
    if(stopWaterCheckTid >= 0)
    {
        appTaskSchedDelete(stopWaterCheckTid);
        stopWaterCheckTid = -1;
    }
}
static void    appSprayOff  (void)
{
    if(sprayPowerCtrlTid >= 0)
    {
#if HAL_FAN_EN > 0
        halFanOff();
#endif /* HAL_FAN_EN > 0 */
        halSprayOff();
        appTaskSchedDelete(sprayPowerCtrlTid);
        sprayPowerCtrlTid = -1;
    }
    
    if(stopWaterCheckTid >= 0)
    {
        appTaskSchedDelete(stopWaterCheckTid);
        stopWaterCheckTid = -1;
    }
}

/* Public functions ----------------------------------------------------------*/

/**
  * @brief Spray initialization routine.
  * @param  None
  * @retval None
  */
extern void    appSprayInit (void)
{
    halSprayInit();
//  halWaterChkInit();
#if HAL_FAN_EN > 0
    halFanInit();
#endif /* HAL_FAN_EN > 0 */

#if APP_FREQHOP_EN > 0
    appFreqHop_Init();
#endif /* APP_FREQHOP_EN > 0 */
    appSprayResetWaterChkData();
}


extern void appSprayResetWaterChkData(void)
{
    uint8_t i;
    
    resultCnt = 0;
    resultIIR = 0;
    baseline = 0;
    resultBufPtr = 0;
    dbgRecMinDeltaIndex = UINT16_MAX;
    dbgRecMaxDeltaIndex = 0;
    for(i = 0; i < APP_SPRAY_WC_RESULT_BUF_SIZE; i++)
    {
        resultBuf[i] = 0;
    }
}

extern void appSpraySet(const APP_SPRAY_PROFILE_t *profile)
{
    if(intermissionModeTid >= 0)
    {
        appTaskSchedDelete(intermissionModeTid);
        intermissionModeTid = -1;
    }
    
    if(minutesLeftTid >= 0)
    {
        appTaskSchedDelete(minutesLeftTid);
        minutesLeftTid = -1;
    }
    
    appSprayOff();
    
    minutesLeft = profile->totalTimeInMin;
    onPeriodInSec = profile->onPeriodInSec;
    offPeriodInSec = profile->offPeriodInSec;
    
    if(onPeriodInSec > 0)
    {
        if(offPeriodInSec > 0)
        {
            //intermission mode
            intermissionModeTid = appTaskSchedCreate(0, appSprayTimerTask_onPeriod);
        }
        else
        {
            //continuous mode
            appSprayOn();
        }
        
        if(minutesLeft > 0)
        {
            //has total working time limitation
            minutesLeftTid = appTaskSchedCreate(60000, appSprayTimerTask_minutesCountDown);
        }
    }
    

}

extern void    appSpraySetPower(uint8_t on_time_ms, uint8_t off_time_ms)
{
    sprayOnTimeInMs = on_time_ms;
    sprayOffTimeInMs = off_time_ms;
}
#endif /* APP_SPRAY_EN > 0 */
/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/

