/**
  ******************************************************************************
  * @file    hal_led.c
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

#include "..\hal\hal_led.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_LED_EN > 0



#define IO_LED_MIST     P03
#define IO_LED_R        P00
#define IO_LED_G        P12
#define IO_LED_B        P01


extern void halLedInit(void)
{
    IO_LED_MIST = 0;
	IO_LED_R = 0;
	IO_LED_G = 0;
	IO_LED_B = 0;
    P03_PushPull_Mode; //HAL_LED_MIST
	P00_PushPull_Mode; //HAL_LED_R
    P12_PushPull_Mode; //HAL_LED_G
    P01_PushPull_Mode; //HAL_LED_B

    
}
extern void halLedOn(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_MIST:
            IO_LED_MIST = 1;
        break;
        
        case HAL_LED_R:
            IO_LED_R = 1;
        break;

        case HAL_LED_G:
            IO_LED_G = 1;
        break;

        case HAL_LED_B:
            IO_LED_B = 1;
        break;
    }
}
extern void halLedOff(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_MIST:
            IO_LED_MIST = 0;
        break;
        
        case HAL_LED_R:
            IO_LED_R = 0;
        break;

        case HAL_LED_G:
            IO_LED_G = 0;
        break;

        case HAL_LED_B:
            IO_LED_B = 0;
        break;
    }
}
extern void halLedToggle(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_MIST:
            IO_LED_MIST = !IO_LED_MIST;
        break;
        
        case HAL_LED_R:
            IO_LED_R = !IO_LED_R;
        break;

        case HAL_LED_G:
            IO_LED_G = !IO_LED_G;
        break;

        case HAL_LED_B:
            IO_LED_B = !IO_LED_B;
        break;
    }
}
extern HAL_LED_STATE_t halLedState(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_MIST:
            return (IO_LED_MIST == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
        case HAL_LED_R:
            return (IO_LED_R == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
        case HAL_LED_G:
            return (IO_LED_G == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
        case HAL_LED_B:
            return (IO_LED_B == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
    }

    return HAL_LED_STATE_OFF;
}


#endif /* HAL_LED_EN > 0 */

