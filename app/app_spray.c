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
#define APP_SPRAY_OFF_TIME              (uint32_t)16    //in ms. 1.7MHz off time in one period. Larger off time, lower power consumption.
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
#define APP_SPRAY_WC_TH_RESULT_CNT      (uint8_t)200    //check more abnormal data after 150 pulses, the smaller the better but should not lead to false detection.
#define APP_SPRAY_WC_TH_DELTAINDEX_HI   (uint16_t)1500  //no water, if (max -min) > 1500, the lower the better but should not lead to false detection.
#define APP_SPRAY_WC_TH_DELTAINDEX_LO   (uint16_t)400   //no water, if (max -min) < 510, the larger the better but should not lead to false detection.
#define APP_SPRAY_WC_TH_DELTAINDEX_NORMAL_MAX (uint16_t)700 //no water, if dbgRecMaxDeltaIndex < 800
/* Private variables ---------------------------------------------------------*/
//static bool sprayOnFlag = FALSE;
static xdata int8_t  sprayPowerCtrlTid = -1;
static xdata int8_t  stopWaterCheckTid = -1;
//static xdata uint16_t resultRawMax = 0;
//static xdata uint16_t resultRawMin = 0;
static xdata uint32_t resultCnt = 0;
static xdata uint16_t resultIIR = 0;
static xdata uint16_t baseline = 0;
//static xdata uint16_t resultIIR = 0;
//static xdata uint16_t resultIIRMax = 0;
//static xdata uint16_t resultIIRMin = 0;
static xdata uint16_t resultBuf[APP_SPRAY_WC_RESULT_BUF_SIZE] = {0};
static xdata uint8_t resultBufPtr = 0;
static xdata uint16_t dbgRecMinDeltaIndex = UINT16_MAX;
static xdata uint16_t dbgRecMaxDeltaIndex = 0;
//static xdata uint32_t dbgRecMinShakeIndex = UINT32_MAX;
//static xdata uint32_t dbgRecMaxShakeIndex = 0;
/* Private function prototypes -----------------------------------------------*/
static void appSprayTimerTask_StopCheckWater(void);
static void appSprayOnPeriod(void);
static void appSprayOffPeriod(void);
static bool appSprayIsWaterExist(void);
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
    sprayPowerCtrlTid = appTaskSchedCreate(APP_SPRAY_ON_TIME, appSprayOffPeriod);
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
    sprayPowerCtrlTid = appTaskSchedCreate(APP_SPRAY_OFF_TIME, appSprayOnPeriod);
}

#if APP_SPRAY_WC_ALGO_EN > 0
static bool appSprayIsWaterExist(void)
{
    uint8_t i;
    uint16_t max = 0;
    uint16_t min = UINT16_MAX;
    uint8_t abnormalCnt = 0;
    int16_t delta = 0;
    uint8_t failId = 0;
//    uint32_t flatIndex = 0;
    uint16_t deltaIndex;
    uint16_t effectiveCnt = 0;
//    uint16_t average;
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

                        //if(resultCnt > APP_SPRAY_WC_TH_RESULT_CNT)
                        //{
                            //working long enough, do more check here
                            //if(dbgRecMaxDeltaIndex < APP_SPRAY_WC_TH_DELTAINDEX_NORMAL_MAX)
                            //{
                                //failId |= (1<<4);//0x10
                            //}
                        //}
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
    //else
    //{
    //    if(resultCnt >= 2)
    //    {
    //        if(deltaIndex > APP_SPRAY_WC_TH_DELTAINDEX_HI)
    //        {
                //failId |= (1<<5);//0x20
    //        }
    //    }
    //}
    
    if(appFreqHop_Timeout() == TRUE)
    {
        failId |= (1<<6);//0x40
    }

    if(failId != 0)
    {
        return FALSE;;
    }
    
    return TRUE;
}
#else /* APP_SPRAY_ALGO_CheckIIR_EN > 0 */
static bool appSprayIsWaterExist(void)
{

    return TRUE;
}
#endif /* APP_SPRAY_ALGO_CheckIIR_EN > 0 */



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
    
//    sprayPowerCtrlTid = appTaskSchedCreate(APP_SPRAY_OFF_TIME, appSprayOnPeriod);
}
extern void    appSprayOn   (void)
{
    if(sprayPowerCtrlTid < 0)
    {
#if HAL_FAN_EN > 0
        halFanOn();
#endif /* HAL_FAN_EN > 0 */
        sprayPowerCtrlTid = appTaskSchedCreate(0, appSprayOnPeriod);
    }
}
extern void    appSprayOff  (void)
{
    if(sprayPowerCtrlTid >= 0)
    {
#if HAL_FAN_EN > 0
        halFanOff();
#endif /* HAL_FAN_EN > 0 */
        //halPwmOff(HAL_PWM_ONOFF_SPRAY);
        //halPwmSetFrequency(HAL_PWM_FREQ_LIGHT);
        //halPwmOn(HAL_PWM_ONOFF_LIGHT);
        halSprayOff();
        appTaskSchedDelete(sprayPowerCtrlTid);
        sprayPowerCtrlTid = -1;
    }
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

//extern void    appSprayGetResult(uint16_t *iir, uint16_t *iirMax, uint16_t *iirMin)
//{
//    if(iir != NULL)
//        *iir = resultIIR;
//    if(iirMax != NULL)
//        *iirMax = resultIIRMax;
//    if(iirMin != NULL)
//        *iirMin = resultIIRMin;
//}

/**
  * @}
  */
#endif /* APP_SPRAY_EN */

//#if APP_SPRAY_ALGO_PeakValueCnt_EN > 0
//static bool appSprayIsWaterExist(void)
//{
//    
//    uint8_t i;
//    uint16_t resultOverLimitCnt = 0;
//    
//    if(sprayPowerCtrlTid >= 0 && resultCnt >= APP_SPRAY_WC_BUF_SIZE)
//    {
//        for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//        {
//            if(resultBuf[i] < APP_SPRAY_WC_THL || resultBuf[i] > APP_SPRAY_WC_THH)
//            {
//                resultOverLimitCnt++;
//            }
//        }
//
//        if(resultOverLimitCnt > APP_SPRAY_WC_CFM_CNT)
//        {
//#if DEBUG_APP_SPRAY_EN > 0
//        printf("resultBuf[] = ");
//        for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//        {
//            printf("%d", resultBuf[i]);
//            printf(" ");
//        }
//        printf("\r\n");
//
//        printf("resultOverLimitCnt = ");
//        printf("%d", resultOverLimitCnt);
//        printf("\r\n");
//#endif
//            return FALSE;
//        }
//    }
//    return TRUE;
//}
//#endif /* APP_SPRAY_ALGO_PeakValueCnt_EN */


//#if APP_SPRAY_ALGO_MiddleValue_EN > 0
//static bool appSprayIsWaterExist(void)
//{
//    
//    uint8_t i;
//    uint8_t j;
//    uint16_t buf[APP_SPRAY_WC_BUF_SIZE];
//    uint16_t tmp;
//    
//    if(sprayPowerCtrlTid >= 0 && resultCnt >= APP_SPRAY_WC_BUF_SIZE)
//    {
//        for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//        {
//            buf[i] = resultBuf[i];
//        }
//
//        
//        for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//        {
//            for(j = i+1; j < APP_SPRAY_WC_BUF_SIZE; j++)
//            {
//                if(buf[i] > buf[j])
//                {
//                    tmp = buf[i];
//                    buf[i] = buf[j] ;
//                    buf[j]= tmp;
//                }
//            }
//        }
//
//        if(buf[APP_SPRAY_WC_BUF_SIZE/2] < APP_SPRAY_WC_THL || buf[APP_SPRAY_WC_BUF_SIZE/2] > APP_SPRAY_WC_THH)
//        {
//#if DEBUG_APP_SPRAY_EN > 0
//            printf("buf[] = ");
//            for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//            {
//                printf("%d", buf[i]);
//                printf(" ");
//            }
//            printf("\r\n");
//
//            printf("Middle Value = ");
//            printf("%d", buf[APP_SPRAY_WC_BUF_SIZE/2]);
//            printf("\r\n");
//#endif
//            return FALSE;
//            //return TRUE;
//        }
//    }
//    return TRUE;
//}
//#endif /* APP_SPRAY_ALGO_MiddleValue_EN > 0 */

//#if APP_SPRAY_ALGO_TestMode_EN > 0
//static bool appSprayIsWaterExist(void)
//{
//    uint8_t i;
//    if(sprayPowerCtrlTid >= 0 && resultCnt >= APP_SPRAY_WC_BUF_SIZE)
//    {
//#if DEBUG_APP_SPRAY_EN > 0
//        printf("resultBuf[] = ");
//        for(i = 0; i < APP_SPRAY_WC_BUF_SIZE; i++)
//        {
//            printf("%d", resultBuf[i]);
//            printf(" ");
//        }
//        printf("\r\n");
//#endif
//        return FALSE;
//    }
//    return TRUE;
//}
//#endif /* APP_SPRAY_ALGO_TestMode_EN > 0 */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/

