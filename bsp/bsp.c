/**
  ******************************************************************************
  * @file    app_bsp.c
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

#include "..\bsp\function_define.h"
#include "..\bsp\bsp_uart.h"
#include "..\bsp\bsp_clk.h"
#include "..\bsp\interrupt.h"
#include "..\bsp\bsp_systick.h"
#include "firmware_conf.h"
void bspInit(void)
{
    //P11_PushPull_Mode;

    //bspClkInit(BSP_HSI_CFG_17000kHz);
    //bspClkInit(BSP_HSI_CFG_17040kHz);
    //bspClkInit(BSP_HSI_CFG_17080kHz);
    //bspClkInit(BSP_HSI_CFG_17120kHz);
    //bspClkInit(BSP_HSI_CFG_17160kHz);
    bspClkInit(BSP_HSI_CFG_17200kHz);
    //bspClkInit(BSP_HSI_CFG_17240kHz);
    //bspClkInit(BSP_HSI_CFG_17280kHz);
    //bspClkInit(BSP_HSI_CFG_17320kHz);
    
#if BSP_UART_EN > 0
    bspUart0Init_Timer3(9600);
#endif /* BSP_UART_EN > 0 */
    
    bspSystickInit();
    
    enable_interrupt();
}