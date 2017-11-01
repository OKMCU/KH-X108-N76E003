/**
  ******************************************************************************
  * @file    hal_freqhop.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    29-SEP-2017
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

#ifndef __HAL_FREQHOP_H__
#define __HAL_FREQHOP_H__

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"

#if HAL_FREQHOP_EN > 0

extern void halFreqHopSet(uint8_t freq_lvl);
extern uint8_t halFreqHopGet(void);

#endif /* HAL_FREQHOP_EN > 0 */

#endif /* __HAL_FREQHOP_H__ */

