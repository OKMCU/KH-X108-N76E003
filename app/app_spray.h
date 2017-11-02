/**
  ******************************************************************************
  * @file    app_spray.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    20-SEP-2017
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
#ifndef __APP_SPRAY_H__
#define __APP_SPRAY_H__

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "firmware_conf.h"

#if APP_SPRAY_EN > 0
/* Exported types ------------------------------------------------------------*/
typedef struct {
    
    uint8_t onPeriodInSec;
    uint8_t offPeriodInSec;
    uint16_t totalTimeInMin;
    
} APP_SPRAY_PROFILE_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void    appSprayInit (void);
extern void    appSprayResetWaterChkData(void);
extern void    appSpraySet(const APP_SPRAY_PROFILE_t *profile);
extern void    appSpraySetPower(uint8_t on_time_ms, uint8_t off_time_ms);
#endif /* APP_SPRAY_EN > 0 */


#endif /* __APP_SPRAY_H__ */
/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/







