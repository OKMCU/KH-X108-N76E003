/**
  ******************************************************************************
  * @file    app_cmd.c
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

#include <stdio.h>
#include <string.h>
#include "..\lib\stringx.h"
#include "..\bsp\typedef.h"
#include "..\bsp\stdint.h"
#include "..\hal\hal_console.h"
#include "..\app\app_cmd.h"
#include "..\app\app_cmd_test.h"
#include "..\app\app_cmd_spray.h"
#include "..\app\app_cmd_waterchk.h"
#include "..\app\app_cmd_clko.h" 
#include "..\app\app_cmd_event.h"

#if APP_CMD_EN > 0


#define     CMD_MAX_LENGTH              64
static xdata char cmdBuffer[CMD_MAX_LENGTH];
static xdata uint8_t cmdLength = 0;



typedef struct {

    char *cmdStr;
    void (*cmdFxn)(char *optarg);
    
} Command_t;



static code Command_t cmdList[] = {
#if APP_CMD_TEST_EN > 0
    {"test",        appCmd_test},
#endif /* APP_CMD_TEST_EN */

#if APP_CMD_PWM_EN > 0
    {"pwm",       appCmd_pwm},
#endif /* APP_CMD_PWM_EN */

#if APP_CMD_SPRAY_EN > 0
    {"spray",       appCmd_spray},
#endif /* APP_CMD_SPRAY_EN */

#if APP_CMD_WATERCHK_EN > 0
    {"waterchk",    appCmd_waterchk},
#endif /* APP_CMD_WATERCHK_EN */

#if APP_CMD_CLKO_EN > 0
    {"clko",        appCmd_clko},
#endif /* APP_CMD_CLKO_EN */

#if APP_CMD_EVENT_EN > 0
    {"event",       appCmd_event},
#endif
};

void appCmdExecute(char *cmdStr)
{
    void (*cmdFxn)(char *optarg);
    uint16_t i;
    char * cmd = NULL;
    char * optarg = NULL;
	
    cmd = strtok_r(cmdStr, " ", &optarg);
    
    for (i = 0; i < sizeof(cmdList)/sizeof(Command_t); i++)
    {
        if(strcmp(cmd, cmdList[i].cmdStr) == 0)
        {
            cmdFxn = cmdList[i].cmdFxn;
            if(cmdFxn != NULL)
            {
                cmdFxn(optarg);
            }
            break;
        }
    }
    
    if(i == sizeof(cmdList)/sizeof(Command_t))
    {
        printf("Unknown command \"%s\"\r\n", cmd);
    }
}



extern void appCmdSrv(void)
{
    char rxByte = 0;
    
    while(halConsoleCharAvail())
    {
        rxByte = halConsoleCharGet();
        
        if(rxByte >= 32 && rxByte <= 127)
        {
            if(rxByte == 127)
            {
                if(cmdLength > 0)
                {
                    cmdLength--;
                    cmdBuffer[cmdLength] = '\0';
                    halConsoleCharPut(rxByte);
                }
            }
            else
            {
                if(cmdLength < CMD_MAX_LENGTH - 1)
                {
                    cmdBuffer[cmdLength++] = rxByte;
                    halConsoleCharPut(rxByte);
                }
                cmdBuffer[cmdLength] = '\0';
            }
        }
        else
        {
            switch((char)rxByte)
            {
                case '\r':
                    printf("\r\n");
                    if(cmdLength > 0)
                    {
                        appCmdExecute(cmdBuffer);
                        cmdLength = 0;
                    }
                    printf("# ");
                break;
                
                default:
                break;
            }
        }
    }
}


#endif /* APP_CMD_EN */

