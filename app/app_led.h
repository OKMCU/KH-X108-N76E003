/**
  ******************************************************************************
  * @file    app_led.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_LED_H
#define __APP_LED_H

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"


#if APP_LED_EN > 0
/* Exported types ------------------------------------------------------------*/
typedef enum {
    LED_MIST = 0,
    LED_R = 1,
    LED_G = 2,
    LED_B = 3
} LED_t;

typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_ON = !LED_STATE_OFF
} LED_STATE_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appLedInit(void);
void appLedOn(LED_t led);
void appLedOff(LED_t led);
void appLedToggle(LED_t led);
LED_STATE_t appLedState(LED_t led);


#endif /* APP_LED_EN */

#endif /* __APP_LED_H */


/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/
