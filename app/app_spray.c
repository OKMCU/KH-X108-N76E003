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

#include <stdio.h>

/** @addtogroup Template_Project
  * @{
  */

#if APP_SPRAY_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_SPRAY_ALGO_CheckIIR_EN      1
/* Private macro -------------------------------------------------------------*/
#define APP_SPRAY_ON_TIME               (uint32_t)5     //in ms. 1.7MHz on time in one period. Larger on time, larger power consumption
#define APP_SPRAY_OFF_TIME              (uint32_t)16    //in ms. 1.7MHz off time in one period. Larger off time, lower power consumption.
#define APP_SPRAY_WC_DELAY              (uint32_t)3     //in ms. Phase delay time. Should confirm this phase delay using dual channel oscilloscope.
#define APP_SPRAY_WC_THL                (uint16_t)1638  //water exist, higher than 1.0V.
#define APP_SPRAY_WC_THH                (uint16_t)3686  //water exist, lower than 4.5V.
#define APP_SPRAY_WC_IIR_MIN_TRAIN      (uint8_t)10     //minimum IIR filter training times.
#define APP_SPRAY_WC_DELTA_BUF_SIZE     (uint8_t)15     //15 pulses last for about 315 ms, should be enough to detect the no water tremble.
#define APP_SPRAY_WC_DELTA_SUM_TH       (uint16_t)2500  //tremble delta sum threshold.
#define APP_SPRAY_FHOP_AVG_MIN_TRAIN    (uint16_t)50    //minimum average value training times of a working frequency.
#define APP_SPRAY_FHOP_AVG_DLY_CFM_CNT  (uint16_t)50    //if it can working stably for 50 groups of pulses, confirm it is stable and confirm the average value recored @No.200 pulses. 
#define APP_SPRAY_FHOP_FREQ_LVL_MIN     (uint8_t)1      //lowest frequency to try
#define APP_SPRAY_FHOP_FREQ_LVL_MAX     (uint8_t)9      //highest frequency to try
/* Private variables ---------------------------------------------------------*/
static xdata int8_t  sprayPowerCtrlTid = -1;
static xdata int8_t  waterCheckTid = -1;
static xdata uint8_t resultCnt = 0;
static xdata uint16_t resultIIR = 0;
static xdata uint16_t resultIIRMax = 0;
static xdata uint16_t resultIIRMin = 0;
//static xdata uint16_t resultPre = 0;
//static xdata uint16_t resultCur = 0;
static xdata uint16_t resultDelta[APP_SPRAY_WC_DELTA_BUF_SIZE] = {0};
static xdata uint8_t resultDeltaPtr = 0;
static xdata uint32_t fhopSampleSum[HAL_FREQHOP_LVL_MAX+1] = {0};
static xdata uint16_t fhopSampleCnt = 0;
static xdata uint8_t fhopSampleLock[HAL_FREQHOP_LVL_MAX+1] = {0};
static xdata uint8_t fhopSampleLockCnt = 0;
static xdata uint8_t fhopSampleDlyCfmCnt = APP_SPRAY_FHOP_AVG_DLY_CFM_CNT;
/* Private function prototypes -----------------------------------------------*/
static void appSprayTimerTask_TriggerCheckWater(void);
static void appSprayOnPeriod(void);
static void appSprayOffPeriod(void);
static void appSprayResetWaterChkData(void);
static bool appSprayIsWaterExist(void);
/* Private functions ---------------------------------------------------------*/
static void appSprayTimerTask_TriggerCheckWater(void)
{
    waterCheckTid = -1;
#if APP_WATER_CHK_EN > 0
    appWaterChkStart();
#endif /* APP_WATER_CHK_EN > 0 */
}

static void appSprayOnPeriod(void)
{
    halSprayOn();

    //halWaterChkStart();
    //peak = 0;
    sprayPowerCtrlTid = appTaskSchedCreate(APP_SPRAY_ON_TIME, appSprayOffPeriod);
    if(waterCheckTid >= 0)
    {
        appTaskSchedDelete(waterCheckTid);
        waterCheckTid = -1;
    }
    waterCheckTid = appTaskSchedCreate(APP_SPRAY_WC_DELAY, appSprayTimerTask_TriggerCheckWater);
}

static void appSprayOffPeriod(void)
{
#if APP_WATER_CHK_EN > 0
    APP_WATERCHK_RESULT_t *result;
#endif /* APP_WATER_CHK_EN > 0 */
    uint8_t freqLevel;
    uint8_t i;
    uint32_t min = UINT32_MAX;
    uint8_t minId;
//    uint8_t fhopLockCnt = 0;
    
    halSprayOff();
    if(waterCheckTid >= 0)
    {
        appTaskSchedDelete(waterCheckTid);
        waterCheckTid = -1;
    }
    
#if APP_WATER_CHK_EN > 0
    appWaterChkStop();
#endif /* APP_WATER_CHK_EN > 0 */
    sprayPowerCtrlTid = appTaskSchedCreate(APP_SPRAY_OFF_TIME, appSprayOnPeriod);
#if APP_WATER_CHK_EN > 0
    result = appWaterChkGetResult();
    
//    resultPre = resultCur;
//    resultCur = result->avg;
#endif /* APP_WATER_CHK_EN > 0 */
    
    if(resultCnt < APP_SPRAY_WC_IIR_MIN_TRAIN)
    {
        resultCnt++;
        resultIIR = (resultIIR == 0) ? result->avg : (uint16_t)(((uint32_t)resultIIR*7 + result->avg)>>3);
    }
    else
    {
          resultIIR = (resultIIR == 0) ? result->avg : (uint16_t)(((uint32_t)resultIIR*63 + result->avg)>>6);
          resultIIRMax = resultIIR > resultIIRMax ? resultIIR : resultIIRMax;
          resultIIRMin = resultIIR < resultIIRMin ? resultIIR : resultIIRMin;
          resultDelta[resultDeltaPtr++] = resultIIR > result->avg ? (resultIIR-result->avg) : (result->avg - resultIIR);
          if(resultDeltaPtr >= APP_SPRAY_WC_DELTA_BUF_SIZE)
          {
              resultDeltaPtr = 0;
          }
    }
    
    freqLevel = halFreqHopGet();
    if(appSprayIsWaterExist() == FALSE)
    {
#if DEBUG_APP_SPRAY_EN > 0
        printf("Trigger no water event!\r\n");
#endif /* DEBUG_APP_SPRAY_EN > 0 */

#if APP_EVENTS_EN > 0
        appEventNoWater();
#endif /* APP_EVENTS_EN > 0 */

        if(fhopSampleLockCnt != APP_SPRAY_FHOP_FREQ_LVL_MAX - APP_SPRAY_FHOP_FREQ_LVL_MIN + 1 || fhopSampleDlyCfmCnt > 0)
        {
            fhopSampleCnt = 0;
            fhopSampleLockCnt = 0;
            for(i = APP_SPRAY_FHOP_FREQ_LVL_MIN; i <= APP_SPRAY_FHOP_FREQ_LVL_MAX; i++)
            {
                fhopSampleLock[i] = 0;
                fhopSampleSum[i] = 0;
            }
        }
        

        appSprayResetWaterChkData();
    }
    else
    {
        if(fhopSampleLockCnt == APP_SPRAY_FHOP_FREQ_LVL_MAX - APP_SPRAY_FHOP_FREQ_LVL_MIN + 1)
        {
            if(fhopSampleDlyCfmCnt > 0)
            {
                fhopSampleDlyCfmCnt--;
            }
        }
        
        if(fhopSampleLock[freqLevel] == 0)
        {
            fhopSampleCnt++;
            if(fhopSampleCnt < APP_SPRAY_FHOP_AVG_MIN_TRAIN)
            {
                fhopSampleSum[freqLevel] += result->avg;
            }
            else
            {
                fhopSampleLock[freqLevel] = 1;
                fhopSampleLockCnt++;
                for(i = APP_SPRAY_FHOP_FREQ_LVL_MIN; i <= APP_SPRAY_FHOP_FREQ_LVL_MAX; i++)
                {
                    if(fhopSampleLock[i] == 0)
                    {
                        halFreqHopSet(i);
                        fhopSampleCnt = 0;
                        fhopSampleSum[i] = 0;
                        appSprayResetWaterChkData();
                        break;
                    }
                    else
                    {
                        if(fhopSampleSum[i] < min)
                        {
                            min = fhopSampleSum[i];
                            minId = i;
                        }
                    }
                }
                
                if(fhopSampleLockCnt == APP_SPRAY_FHOP_FREQ_LVL_MAX - APP_SPRAY_FHOP_FREQ_LVL_MIN + 1)
                {
                    //all frequency is locked
                    if(minId != freqLevel)
                    {
                        halFreqHopSet(minId);
                        appSprayResetWaterChkData();
                        fhopSampleDlyCfmCnt = APP_SPRAY_FHOP_AVG_DLY_CFM_CNT;
                    }
                }
            }
        }
    }
}

static void appSprayResetWaterChkData(void)
{
    uint8_t i;
    
    resultCnt = 0;
        
    resultIIRMin = UINT16_MAX;
    resultIIRMax = 0;
    resultIIR = 0;
//    resultPre = 0;
//    resultCur = 0;
    resultDeltaPtr = 0;
    for(i = 0; i < APP_SPRAY_WC_DELTA_BUF_SIZE; i++)
    {
        resultDelta[i] = 0;
    }
}

#if APP_SPRAY_ALGO_CheckIIR_EN > 0
static bool appSprayIsWaterExist(void)
{
    uint8_t i;
    uint16_t deltaSum = 0;
    
    if(sprayPowerCtrlTid >= 0 && resultCnt >= APP_SPRAY_WC_IIR_MIN_TRAIN)
    {
        if(fhopSampleLockCnt == APP_SPRAY_FHOP_FREQ_LVL_MAX - APP_SPRAY_FHOP_FREQ_LVL_MIN + 1)
        {
            if(resultIIR > APP_SPRAY_WC_THH)
            {
#if DEBUG_APP_SPRAY_EN > 0
                printf("resultIIR = ");
                printf("%d", resultIIR);
                printf("\r\n");
#endif
                return FALSE;
            }
        }
        
        if(resultIIR < APP_SPRAY_WC_THL)
        {
#if DEBUG_APP_SPRAY_EN > 0
            printf("resultIIR = ");
            printf("%d", resultIIR);
            printf("\r\n");
#endif
            return FALSE;
        }

        for(i = 0; i < APP_SPRAY_WC_DELTA_BUF_SIZE; i++)
        {
            deltaSum += resultDelta[i];
        }
        
        if(deltaSum > APP_SPRAY_WC_DELTA_SUM_TH)
        {
            return FALSE;
        }
    }
    return TRUE;
}
#endif /* APP_SPRAY_ALGO_CheckIIR_EN */



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
    appSprayResetWaterChkData();
}
extern void    appSprayOn   (void)
{
    //uint8_t i;
    
    if(sprayPowerCtrlTid < 0)
    {
#if HAL_FAN_EN > 0
        halFanOn();
#endif /* HAL_FAN_EN > 0 */
        appSprayOnPeriod();
        
        //appSprayResetWaterChkData();
        
    }
}
extern void    appSprayOff  (void)
{
    //uint8_t i;
    
    if(sprayPowerCtrlTid >= 0)
    {
#if HAL_FAN_EN > 0
        halFanOff();
#endif /* HAL_FAN_EN > 0 */
        halSprayOff();
        appTaskSchedDelete(sprayPowerCtrlTid);
        sprayPowerCtrlTid = -1;
        //appSprayResetWaterChkData();
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

