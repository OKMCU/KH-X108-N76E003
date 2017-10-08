/**
  ******************************************************************************
  * @file    app_tasksched.c
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

#include "..\APP\app_tasksched.h"
#include <stdio.h>

#if APP_TASKSCHED_EN > 0
#define     APP_MAX_SCHED_TASK                20 //(<= 128)

typedef struct {

    uint32_t time;
    void (*task)(void);
        
} ScheduledTask_t;
static xdata ScheduledTask_t schedTaskList[APP_MAX_SCHED_TASK] = {0, (void (*)(void))NULL};
static xdata int8_t schedTaskUsedCnt = 0;
static xdata uint32_t timeStamp = 0;

extern void appTaskSchedSystickCb(void)
{
    timeStamp++;
}

extern void appTaskSchedSrv(void)
{
    int8_t i;
    void (*task)(void);

    if(schedTaskUsedCnt > 0)
    {
        for(i = 0; i < APP_MAX_SCHED_TASK; i++)
        {
            if(schedTaskList[i].task != (void (*)(void))NULL)
            {
                if(timeStamp >= schedTaskList[i].time)
                {
                    task = schedTaskList[i].task;
                    appTaskSchedDelete(i);
                    task();
                }
            }
        }
    }
    else
    {
        timeStamp = 0;
    }
}
extern int8_t appTaskSchedCreate(uint32_t offsetTime, void (*task)(void))
{
    int8_t schedTaskId = -1;
    int8_t i;
    
    if(task != NULL)
    {        
        if(schedTaskUsedCnt < APP_MAX_SCHED_TASK)
        {
            for(i = 0; i < APP_MAX_SCHED_TASK; i++)
            {
                if(schedTaskList[i].task == NULL)
                {
                    schedTaskList[i].time = timeStamp+offsetTime;
                    schedTaskList[i].task = task;
                    schedTaskUsedCnt++;
                    schedTaskId = i;
                    break;
                }
            }
        }
    }
    
    return schedTaskId;
}
extern void appTaskSchedDelete(int8_t schedTaskId)
{
    if(schedTaskId < 0 || schedTaskId >= APP_MAX_SCHED_TASK)
        return;
    
    if(schedTaskList[schedTaskId].task != NULL)
    {
        schedTaskList[schedTaskId].task = NULL;
        //schedTaskList[schedTaskId].time = 0;
        schedTaskUsedCnt--;
        if(schedTaskUsedCnt == 0)
        {
            timeStamp = 0;
        }
    }
}

#endif /* APP_TASKSCHED_EN */


