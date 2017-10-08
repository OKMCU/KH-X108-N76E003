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
#include <stdio.h>
#include "..\app\app_tasksched.h"
#include "..\app\app_spray.h"
#include "..\app\app_led.h"
#include "..\app\app_events.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_EVENTS_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SECOND                 (uint32_t)1000
/* Private variables ---------------------------------------------------------*/
static uint8_t buttonCnt = 0;
static int8_t mistModeTid = -1;
static int8_t operTimeTid = -1;
static uint8_t operTimeMin = 0;
/* Private function prototypes -----------------------------------------------*/
static void appMistMode_intermittenly_on(void);
static void appMistMode_intermittenly_off(void);
static void appEvent_OperTimeCount(void);

/* Private functions ---------------------------------------------------------*/
static void appMistMode_intermittenly_on(void)
{
    appSprayOn();
    mistModeTid = appTaskSchedCreate(10*SECOND, appMistMode_intermittenly_off);
}
static void appMistMode_intermittenly_off(void)
{
    appSprayOff();
    mistModeTid = appTaskSchedCreate(20*SECOND, appMistMode_intermittenly_on);
}
static void appEvent_OperTimeCount(void)
{
    operTimeMin++;
#if DEBUG_APP_EVENTS_EN > 0
    printf("operTimeMin++\r\n");
#endif /* APP_EVENTS_EN > 0 */
    if(buttonCnt == 1)
    { 
        if(operTimeMin >= 120)
        {
            operTimeTid = -1;
        }
        else
        {
            operTimeTid = appTaskSchedCreate(60*SECOND, appEvent_OperTimeCount);
        }
    }
    else if(buttonCnt == 2)
    {
        if(operTimeMin >= 240)
        {
            operTimeTid = -1;
#if DEBUG_APP_EVENTS_EN > 0
    printf("4 hour timeout!\r\n");
#endif /* APP_EVENTS_EN > 0 */
        }
        else
        {
            operTimeTid = appTaskSchedCreate(60*SECOND, appEvent_OperTimeCount);
        } 
    }
    else
    {
        operTimeTid = -1;
        operTimeMin = 0;
    }

    if(operTimeTid == -1)
    {
        appEventNoWater();
    }
    
}

/* Public functions ----------------------------------------------------------*/

void appEventButtonPress(void)
{
//    if(blinkTid >= 0)
//        return;

#if DEBUG_APP_EVENTS_EN > 0
    printf("Button Pressed!\r\n");
#endif /* APP_EVENTS_EN > 0 */

#if APP_LED_EN > 0
    appLedOff(LED_B);
    appLedOff(LED_G);
#endif /* APP_LED_EN > 0 */
    operTimeMin = 0;

    if(operTimeTid >= 0)
    {
        appTaskSchedDelete(operTimeTid);
        operTimeTid = -1;
    }
    if(mistModeTid >= 0)
    {
        appTaskSchedDelete(mistModeTid);
        mistModeTid = -1;
    }
    
    buttonCnt++;
    if(buttonCnt == 1)
    {
 #if APP_LED_EN > 0
        appLedOn(LED_G);
 #endif /* APP_LED_EN > 0 */
        appSprayOn();
        operTimeTid = appTaskSchedCreate(60*SECOND, appEvent_OperTimeCount);
    }
    else if(buttonCnt == 2)
    {
#if APP_LED_EN > 0
        appLedOn(LED_B);
#endif /* APP_LED_EN > 0 */
        appMistMode_intermittenly_on();
        operTimeTid = appTaskSchedCreate(60*SECOND, appEvent_OperTimeCount);
    }
    else
    {
        buttonCnt = 0;
#if APP_LED_EN > 0
        appLedOff(LED_B);
        appLedOff(LED_G);
#endif /* APP_LED_EN > 0 */
        appSprayOff();
    }
}
void appEventNoWater(void)
{
#if DEBUG_APP_EVENTS_EN > 0
    printf("No water!\r\n");
#endif /* APP_EVENTS_EN > 0 */
    buttonCnt = 0;
    operTimeMin = 0;
    
    if(mistModeTid >= 0)
    {
        appTaskSchedDelete(mistModeTid);
        mistModeTid = -1;
    }

    if(operTimeTid >= 0)
    {
        appTaskSchedDelete(operTimeTid);
        operTimeTid = -1;
    }
    
#if APP_LED_EN > 0
    appLedOff(LED_B);
    appLedOff(LED_G);
#endif /* APP_LED_EN > 0 */
    appSprayOff();
//    appTaskSchedCreate(500, appEvent_InCaseErrorBeepFailed);
//    appEvent_IndicateError(3, 600);
}

#endif /* APP_EVENTS_EN > 0 */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/