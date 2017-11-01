/**
  ******************************************************************************
  * @file    hal_light.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    14-OCT-2017
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

#ifndef __HAL_LIGHT_H__
#define __HAL_LIGHT_H__

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"

#if HAL_LIGHT_EN > 0
typedef enum {
    HAL_LIGHT_R = 0,
    HAL_LIGHT_G,
    HAL_LIGHT_B
} HAL_LIGHT_t;

extern void halLightInit(void);
extern void halLightSet(HAL_LIGHT_t light, uint8_t bright);
extern void halLightUpdate(void);
#endif /* HAL_LIGHT_EN > 0 */

#endif /* __HAL_LIGHT_H__ */

