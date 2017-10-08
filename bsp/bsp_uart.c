/**
  ******************************************************************************
  * @file    bsp_uart.c
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
#include "..\bsp\bsp_clk.h"
#include "..\bsp\typedef.h"

#if BSP_UART_EN > 0

#define     UART0_RX_BUFFER_SIZE       16


static xdata char     rxBuf[UART0_RX_BUFFER_SIZE];
static xdata uint16_t ptrRxChar = 0;
static xdata uint16_t ptrRdChar = 0;
static bool     btf_flag = FALSE;
static code uint32_t baudrateFactor[] = {
    1000000,
    1062500,
    1065000,
    1067500,
    1070000,
    1072500,
    1075000,
    1077500,
    1080000,
    1082500,
};

//----------------------------------------------------------------------------------
// UART0 baud rate initial setting 
//----------------------------------------------------------------------------------
//void InitialUART0_Timer1(UINT32 u32Baudrate)    //T1M = 1, SMOD = 1
//{
//    P06_Quasi_Mode;        //Setting UART pin as Quasi mode for transmit
//    P07_Quasi_Mode;        //Setting UART pin as Quasi mode for transmit
//    
//    SCON = 0x50;         //UART0 Mode1,REN=1,TI=1
//    TMOD |= 0x20;        //Timer1 Mode1
//    
//    set_SMOD;            //UART0 Double Rate Enable
//    set_T1M;
//    clr_BRCK;            //Serial port 0 baud rate clock source = Timer1
// 
//#ifdef FSYS_160000
//    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
//#endif        
//#ifdef FSYS_166000
//    TH1 = 256 - (1037500/u32Baudrate);                      /*16.6 MHz */
//#endif
//    set_TR1;
//        set_TI;                        //For printf function must setting TI = 1
//}
//---------------------------------------------------------------
void bspUart0Init_Timer3(uint32_t u32Baudrate) //use timer3 as Baudrate generator
{
    BSP_HSI_CFG_t bspHsiCfg;
    
    P06_Quasi_Mode;         //Setting UART pin as Quasi mode for transmit
    P07_Quasi_Mode;         //Setting UART pin as Quasi mode for transmit    
    
    SCON = 0x50;            //UART0 Mode1,REN=1,TI=1
    set_SMOD;               //UART0 Double Rate Enable
    T3CON &= 0xF8;          //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_BRCK;               //UART0 baud rate clock source = Timer3

#ifdef FSYS_160000
    RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);           /*16 MHz */
    RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);           /*16 MHz */
#endif
#ifdef FSYS_166000
    RH3    = HIBYTE(65536 - (1037500/u32Baudrate)-1);           /*16.6 MHz */
    RL3    = LOBYTE(65536 - (1037500/u32Baudrate)-1);           /*16.6 MHz */
#endif
#ifdef FSYS_1700kHz
    RH3    = HIBYTE(65536 - (106250/u32Baudrate)-1);            /*1.7 MHz */
    RL3    = LOBYTE(65536 - (106250/u32Baudrate)-1);            /*1.7 MHz */
#endif
#ifdef FSYS_170000
    RH3    = HIBYTE(65536 - (1062500/u32Baudrate)-1);           /*17 MHz */
    RL3    = LOBYTE(65536 - (1062500/u32Baudrate)-1);           /*17 MHz */
#endif

    bspHsiCfg = bspClkGetCfg();
    RH3    = HIBYTE(65536 - (baudrateFactor[(uint8_t)bspHsiCfg]/u32Baudrate)-1);
    RL3    = LOBYTE(65536 - (baudrateFactor[(uint8_t)bspHsiCfg]/u32Baudrate)-1);
    
    set_TR3;            //Trigger Timer3
    set_TI;             //For printf function must setting TI = 1
    set_ES;
}

void  bspUart0Update_Timer3(uint32_t u32Baudrate)
{
    BSP_HSI_CFG_t bspHsiCfg;

    clr_ES;
    clr_TI;
    clr_TR3;


    bspHsiCfg = bspClkGetCfg();
    RH3    = HIBYTE(65536 - (baudrateFactor[(uint8_t)bspHsiCfg]/u32Baudrate)-1);
    RL3    = LOBYTE(65536 - (baudrateFactor[(uint8_t)bspHsiCfg]/u32Baudrate)-1);

    set_TR3;
    set_TI;
    set_ES;
}

bool bspUart0CharAvail(void)
{
    return ptrRxChar != ptrRdChar ? TRUE : FALSE;
}
char BspUart0CharGet(void)
{
    char c = 0;
        
    if(ptrRxChar != ptrRdChar)
    {
        c = rxBuf[ptrRdChar++];
        if(ptrRdChar >= UART0_RX_BUFFER_SIZE)
        {
            ptrRdChar = 0;
        }
    }
    
    return c;
}

void bspUart0CharPut(char c)
{
    while(btf_flag == FALSE);
    btf_flag = FALSE;
    SBUF = c;
}

void bspUart0RxCharISR(uint8_t c)
{
    xdata uint16_t tmp;
    
    tmp = ptrRxChar;
    tmp++;
    if(tmp >= UART0_RX_BUFFER_SIZE)
    {
        tmp = 0;
    }
    
    if(tmp == ptrRdChar)
        return;
    
	rxBuf[ptrRxChar++] = (char)c;
    if(ptrRxChar >= UART0_RX_BUFFER_SIZE)
    {
        ptrRxChar = 0;
    }
}

void bspUart0TxCharISR(void)
{
    btf_flag = TRUE;
}


char putchar(char c)
{
    bspUart0CharPut(c);
    
    return c;
}


#endif /* BSP_UART_EN > 0 */

//char getchar(void)
//{
//    while(BspUart0CharAvail() == FALSE);
//    return BspUart0CharGet();
//}

//----------------------------------------------------------------------------------
// UART1 baud rate initial setting 
//----------------------------------------------------------------------------------
//void InitialUART1_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
//{
//        P02_Quasi_Mode;        //Setting UART pin as Quasi mode for transmit
//        P16_Quasi_Mode;        //Setting UART pin as Quasi mode for transmit
//    
//      SCON_1 = 0x50;       //UART1 Mode1,REN_1=1,TI_1=1
//    T3CON = 0x08;       //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1), UART1 in MODE 1
//        clr_BRCK;
//    
//#ifdef FOSC_160000
//        RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);          /*16 MHz */
//        RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);            /*16 MHz */
//#endif
//#ifdef FOSC_166000
//        RH3    = HIBYTE(65536 - (1037500/u32Baudrate));              /*16.6 MHz */
//        RL3    = LOBYTE(65536 - (1037500/u32Baudrate));                /*16.6 MHz */
//#endif
//    set_TR3;         //Trigger Timer3
//}

//UINT8 Receive_Data_From_UART1(void)
//{
//    UINT8 c;
//    
//    while (!RI_1);
//    c = SBUF_1;
//    RI_1 = 0;
//    return (c);
//}
//
//void Send_Data_To_UART1 (UINT8 c)
//{
//    TI_1 = 0;
//    SBUF_1 = c;
//    while(TI_1==0);
//}






/*==========================================================================*/
//#ifdef SW_Reset
//void SW_Reset(void)
//{
//    TA = 0xAA;
//    TA = 0x55;
//    set_SWRST;
//}
//#endif
/*==========================================================================*/
