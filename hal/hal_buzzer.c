/**
  ******************************************************************************
  * @file    hal_buzzer.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    15-OCT-2017
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

#include "..\hal\hal_buzzer.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_BUZZER_EN > 0

#define AUTO_RELOAD_VALUE   280     //about 5kHz intterrupt
extern void halBuzzerInit(void)
{
    P05 = 0;
    P05_PushPull_Mode;

    TIMER0_MODE1_ENABLE;
}
extern void halBuzzerOn(void)
{
    TH0 = ((65536-AUTO_RELOAD_VALUE) >> 8) & 0xFF;
    TH1 = (65536-AUTO_RELOAD_VALUE) & 0xFF;
    set_ET0;
    set_TR0;
}
extern void halBuzzerOff(void)
{
    clr_ET0;
    clr_TR0;
    P05 = 0;
}

extern void halBuzzerToggle(void)
{
    clr_TR0;
    TH0 = ((65536-AUTO_RELOAD_VALUE) >> 8) & 0xFF;
    TH1 = (65536-AUTO_RELOAD_VALUE) & 0xFF;
    set_TR0;
    P05 = !P05;
}
#endif /* HAL_BUZZER_EN > 0 */

