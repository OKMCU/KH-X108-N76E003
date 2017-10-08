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

extern void halLedInit(void)
{
    P06 = 0;
    P06_PushPull_Mode; //HAL_LED_Blue

    P03 = 0;
    P03_PushPull_Mode; //HAL_LED_Green
}
extern void halLedOn(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_Blue:
            P06 = 1;
        break;

        case HAL_LED_Green:
            P03 = 1;
        break;
    }
}
extern void halLedOff(HAL_LED_t led)
{
    switch (led)
    {
        case HAL_LED_Blue:
            P06 = 0;
        break;

        case HAL_LED_Green:
            P03 = 0;
        break;
    }
}
//extern void halLedToggle(HAL_LED_t led)
//{
//    switch (led)
//    {
//        case HAL_LED_Blue:
//            P06 = !P06;
//        break;
//
//        case HAL_LED_Green:
//            P03 = !P03;
//        break;
//    }
//}
//extern HAL_LED_STATE_t halLedState(HAL_LED_t led)
//{
//    switch (led)
//    {
//        case HAL_LED_Blue:
//            return (P06 == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
//
//        case HAL_LED_Green:
//            return (P03 == 0) ? HAL_LED_STATE_OFF : HAL_LED_STATE_ON;
//    }
//
//    return HAL_LED_STATE_OFF;
//}


#endif /* HAL_LED_EN > 0 */

