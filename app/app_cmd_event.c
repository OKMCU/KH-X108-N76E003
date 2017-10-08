/**
  ******************************************************************************
  * @file    app_cmd_event.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    20-SEP-2017
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
#include "..\bsp\stdint.h"
#include "..\lib\stringx.h"
#include "..\app\app_cmd_event.h"
#include "..\app\app_events.h"

#if APP_CMD_EVENT_EN > 0

extern void appCmd_event(char *optarg)
{
    char *opt = NULL;
    char *arg = NULL;
    char *remain = NULL;
    
    
    opt = strtok_r(optarg, " ", &remain);
    arg = strtok_r(NULL, " ", &remain);
    
    
    if(strcmp(opt, "-buttonpress") == 0)
    {
        appEventButtonPress();
    }
    else
    {
        printf("Unknown option \"%s\".\r\n", opt);
    }
    if(arg != NULL)
    {
        printf("Unknown argument \"%s\".\r\n", arg);
        if(remain != NULL)
        {
            printf("Unknown argument \"%s\".\r\n", remain);
        }
    }
    
}
#endif /* APP_CMD_EVENT_EN */

