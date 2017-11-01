/**
  ******************************************************************************
  * @file    app_button.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    19-SEP-2017
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
#ifndef __APP_BUTTON_H
#define __APP_BUTTON_H

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"


#if APP_BUTTON_EN > 0
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define APP_BUTTON_STATE_MIST       (uint8_t)(1<<0)
/* Exported functions ------------------------------------------------------- */
void appButtonInit(void);
uint8_t appButtonGetCurrentState(void);
#endif /* APP_BUTTON_EN */

#endif /* __APP_BUTTON_H */


/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/
