/**
  ******************************************************************************
  * @file    hal_led.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    18-SEP-2017
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

#ifndef __HAL_LED_H__
#define __HAL_LED_H__

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"

#if HAL_LED_EN > 0
typedef enum {

    HAL_LED_Blue = 0,
    HAL_LED_Green = 1,
    
} HAL_LED_t;

typedef enum {
    
    HAL_LED_STATE_OFF = 0,
    HAL_LED_STATE_ON = !HAL_LED_STATE_OFF
    
} HAL_LED_STATE_t;

extern void halLedInit(void);
extern void halLedOn(HAL_LED_t led);
extern void halLedOff(HAL_LED_t led);
//extern void halLedToggle(HAL_LED_t led);
//extern HAL_LED_STATE_t halLedState(HAL_LED_t led);

#endif /* HAL_LED_EN > 0 */

#endif /* __HAL_LED_H__ */

