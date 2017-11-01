/**
  ******************************************************************************
  * @file    bsp_systick.h
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    9-AUG-2017
  * @brief   This file contains the headers of the interrupt handlers
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


/* Includes ------------------------------------------------------------------*/
#include "..\bsp\bsp_systick.h"
#include "..\bsp\bsp_clk.h"
#include "..\bsp\Function_Define.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"

/** @addtogroup Template_Project
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
static code uint16_t reloadValue_1ms[] = {
    65536 - 16000,
    65536 - 17000,
    65536 - 17040,
    65536 - 17080,
    65536 - 17120,
    65536 - 17160,
    65536 - 17200,
    65536 - 17240,
    65536 - 17280,
    65536 - 17320
};
*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/


/**
  * @brief Systick initialization routine. 1ms interrupt.
  * @param  None
  * @retval None
  */
void bspSystickInit(void)
{
    uint8_t bspHsiCfg;
    uint16_t reloadValue_1ms = UINT16_MAX-16000+1;
    
    TIMER2_DIV_1;
    TIMER2_Auto_Reload_Delay_Mode;

    bspHsiCfg = bspClkGetCfg();

    reloadValue_1ms -= (uint16_t)bspHsiCfg*40;
    RCMP2L = (uint8_t)(reloadValue_1ms & 0xFF);
    RCMP2H = (uint8_t)(reloadValue_1ms >> 8);
    TL2 = 0;
    TH2 = 0;

    set_ET2;                                    // Enable Timer2 interrupt
    //set_EA;
    set_TR2;                                    // Timer2 run
}

void bspSystickUpdate(void)
{
    uint8_t bspHsiCfg;
    uint16_t th2;
    uint16_t reloadValue_1ms = UINT16_MAX-16000+1;
    
    clr_ET2;
    clr_TR2;
    
    bspHsiCfg = bspClkGetCfg();

    reloadValue_1ms -= (uint16_t)bspHsiCfg*40;
    RCMP2L = (uint8_t)(reloadValue_1ms & 0xFF);
    RCMP2H = (uint8_t)(reloadValue_1ms >> 8);

    th2 = (uint16_t)TH2;
    th2 <<= 8;
    th2 += (uint16_t)TL2;
    
    if(th2 >= reloadValue_1ms)
    {
        th2 = reloadValue_1ms - 1;
        TH2 = (th2 >> 8) & 0xFF;
        TL2 = th2 & 0xFF;
    }
    
    set_ET2;
    set_TR2;
}

/**
  * @}
  */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/