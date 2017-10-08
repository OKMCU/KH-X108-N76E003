/**
  ******************************************************************************
  * @file    bsp_uart.h
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

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "firmware_conf.h"

#if BSP_UART_EN > 0
//void  InitialUART0_Timer1(UINT32 u32Baudrate); //T1M = 1, SMOD = 1
void  bspUart0Init_Timer3(uint32_t u32Baudrate); //Timer3 as Baudrate, SMOD=1, Prescale=0
void  bspUart0Update_Timer3(uint32_t u32Baudrate);
//void  InitialUART1_Timer3(UINT32 u32Baudrate);
//void  InitialUART1(UINT32 u32Baudrate);
bool bspUart0CharAvail(void);
char bspUart0CharGet(void);
void bspUart0CharPut(char c);
void bspUart0RxCharISR(uint8_t c);
void bspUart0TxCharISR(void);

char putchar(char c);
//char getchar(void);
#endif /* BSP_UART_EN > 0 */

#endif /* __BSP_UART_H__ */