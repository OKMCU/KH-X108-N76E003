/**
  ******************************************************************************
  * @file    hal_light.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    14-OCT-2017
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

#include "..\hal\hal_light.h"
#include "..\hal\hal_led.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_LIGHT_EN > 0

#define IO_LIGHT_R        P00
#define IO_LIGHT_G        P12
#define IO_LIGHT_B        P01

#define AUTO_RELOAD_VALUE 70

static uint8_t color[3] = {0};
static uint8_t count = 0;

extern void halLightInit(void)
{   
    TIMER1_MODE2_ENABLE;
    TH1 = 256-AUTO_RELOAD_VALUE;
    TL1 = 256-AUTO_RELOAD_VALUE;
    //set_T1M;
    set_ET1;
    set_TR1;
}

extern void halLightSet(HAL_LIGHT_t light, uint8_t bright)
{
    color[light] = bright>>1;
    switch(light)
    {
        case HAL_LIGHT_R:
            IO_LIGHT_R = count < color[0] ? 1 : 0;
        break;
        case HAL_LIGHT_G:
            IO_LIGHT_G = count < color[1] ? 1 : 0;
        break;
        case HAL_LIGHT_B:
            IO_LIGHT_B = count < color[2] ? 1 : 0;
        break;
    }
}

extern void halLightUpdate(void)
{
    count++;
    //clr_TR1;
    //count = ((uint16_t)TH1 << 8) | (uint16_t)TL1;
    //set_TR1;
    
    IO_LIGHT_R = count < color[0] ? 1 : 0;
    IO_LIGHT_G = count < color[1] ? 1 : 0;
    IO_LIGHT_B = count < color[2] ? 1 : 0;
}

#endif /* HAL_LIGHT_EN > 0 */

