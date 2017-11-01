/**
  ******************************************************************************
  * @file    app_events.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    9-AUG-2017
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
#ifndef __APP_EVENTS_H
#define __APP_EVENTS_H

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"


#if APP_EVENTS_EN > 0
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appEventPowerOnReset(void);
void appEventMistButtonTouchEnter(void);
void appEventMistButtonPress(void);
void appEventMistButtonLongPress(void);
void appEventFreqHopCfm(void);
void appEventNoWater(void);
#endif /* APP_EVENTS_EN > 0 */

#endif /* __APP_EVENTS_H */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/
