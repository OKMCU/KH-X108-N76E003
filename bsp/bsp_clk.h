/**
  ******************************************************************************
  * @file    bsp_clk.h
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

#ifndef __BSP_CLK_H__
#define __BSP_CLK_H__

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"

//#define FSYS_160000
//#define FSYS_166000
//#define FSYS_1700kHz
//#define FSYS_170000

/*
typedef enum {

    BSP_HSI_CFG_16000kHz = 0,
    BSP_HSI_CFG_17000kHz = 1,
    BSP_HSI_CFG_17040kHz = 2,
    BSP_HSI_CFG_17080kHz = 3,
    BSP_HSI_CFG_17120kHz = 4,
    BSP_HSI_CFG_17160kHz = 5,
    BSP_HSI_CFG_17200kHz = 6,
    BSP_HSI_CFG_17240kHz = 7,
    BSP_HSI_CFG_17280kHz = 8,
    BSP_HSI_CFG_17320kHz = 9,
    
} BSP_HSI_CFG_t;
*/

void bspClkInit(uint8_t bspHsiCfg);
uint8_t bspClkGetCfg(void);
void bspClkOutEnable(void);
void bspClkOutDisable(void);


#endif
