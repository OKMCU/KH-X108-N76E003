/**
  ******************************************************************************
  * @file    app_buzzer.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    08-OCT-2017
  * @brief   This file contains the APIs of tri-color LED control.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_BUZZER_H
#define __APP_BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include "firmware_conf.h"
#include "..\bsp\typedef.h"

#if APP_BUZZER_EN > 0
/* Exported types ------------------------------------------------------------*/
typedef enum {
    APP_BUZZ_BUTTON = 0,
    APP_BUZZ_ERROR,
    APP_BUZZ_FACTORY,
    APP_BUZZ_HIGHPOWER,
    APP_BUZZ_LOWPOWER
} APP_BUZZ_SOUND_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appBuzzerInit(void);
bool appBuzzerBeep(APP_BUZZ_SOUND_t beep);
#endif /* APP_BUZZER_EN */

#endif /* __APP_BUZZER_H */


/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/
