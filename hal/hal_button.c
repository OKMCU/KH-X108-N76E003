/**
  ******************************************************************************
  * @file    hal_button.c
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

#include "..\hal\hal_button.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_BUTTON_EN > 0

extern void halButtonInit(void)
{
    P06_Quasi_Mode;//Mode button
    
}
extern HAL_BUTTON_STATE_t halButtonGetState(HAL_BUTTON_t button)
{
    switch (button)
    {
        case HAL_BUTTON_Mist:
        return (P06 == 0) ? HAL_BUTTON_STATE_Pressed : HAL_BUTTON_STATE_Released;
        
        default:
        break;
    }
    
    return HAL_BUTTON_STATE_Released;
}

#endif /* HAL_BUTTON_EN > 0 */
