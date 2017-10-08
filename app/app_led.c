/**
  ******************************************************************************
  * @file    app_led.c
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
#include "..\APP\app_tasksched.h"
#include "..\APP\app_led.h"
#include "..\hal\hal_led.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_LED_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
void appLedInit(void)
{
    halLedInit();
}
void appLedOn(LED_t led)
{
    halLedOn((HAL_LED_t)led);
}
void appLedOff(LED_t led)
{
    halLedOff((HAL_LED_t)led);
}
void appLedToggle(LED_t led)
{
    halLedToggle((HAL_LED_t)led);
}
LED_STATE_t appLedState(LED_t led)
{
    return halLedState((HAL_LED_t)led);
}

#endif /* APP_LED_EN */

/**
  * @}
  */


/************************ (C) COPYRIGHT OKMCU ltd. *****END OF FILE****/