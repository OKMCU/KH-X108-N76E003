/**
  ******************************************************************************
  * @file    app_events.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    9-AUG-2017
  * @brief   This file contains the implementations of tri-color LED control.
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
#include "..\app\app_tasksched.h"
#include "..\app\app_light.h"
#include "..\app\app_spray.h"
#include "..\app\app_buzzer.h"
#include "..\app\app_led.h"
#include "..\app\app_events.h"
#include "..\app\app_button.h"
#include "..\app\app_freqhop.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_EVENTS_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static xdata uint8_t mistButtonCnt = 0;
static xdata uint16_t blinkCnt = 0;
static xdata uint16_t blinkPeriod = 0;
static xdata int8_t blinkTid = -1;
static xdata uint8_t porButtonState = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void appEvent_InCaseErrorBeepFailed(void)
{
    appBuzzerBeep(APP_BUZZ_ERROR);
}

static void blinkTask(void)
{
    if(appLedState(LED_MIST) == LED_STATE_ON)
    {
        appLedOff(LED_MIST);
        appLightSetMode(0);
    }
    else
    {
        appLedOn(LED_MIST);
        appLightSetMode(5);
    }
    blinkCnt--;
    if(blinkCnt)
        blinkTid = appTaskSchedCreate(blinkPeriod, blinkTask);
    else
        blinkTid = -1;
}
static void appEvent_IndicateError(uint8_t cnt, uint16_t period)
{
    if(blinkTid < 0 && cnt > 0 && period > 0)
    {
        appLedOff(LED_MIST);
        blinkCnt = cnt<<1;
        blinkPeriod = period>>1;
        blinkTid = appTaskSchedCreate(blinkPeriod, blinkTask);
    }
}
/* Public functions ----------------------------------------------------------*/

void appEventPowerOnReset(void)
{
    porButtonState = appButtonGetCurrentState();
    if(porButtonState == APP_BUTTON_STATE_MIST)
    {
    }
    else
    {
        appBuzzerBeep(APP_BUZZ_ERROR);
    }
}
void appEventMistButtonPress(void)
{
    if(blinkTid >= 0)
        return;
    
    mistButtonCnt++;
    if(mistButtonCnt == 1)
    {
        appLedOn(LED_MIST);
        appLightSetMode(0);
        appSprayOn();
    }
    else if(mistButtonCnt == 2)
    {
        appLedOn(LED_MIST);
        appLightSetMode(1);
    }
    else
    {
        mistButtonCnt = 0;
        appLedOff(LED_MIST);
        appLightSetMode(0);
        appSprayOff();
    }
}
void appEventMistButtonTouchEnter(void)
{
    if(blinkTid >= 0)
        return;
    appBuzzerBeep(APP_BUZZ_BUTTON);
}
void appEventMistButtonLongPress(void)
{
    if(blinkTid >= 0)
        return;
    if(porButtonState == APP_BUTTON_STATE_MIST)
    {
        porButtonState = 0;
        appBuzzerBeep(APP_BUZZ_FACTORY);
        appFreqHop_SetEnable();
        appSprayResetWaterChkData();
    }
}
void appEventLightMistButtonVLongPress(void)
{
//    if(porButtonState == APP_BUTTON_STATE_LIGHT + APP_BUTTON_STATE_MIST)
//    {
//        appBuzzerBeep(APP_BUZZ_FACTORY);
//        appFreqHop_SetEnable();
//        appSprayResetWaterChkData();
//    }
}
void appEventFreqHopCfm(void)
{
    appFreqHop_SaveFreqToFlash();
    mistButtonCnt = 0;
    appLightSetMode(8);//white
    appSprayOff();
    appBuzzerBeep(APP_BUZZ_FACTORY);
    appLedOn(LED_MIST);
}

void appEventNoWater(void)
{
    mistButtonCnt = 0;
    appLightSetMode(0);
    appSprayOff();
    appTaskSchedCreate(500, appEvent_InCaseErrorBeepFailed);
    appEvent_IndicateError(3, 600);
}

#endif /* APP_EVENTS_EN > 0 */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/