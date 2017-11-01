/**
  ******************************************************************************
  * @file    app_freqhop.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    13-OCT-2017
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
#ifndef __APP_FREQHOP_H__
#define __APP_FREQHOP_H__

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "firmware_conf.h"

#if APP_FREQHOP_EN > 0
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void appFreqHop_Init(void);
extern void appFreqHop_SetEnable(void);
extern bool appFreqHop_IsEnabled(void);
extern void appFreqHop_SaveFreqToFlash(void);
extern void appFreqHop_Reset(void);
extern bool appFreqHop_Timeout(void);
extern bool appFreqHop_UpdateFreq(void);
extern bool appFreqHop_RoughTuneFinished(void);
extern bool appFreqHop_FineTuneFinished(void);
extern bool appFreqHop_LockFreq(void);
#endif /* APP_FREQHOP_EN > 0 */


#endif /* __APP_FREQHOP_H__ */
/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/







