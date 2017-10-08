/**
  ******************************************************************************
  * @file    app_cmd_test.c
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
#include "..\bsp\stdint.h"
#include "..\lib\stringx.h"
#include "..\app\app_cmd_test.h"

#if APP_CMD_TEST_EN > 0
/*
#    test  4321   1357   -a   1234   -b   4567  -c  0987  

[OPT_D] = NA, [ARG_1] = 4321, [ARG_2] = 1357
[OPT_1] = -a, [ARG_1] = 1234
[OPT_2] = -b, [ARG_1] = 4567
[OPT_3] = -c, [ARG_1] = 0987

# test

[OPD] = NULL, [ARG] = NULL
 
*/
extern void appCmd_test(char *optarg)
{
    char *opt = NULL;
    char *arg = NULL;
    char *tmp = NULL;
    char *remain = NULL;
    uint8_t argCnt = 0;
    uint8_t optCnt = 0;
    
    while(optarg == NULL)   
        optarg = NULL;
    
    tmp = strtok_r(optarg, " ", &remain);
    while(tmp != NULL)
    {
        if(*tmp == '-')
        {
            optCnt++;
            argCnt = 0;
            opt = tmp;
            printf("\r\n");
            printf("[OPT_%d] = %s", (uint16_t)optCnt, opt);
        }
        else
        {
            if(optCnt == 0 && argCnt == 0)
            {
                printf("\r\n");
                printf("[OPT_D] = NA");
            }
            arg = tmp;
            argCnt++;
            printf(", [ARG_%d] = %s", (uint16_t)argCnt, arg);
        }
        tmp = strtok_r(NULL, " ", &remain);
    }
    printf("\r\n");
    if(optCnt == 0 && argCnt == 0)
    {
        printf("[OPD] = NULL, [ARG] = NULL\r\n");
    }
}
#endif /* APP_CMD_TEST_EN */

