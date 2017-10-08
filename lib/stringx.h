/**
  ******************************************************************************
  * @file    stringx.h
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

#ifndef __STRINGX_H__
#define __STRINGX_H__

#include "..\bsp\stdint.h"
#include "firmware_conf.h"

#if LIB_STRINGX_EN > 0
char *strtok_r(char *s, const char *delim, char **save_ptr);
#endif /* LIB_STRINGX_EN */


#if LIB_NUM_STRING_EN > 0
extern uint8_t hex2int(char *hexStr, int32_t *num);
extern uint8_t hex2uint(char *hexStr, uint32_t *num);
extern uint8_t dec2int(char *decStr, int32_t *num);
#endif /* LIB_NUM_STRING_EN */


#endif

