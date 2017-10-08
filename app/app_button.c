/**
  ******************************************************************************
  * @file    app_button.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    19-SEP-2017
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
#include <stdio.h>
#include "..\app\app_button.h"
#include "..\app\app_tasksched.h"
#include "..\app\app_events.h"
#include "..\hal\hal_button.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_BUTTON_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define BUTTON_CHECK_INTERVAL       100
#define BUTTON_DBLCFM_INTERVAL      5
/* Private variables ---------------------------------------------------------*/
//static uint8_t lightButtonCnt = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void appButtonPeriodCheck(void)
{
    static xdata uint8_t pressCnt = 0;//support 8 button if using uint8_t
    int8_t tid = -1;
    if(halButtonGetState(HAL_BUTTON_Mode) == HAL_BUTTON_STATE_Released)
    {
        //released
        pressCnt = 0;
        tid = appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck);
    }
    else
    {
        //pressed
        if(pressCnt == 0)
        {
            pressCnt++;
            //detect press button, need to confirm
            tid = appTaskSchedCreate(BUTTON_DBLCFM_INTERVAL, appButtonPeriodCheck);
        }
        else if(pressCnt == 1)
        {
            pressCnt++;
#if APP_EVENTS_EN > 0
            appEventButtonPress();
#endif /* APP_EVENTS_EN > 0 */
#if DEBUG_APP_BUTTON_EN > 0
            printf("Mode button pressed!\r\n");
#endif /* DEBUG_APP_BUTTON_EN > 0 */
            tid = appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck);
        }
        else
        {
            tid = appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck);
        }
    }

    while(tid < 0);
}
/* Public functions ----------------------------------------------------------*/

/**
  * @brief Light initialization routine. Config TIM1 frequencys, etc.
  * @param  None
  * @retval None
  */
void appButtonInit(void)
{
    halButtonInit();
    if(appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck) < 0)
    {
#if DEBUG_APP_BUTTON_EN > 0
        printf("Create button scan task failed!");
        printf("MCU hang up!\r\n");
#endif
        while(1);
    }
}


#endif /* APP_BUTTON_EN */

/**
  * @}
  */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/