/**
  ******************************************************************************
  * @file    interrupt.c
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
#include "..\bsp\bsp_uart.h"
#include "..\bsp\SFR_Macro.h"
#include "..\bsp\Function_define.h"
#include "..\bsp\stdint.h"
#include "..\hal\hal_led.h"
#include "..\hal\hal_water_chk.h"
#include "..\hal\hal_light.h"
#include "..\hal\hal_buzzer.h"
#include "..\app\app_tasksched.h"
#include "firmware_conf.h"

void Timer0_ISR (void) interrupt 1
{
    halBuzzerToggle();
}

void Timer1_ISR(void) interrupt 3
{
    halLightUpdate();
}

#if BSP_UART_EN > 0
/**
 * FUNCTION_PURPOSE: serial interrupt, echo received data.
 * FUNCTION_INPUTS: P0.7(RXD) serial input
 * FUNCTION_OUTPUTS: P0.6(TXD) serial output
 */
void SerialPort0_ISR(void) interrupt 4 
{
    if(RI) 
    {                                       /* if reception occur */
        clr_RI;                             /* clear reception flag for next reception */
        bspUart0RxCharISR(SBUF);
    }
    if(TI)
    {
        clr_TI;                             /* if emission occur */
        bspUart0TxCharISR();
    }
}
#endif /* BSP_UART_EN > 0 */


void Timer2_ISR (void) interrupt 5
{
	  clr_TF2;      //Clear Timer2 Interrupt Flag
//	  P12 = ~P12;   // GPIO1 toggle
      //printf("\n TM2 INT 0x%bX",RCMP2H);
#if APP_TASKSCHED_EN > 0
      appTaskSchedSystickCb();
#endif /* APP_TASKSCHED_EN > 0 */
      //halLedToggle();
}


//void ADC_ISR (void) interrupt 11
//{
//    clr_ADCF;                               //clear ADC interrupt flag
//    halLedToggle();
//    halWaterChkIsr((uint16_t)(ADCRH << 8) | (uint16_t)(ADCRL));
//}

