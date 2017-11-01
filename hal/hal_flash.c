/**
  ******************************************************************************
  * @file    hal_flash.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    19-OCT-2017
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

#include "..\hal\hal_flash.h"
#include "..\bsp\N76E003.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\bsp\common.h"
#include "..\bsp\interrupt.h"

#if HAL_FLASH_EN > 0
#define  PAGE_ERASE_AP   0x22 
#define  BYTE_PROGRAM_AP  0x21 

volatile uint8_t code Data_Flash[128] _at_ 0x0200;

extern void halFlashErase(void)
{
    disable_interrupt();
    TA = 0xAA;        //CHPCON is TA protected 
    TA = 0x55; 
    CHPCON |= 0x01;       //IAPEN = 1, enable IAP mode 

    TA = 0xAA;        //IAPUEN is TA protected 
    TA = 0x55; 
    IAPUEN |= 0x01;       //APUEN = 1, enable APROM update 
    
    IAPCN = PAGE_ERASE_AP;    //Erase page 200h~27Fh 
    IAPAH = 0x02; 
    IAPAL = 0x00; 
    IAPFD = 0xFF;
    
    TA = 0xAA;        //IAPTRG is TA protected 
    TA = 0x55;
    IAPTRG |= 0x01;      //write ‘1’ to IAPGO to trigger IAP process 
    
    TA = 0xAA;        //IAPUEN is TA protected 
    TA = 0x55; 
    IAPUEN &= ~0x01;       //APUEN = 0, disable APROM update 
 
    TA = 0xAA;        //CHPCON is TA protected 
    TA = 0x55; 
    CHPCON &= ~0x01;       //IAPEN = 0, disable IAP mode 
    enable_interrupt();
}
extern void halFlashWrite(const uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint16_t addr = 0x0200;

    disable_interrupt();
    TA = 0xAA;        //CHPCON is TA protected 
    TA = 0x55; 
    CHPCON |= 0x01;       //IAPEN = 1, enable IAP mode 

    TA = 0xAA;        //IAPUEN is TA protected 
    TA = 0x55; 
    IAPUEN |= 0x01;       //APUEN = 1, enable APROM update 
    
    for(i = 0; i < len && i < sizeof(Data_Flash); i++)
    {
        IAPCN = BYTE_PROGRAM_AP;    // Program 201h with 55h 
        IAPAH = ((addr+i)>>8) & 0xFF; 
        IAPAL = (addr+i) & 0xFF; 
        IAPFD = buf[i];
        
        TA = 0xAA; 
        TA = 0x55; 
        IAPTRG |= 0x01;      //write ‘1’ to IAPGO to trigger IAP process 
    }
    
    TA = 0xAA;        //IAPUEN is TA protected 
    TA = 0x55; 
    IAPUEN &= ~0x01;       //APUEN = 0, disable APROM update 
 
    TA = 0xAA;        //CHPCON is TA protected 
    TA = 0x55; 
    CHPCON &= ~0x01;       //IAPEN = 0, disable IAP mode 
    enable_interrupt();
}
extern void halFlashRead(uint8_t *buf, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < len && i < sizeof(Data_Flash); i++)
    {
        buf[i] = Data_Flash[i];
    }
}



#endif /* HAL_FLASH_EN > 0 */

