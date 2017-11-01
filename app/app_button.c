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
#define BUTTON_LONG_PRESS_TIME      2000
#define BUTTON_VLONG_PRESS_TIME     3000
/* Private variables ---------------------------------------------------------*/
static xdata int8_t tidMist = -1;
static xdata int8_t tidPeriodCheck = -1;
static xdata uint8_t preTouchState = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void appButtonMistLongPress(void)
{
    tidMist = -1;
    appEventMistButtonLongPress();
}

static void appButtonPeriodCheck(void)
{
    uint8_t curTouchState;
    tidPeriodCheck = appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck);

    curTouchState = appButtonGetCurrentState();
    if(curTouchState != preTouchState)
    {
        switch(curTouchState)
        {

            case APP_BUTTON_STATE_MIST:
                if(preTouchState == 0)
                {
                    appEventMistButtonTouchEnter();
                    if(tidMist < 0)
                        tidMist = appTaskSchedCreate(BUTTON_LONG_PRESS_TIME, appButtonMistLongPress);
                }

            break;


            default:

                if(tidMist >= 0)
                {
                    appEventMistButtonPress();
                    appTaskSchedDelete(tidMist);
                    tidMist = -1;
                }

            break;
        }
    }
    preTouchState = curTouchState;
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
    tidPeriodCheck = appTaskSchedCreate(BUTTON_CHECK_INTERVAL, appButtonPeriodCheck);
    if(tidPeriodCheck < 0)
    {
#if DEBUG_APP_BUTTON_EN > 0
        printf("Create button scan task failed!");
        printf("MCU hang up!\r\n");
#endif
        while(1);
    }
}

uint8_t appButtonGetCurrentState(void)
{
    uint8_t state = 0;


    if(halButtonGetState(HAL_BUTTON_Mist) == HAL_BUTTON_STATE_Pressed)
    {
        state |= (1<<HAL_BUTTON_Mist);
    }

    return state;
}
#endif /* APP_BUTTON_EN */

/**
  * @}
  */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/