/**
  ******************************************************************************
  * @file    bsp_clk.c
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
#include "..\bsp\N76E003.h"
#include "..\bsp\bsp_clk.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"


//static xdata BSP_HSI_CFG_t hsiCfg = BSP_HSI_CFG_16000kHz;
//static code uint8_t rctrim[] = {0, 25, 26, 27, 28, 29, 30, 31, 32, 33};
static xdata uint8_t hsiCfg = 0;

void bspClkInit(uint8_t bspHsiCfg)
{
    uint8_t hircmap0,hircmap1;
    uint16_t trimvalue16bit;
    bool BIT_TMP;

    set_IAPEN;
    IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPGO;
    hircmap0 = IAPFD;
    IAPAL = 0x31;
    IAPAH = 0x00;
    set_IAPGO;
    hircmap1 = IAPFD;
    clr_IAPEN;
    trimvalue16bit = ((hircmap0<<1)+(hircmap1&0x01));
    trimvalue16bit = trimvalue16bit - bspHsiCfg;
    hircmap1 = trimvalue16bit&0x01;
    hircmap0 = trimvalue16bit>>1;
    TA=0XAA;
    TA=0X55;
    RCTRIM0 = hircmap0;
    TA=0XAA;
    TA=0X55;
    RCTRIM1 = hircmap1;

    hsiCfg = bspHsiCfg;
}

uint8_t bspClkGetCfg(void)
{
    return hsiCfg;
}

void bspClkOutEnable(void)
{
    P11_PushPull_Mode;
    set_CLOEN;
}
void bspClkOutDisable(void)
{
    P11_PushPull_Mode;
    clr_CLOEN;
}

