/**
  ******************************************************************************
  * @file    app_freqhop.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    13-OCT-2017
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
#include "..\hal\hal_freqhop.h"
#include "..\hal\hal_flash.h"
#include "..\app\app_freqhop.h"
#include "..\app\app_water_chk.h"
#include "..\app\app_spray.h"
#include "..\bsp\interrupt.h"
#include "..\app\app_tasksched.h"
#include "..\app\app_events.h"

#include <stdio.h>

/** @addtogroup Template_Project
  * @{
  */

#if APP_FREQHOP_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN (uint16_t)5     //minimum average value training times of rought tune.
#define APP_SPRAY_FHOP_FINE_TUNE_TRAIN  (uint16_t)15    //minimum average value training times of fine tune.
#define APP_SPRAY_FHOP_TARGET_VOLTAGE   (uint16_t)2457  //target working voltage
#define APP_SPRAY_FHOP_CFM_CNT          (uint8_t)3
#define APP_SPRAY_FHOP_AVG_DLY_CFM_CNT  (uint16_t)100   //if it can working stably for 1500 groups of pulses (about 30 sec), confirm it is stable and confirm the average value recored @No.200 pulses. 
#define APP_SPRAY_FHOP_FREQ_LVL_DEF     (uint8_t)24     //default frequency
#define APP_SPRAY_FHOP_FREQ_LVL_MIN     (uint8_t)8      //lowest frequency to try
#define APP_SPRAY_FHOP_FREQ_LVL_MAX     (uint8_t)55     //highest frequency to try
#define APP_SPRAY_FHOP_FREQ_LVL_CNT     (uint8_t)(APP_SPRAY_FHOP_FREQ_LVL_MAX-APP_SPRAY_FHOP_FREQ_LVL_MIN+1)
#define APP_SPRAY_FHOP_TIMEOUT          (uint32_t)10000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static xdata uint32_t fhopSampleSum[APP_SPRAY_FHOP_FREQ_LVL_CNT] = {0};
static xdata uint16_t fhopSampleCnt = 0;
static xdata uint8_t fhopSampleLock[APP_SPRAY_FHOP_FREQ_LVL_CNT] = {0};
static xdata uint8_t fhopSampleCfmCnt[APP_SPRAY_FHOP_FREQ_LVL_CNT] = {0};
static xdata uint16_t fhopSampleDlyCfmCnt = 0;
static xdata int8_t fhopStep = 0;
static xdata int8_t tidTimeout = -1;
static bool fhopTimeout = FALSE;
static bool fhopEnableFlag = FALSE;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void appFreqHop_TimerTask_Timeout(void)
{
    //fhopStep = 0;
    //fhopSampleDlyCfmCnt = APP_SPRAY_FHOP_AVG_DLY_CFM_CNT;
    tidTimeout = -1;
    fhopTimeout = TRUE;
}
static void appFreqHop_ResetData(void)
{
    uint8_t i;
    
    fhopSampleCnt = 0;
    fhopStep = -2;
    fhopSampleDlyCfmCnt = UINT16_MAX;
    fhopTimeout = FALSE;
    for(i = 0; i < APP_SPRAY_FHOP_FREQ_LVL_CNT; i++)
    {
        fhopSampleLock[i] = 0;
        fhopSampleSum[i] = 0;
        fhopSampleCfmCnt[i] = 0;
    }
    appFreqHop_Init();
    
}
/* Public functions ----------------------------------------------------------*/
extern void appFreqHop_Init(void)
{
    uint8_t freq = 0xFF;
    
    halFlashRead(&freq, sizeof(uint8_t));
    if(freq < APP_SPRAY_FHOP_FREQ_LVL_MIN || freq > APP_SPRAY_FHOP_FREQ_LVL_MAX || fhopEnableFlag == TRUE)
        freq = APP_SPRAY_FHOP_FREQ_LVL_DEF;
    halFreqHopSet(freq);
}
extern void appFreqHop_SetEnable(void)
{
    fhopEnableFlag = TRUE;
    appFreqHop_ResetData();
}
extern bool appFreqHop_IsEnabled(void)
{
    return fhopEnableFlag;
}
extern void appFreqHop_SaveFreqToFlash(void)
{
    uint8_t freqLevel;
    freqLevel = halFreqHopGet();
    halFlashErase();
    halFlashWrite(&freqLevel, sizeof(uint8_t));
}
extern void appFreqHop_Reset(void)
{
    appFreqHop_ResetData();
    appTaskSchedDelete(tidTimeout);
    tidTimeout = -1;
}
extern bool appFreqHop_Timeout(void)
{
    return fhopTimeout;
}
extern bool appFreqHop_UpdateFreq(void)
{
    uint8_t freqLevel;
    uint8_t freqLevelBak;
    uint8_t freqLevelMin;
    uint8_t i;
//    uint8_t j;
    uint8_t contLockFreqCnt = 0;
    bool newFreqFlag = FALSE;
    uint32_t xdata compareBuf[3];
#if APP_WATER_CHK_EN > 0
    APP_WATERCHK_RESULT_t *result;
#endif /* APP_WATER_CHK_EN > 0 */
    
    
    freqLevel = halFreqHopGet();
    while(freqLevel < APP_SPRAY_FHOP_FREQ_LVL_MIN);
    freqLevel -= APP_SPRAY_FHOP_FREQ_LVL_MIN;

    //if sanning frequency finished
    if(fhopStep == 0)
    {
        if(fhopSampleDlyCfmCnt != UINT16_MAX)
        {
            //fine tune finished
            if(fhopSampleDlyCfmCnt > 0)
            {
                fhopSampleDlyCfmCnt--;
                if(fhopSampleDlyCfmCnt == 0)
                {
                    fhopEnableFlag = FALSE;
                    appEventFreqHopCfm();
                }
            }
        }
        else
        {
            //fine tune process
            if(tidTimeout < 0)
                tidTimeout = appTaskSchedCreate(APP_SPRAY_FHOP_TIMEOUT, appFreqHop_TimerTask_Timeout);
            if(fhopSampleLock[freqLevel] == 0)
            {
                if(fhopSampleCnt++ < APP_SPRAY_FHOP_FINE_TUNE_TRAIN)
                {
#if APP_WATER_CHK_EN > 0
                    result = appWaterChkGetResult();
                    fhopSampleSum[freqLevel] += result->avg;
#endif /* APP_WATER_CHK_EN > 0 */
                }
                else
                {
                    fhopSampleLock[freqLevel] = 1;
                    freqLevelBak = freqLevel;

                    if(fhopSampleSum[freqLevel] > (uint32_t)APP_SPRAY_FHOP_FINE_TUNE_TRAIN*(uint32_t)APP_SPRAY_FHOP_TARGET_VOLTAGE)
                    {
                        if(freqLevel > 0)
                        {
                            freqLevel--;
                            halFreqHopSet(freqLevel+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                            newFreqFlag = TRUE;
                        }
                    }
                    else
                    {
                        if(freqLevel + APP_SPRAY_FHOP_FREQ_LVL_MIN < APP_SPRAY_FHOP_FREQ_LVL_MAX)
                        {
                            freqLevel++;
                            newFreqFlag = TRUE;
                        }
                    }

                    if(newFreqFlag == TRUE)
                    {
                        if(fhopSampleLock[freqLevel] == 1)
                        {
                            freqLevelMin = freqLevel > freqLevelBak ? freqLevelBak : freqLevel;
                            fhopSampleCfmCnt[freqLevelMin]++;
                            if(fhopSampleCfmCnt[freqLevelMin] >= APP_SPRAY_FHOP_CFM_CNT)
                            {
                                fhopSampleDlyCfmCnt = APP_SPRAY_FHOP_AVG_DLY_CFM_CNT;
                                halFreqHopSet(freqLevelMin+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                                appTaskSchedDelete(tidTimeout);
                                tidTimeout = -1;
                            }
                            else
                            {
                                halFreqHopSet(freqLevel+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                            }
                            fhopSampleSum[freqLevel] = 0;
                            fhopSampleCnt = 0;
                            fhopSampleLock[freqLevel] = 0;
                        }
                        else
                        {
                            fhopSampleCnt = 0;
                            fhopSampleSum[freqLevel] = 0;
                            halFreqHopSet(freqLevel+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                        }
                    }
                    else
                    {
                        //no new frequency is available, use current frequency
                        fhopSampleDlyCfmCnt = APP_SPRAY_FHOP_AVG_DLY_CFM_CNT;
                    }
                    
                }
            }
        }
    }
    else
    {
        if(tidTimeout < 0)
            tidTimeout = appTaskSchedCreate(APP_SPRAY_FHOP_TIMEOUT, appFreqHop_TimerTask_Timeout);
        
        if(fhopSampleLock[freqLevel] == 0)
        {
            if(fhopSampleCnt++ < APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN)
            {
#if APP_WATER_CHK_EN > 0
                result = appWaterChkGetResult();
                fhopSampleSum[freqLevel] += result->avg;
#endif /* APP_WATER_CHK_EN > 0 */
            }
            else
            {
                //Lock the current frequency
                fhopSampleLock[freqLevel] = 1;

                if(fhopStep == -2 || fhopStep == 2)
                {
                    for(i = 0; i < APP_SPRAY_FHOP_FREQ_LVL_CNT; i++)
                    {
                        if(fhopSampleLock[i] == 1)
                        {
                            compareBuf[0] = compareBuf[1];
                            compareBuf[1] = compareBuf[2];
                            compareBuf[2] = fhopSampleSum[i];
                            contLockFreqCnt++;
                            if(contLockFreqCnt >= 3)
                            {
                                if(fhopStep == -2)
                                {
                                    if(compareBuf[2] <= compareBuf[1] &&
                                       compareBuf[1] <= compareBuf[0] && 
                                       compareBuf[0] < (uint32_t)APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN*(uint32_t)4095)
                                    {
                                        //predict that lower frequencies are worse
                                        //disable_interrupt();
                                        //while(1);
                                        fhopStep = 2;
                                        break;
                                    }
                                    if(compareBuf[2] >= compareBuf[1] &&
                                       compareBuf[1] >= compareBuf[0] &&
                                       compareBuf[2] <= (uint32_t)APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN*(uint32_t)APP_SPRAY_FHOP_TARGET_VOLTAGE)
                                    {
                                        fhopStep = 2;
                                        break;
                                    }
                                    
                                }
                                else if(fhopStep == 2)
                                {
                                    if(compareBuf[2] >= compareBuf[1] &&
                                       compareBuf[1] >= compareBuf[0])
                                    {
                                        if(compareBuf[2] > (uint32_t)APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN*(uint32_t)APP_SPRAY_FHOP_TARGET_VOLTAGE)
                                        {
                                            fhopStep = -1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if(fhopStep == -2 || fhopStep == 2)
                    {
                        i = freqLevel;
                        contLockFreqCnt = 0;
                        while(((int16_t)i + (int16_t)fhopStep) >= 0 && ((int16_t)i + (int16_t)fhopStep) < APP_SPRAY_FHOP_FREQ_LVL_CNT)
                        {
                            i += fhopStep;
                            if(fhopSampleLock[i] == 0)
                            {
                                halFreqHopSet(i+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                                fhopSampleCnt = 0;
                                fhopSampleSum[i] = 0;
                                freqLevel = i;
                                newFreqFlag = TRUE;
                                break;
                            }
                            else
                            {
                                compareBuf[0] = compareBuf[1];
                                compareBuf[1] = compareBuf[2];
                                compareBuf[2] = fhopSampleSum[i];
                                contLockFreqCnt++;
                                if(contLockFreqCnt >= 3)
                                {
                                    if(compareBuf[2] >= compareBuf[1] &&
                                       compareBuf[1] >= compareBuf[0])
                                    {
                                        if(compareBuf[2] > (uint32_t)APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN*(uint32_t)APP_SPRAY_FHOP_TARGET_VOLTAGE)
                                        {
                                            fhopStep = -1;
                                        }
                                    }
                                }
                            }
                        }
                        if(newFreqFlag == FALSE)
                        {
                            appFreqHop_ResetData();
                            newFreqFlag = TRUE;
                        }
                    }
                }
            }
        }

        if(fhopStep == -1)
        {
            if(fhopSampleLock[freqLevel] == 1)
            {
                if(fhopSampleSum[freqLevel] < (uint32_t)APP_SPRAY_FHOP_ROUGH_TUNE_TRAIN*(uint32_t)APP_SPRAY_FHOP_TARGET_VOLTAGE)
                {
                    fhopStep = 0;
                    fhopSampleDlyCfmCnt = UINT16_MAX;
                    for(i = 0; i < APP_SPRAY_FHOP_FREQ_LVL_CNT; i++)
                    {
                        fhopSampleLock[i] = 0;
                        fhopSampleSum[i] = 0;
                    }
                    fhopSampleCnt = 0;
                }
                else
                {
                    if(freqLevel > 0)
                    {
                        freqLevel += fhopStep;
                        halFreqHopSet(freqLevel+APP_SPRAY_FHOP_FREQ_LVL_MIN);
                        if(fhopSampleLock[freqLevel] == 0)
                        {
                            fhopSampleCnt = 0;
                            fhopSampleSum[freqLevel] = 0;
                        }
                        newFreqFlag = TRUE;
                    }
                    else
                    {
                        appFreqHop_ResetData();
                        newFreqFlag = TRUE;
                    }
                }
            }
        }
    }
    
    return newFreqFlag;
}

extern bool appFreqHop_FineTuneFinished(void)
{
    if(fhopStep == 0 && fhopSampleDlyCfmCnt != UINT16_MAX)
    {
        return TRUE;
    }

    return FALSE;
}

extern bool appFreqHop_RoughTuneFinished(void)
{
    if(fhopStep == 0)
    {
        return TRUE;
    }

    return FALSE;
}

extern bool appFreqHop_LockFreq(void)
{
    if(fhopStep == 0 && fhopSampleDlyCfmCnt == 0)
    {
        return TRUE;
    }

    return FALSE;
}

/**
  * @}
  */
#endif /* APP_FREQHOP_EN > 0 */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/

