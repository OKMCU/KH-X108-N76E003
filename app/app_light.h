/**
  ******************************************************************************
  * @file    app_light.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    08-OCT-2017
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
#ifndef __APP_LIGHT_H
#define __APP_LIGHT_H

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"


#if APP_LIGHT_EN > 0
/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} APP_LIGHT_COLOR_t;
/* Exported constants --------------------------------------------------------*/
typedef enum {
    APP_LIGHT_R = 0,
    APP_LIGHT_G,
    APP_LIGHT_B
} APP_LIGHT_t;
/* Exported macro ------------------------------------------------------------*/
#define COLOR_TABLE_SEL_SIZE  6
/* Exported functions ------------------------------------------------------- */
void appLightSet(const APP_LIGHT_COLOR_t *color);
void appLightSetMode(uint8_t lightMode);
void appLightSrv(void);
#endif /* APP_LIGHT_EN */

#endif /* __APP_LIGHT_H */


/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/
