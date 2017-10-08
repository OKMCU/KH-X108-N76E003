/**
  ******************************************************************************
  * @file    app_buzzer.c
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
#include "..\APP\app_tasksched.h"
#include "..\APP\app_buzzer.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_BUZZER_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SINGLE_BEEP_LAST      100
#define DOUBLE_BEEP_INTERVAL  100
#define ERROR_BEEP_INTERVAL   300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static xdata int8_t beepTid = -1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void BEEP_Cmd(bool enable)
{
    enable = enable;
}
static void appBuzzerSoundButton(void)
{
    static xdata uint8_t cnt = 0;

    if(cnt == 0)
    {
        //GPIOD->ODR |= (uint8_t)GPIO_PIN_4;
        BEEP_Cmd(TRUE);
        beepTid = appTaskSchedCreate(SINGLE_BEEP_LAST, appBuzzerSoundButton);
        cnt++;
    }
    else if(cnt == 1)
    {
        //GPIOD->ODR &= (uint8_t)(~GPIO_PIN_4);
        BEEP_Cmd(FALSE);
        beepTid = -1;
        cnt = 0;
    }
}

static void appBuzzerSoundError(void)
{
    static xdata uint8_t cnt = 0;

    if(cnt == 0 || cnt == 2 || cnt == 4 || cnt== 6)
    {
        //GPIOD->ODR |= (uint8_t)GPIO_PIN_4;
        BEEP_Cmd(TRUE);
        beepTid = appTaskSchedCreate(SINGLE_BEEP_LAST, appBuzzerSoundError);
        cnt++;
    }
    else if(cnt == 1 || cnt== 5)
    {
        //GPIOD->ODR &= (uint8_t)(~GPIO_PIN_4);
        BEEP_Cmd(FALSE);
        beepTid = appTaskSchedCreate(DOUBLE_BEEP_INTERVAL, appBuzzerSoundError);
        cnt++;
    }
    else if(cnt == 3)
    {
        //GPIOD->ODR &= (uint8_t)(~GPIO_PIN_4);
        BEEP_Cmd(FALSE);
        beepTid = appTaskSchedCreate(ERROR_BEEP_INTERVAL, appBuzzerSoundError);
        cnt++;
    }
    else if(cnt == 7)
    {
        //GPIOD->ODR &= (uint8_t)(~GPIO_PIN_4);
        BEEP_Cmd(FALSE);
        beepTid = -1;
        cnt = 0;
    }
}
/* Public functions ----------------------------------------------------------*/


/**
  * @brief Light initialization routine. Config TIM1 frequencys, etc.
  * @param  None
  * @retval None
  */
void appBuzzerInit(void)
{
    
    
}

bool appBuzzerBeep(APP_BUZZ_SOUND_t beep)
{
    if(beepTid < 0)
    {
        if(beep == APP_BUZZ_BUTTON)
        {
            beepTid = appTaskSchedCreate(SINGLE_BEEP_LAST, appBuzzerSoundButton);
        }
        else 
        {
            beepTid = appTaskSchedCreate(SINGLE_BEEP_LAST, appBuzzerSoundError);
        }
        return TRUE;
    }

    return FALSE;
    
}
#endif /* APP_BUZZER_EN */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/