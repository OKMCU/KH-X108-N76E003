/**
  ******************************************************************************
  * @file    hal_spray.c
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

#include "..\hal\hal_spray.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_SPRAY_EN > 0

extern void halSprayInit(void)
{
    P10 = 0;
    P10_PushPull_Mode;
    //PWM2_P10_OUTPUT_ENABLE;
    PWM_IMDEPENDENT_MODE;
    

    // PWM frequency = Fpwm/((PWMPH,PWMPL) + 1), where Fpwm = Fsys/PWM_CLOCK_DIV
    //               = (17MHz/1)/(0x0009 + 1)
    //               = 1.7MHz
    //PWM_CLOCK_DIV_1;
    PWMPH = 0x00;
    PWMPL = 0x09;
    
    //  PWM duty = 50%
    PWM2H = 0x00;						
    PWM2L = 0x05;
    
    set_LOAD;
}
extern void halSprayOn(void)
{
    PWM2_P10_OUTPUT_ENABLE;
    set_PWMRUN;
}
extern void halSprayOff(void)
{
    clr_PWMRUN;
    PWM2_P10_OUTPUT_DISABLE;
}

#endif /* HAL_SPRAY_EN > 0 */

