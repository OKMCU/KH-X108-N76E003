/**
  ******************************************************************************
  * @file    hal_freqhop.c
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

#include "..\hal\hal_freqhop.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"
#include "..\bsp\bsp_clk.h"
#include "..\bsp\bsp_systick.h"
#include "..\bsp\bsp_uart.h"

#if HAL_FREQHOP_EN > 0

extern void halFreqHopSet(uint8_t freq_lvl)
{
    BSP_HSI_CFG_t bspHsiCfg;
    
    if(freq_lvl >= (uint8_t)BSP_HSI_CFG_17000kHz ||
       freq_lvl <= (uint8_t)BSP_HSI_CFG_17320kHz)
    {
        bspHsiCfg = bspClkGetCfg();
        if(bspHsiCfg != (BSP_HSI_CFG_t)freq_lvl)
        {
            bspClkInit((BSP_HSI_CFG_t)freq_lvl);
            bspSystickUpdate();
#if BSP_UART_EN > 0
            bspUart0Update_Timer3(9600);
#endif /* BSP_UART_EN > 0 */
        }
    }
}

extern uint8_t halFreqHopGet(void)
{
    BSP_HSI_CFG_t bspHsiCfg;
    bspHsiCfg = bspClkGetCfg();
    return (uint8_t)bspHsiCfg;
}

#endif /* HAL_FREQHOP_EN > 0 */

