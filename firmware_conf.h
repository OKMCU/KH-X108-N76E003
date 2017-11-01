/**
  ******************************************************************************
  * @file    firmware_conf.h
  * @author  SUN Wentao
  * @version V0x0.1
  * @date    10-Aug-2017
  * @brief   This file is used to configure the whole firmware.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 OKMCU</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.okmcu.com
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
#ifndef __FIRMWARE_CONF_H
#define __FIRMWARE_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define     DEBUG_APP_BUTTON_EN     0
#define     DEBUG_APP_SPRAY_EN      0
#define     DEBUG_APP_EVENTS_EN     0

#define     APP_CMD_EN              0
#define     APP_CMD_TEST_EN         0
#define     APP_CMD_SPRAY_EN        0
#define     APP_CMD_WATERCHK_EN     0
#define     APP_CMD_CLKO_EN         0
#define     APP_CMD_EVENT_EN        0
#define     APP_LIGHT_EN            1
#define     APP_SPRAY_EN            1
#define     APP_TASKSCHED_EN        1
#define     APP_BUTTON_EN           1
#define     APP_EVENTS_EN           1
#define     APP_BUZZER_EN           1
#define     APP_LED_EN              1
#define     APP_WATER_CHK_EN        1
#define     APP_FREQHOP_EN          1

#define     HAL_CONSOLE_EN          0
#define     HAL_LED_EN              1
#define     HAL_SPRAY_EN            1
#define     HAL_WATER_CHK_EN        1
#define     HAL_BUTTON_EN           1
#define     HAL_FAN_EN              1
#define     HAL_FREQHOP_EN          1
#define     HAL_LIGHT_EN            1
#define     HAL_BUZZER_EN           1
#define     HAL_FLASH_EN            1

#define     BSP_UART_EN             0


#define     LIB_NUM_STRING_EN       0
#define     LIB_STRINGX_EN          0

/* Exported functions ------------------------------------------------------- */

#endif /* __FIRMWARE_CONF_H */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/
