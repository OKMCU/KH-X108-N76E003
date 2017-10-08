/**
  ******************************************************************************
  * @file    hal_water_chk.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    19-SEP-2017
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

#include "..\hal\hal_water_chk.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"

#if HAL_WATER_CHK_EN > 0

extern void     halWaterChkInit(void)
{
    Enable_ADC_AIN5;
    clr_ADCF;
    //set_EADC;//enable interrupt
}
extern void     halWaterChkStart(void)
{
    clr_ADCF;
    set_ADCS;//trigger ADC
}
extern bool     halWaterChkResultReady(void)
{
    if(ADCF)
    {
        clr_ADCF;//clear the convertion finish flag
        return TRUE;
    }

    return FALSE;
}
extern uint16_t halWaterChkGetResult(void)
{
    return ((uint16_t)ADCRH << 4) | (uint16_t)(ADCRL&0x0F);//ADCRH[11:4] stores MSB, ADCRL[3:0] stores LSB
}

#endif
//extern void     halWaterChkIsr(uint16_t result)
//{
//    set_ADCS;
//    maxAdcVal = result > maxAdcVal ? result : maxAdcVal;
//}

