/**
  ******************************************************************************
  * @file    app_waterchk.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    29-SEP-2017
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


/* Includes ------------------------------------------------------------------*/

#include "..\bsp\stdint.h"
#include "..\bsp\typedef.h"
#include "..\hal\hal_water_chk.h"
#include "..\app\app_water_chk.h"

#include <stdio.h>

/** @addtogroup Template_Project
  * @{
  */

#if APP_WATER_CHK_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool start_flag = FALSE;
static xdata APP_WATERCHK_RESULT_t appWaterChkResult;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void appWaterChkClrResult(void)
{
    appWaterChkResult.avg = 0;
    appWaterChkResult.max = 0;
    appWaterChkResult.min = 0xFFFF;
    appWaterChkResult.sum = 0;
    appWaterChkResult.iir = 0;
    appWaterChkResult.cnt = 0;
}

/* Public functions ----------------------------------------------------------*/
extern void appWaterChkInit (void)
{
    halWaterChkInit();
    appWaterChkClrResult();
}
extern void appWaterChkStart(void)
{
    start_flag = TRUE;
    
    halWaterChkStart();
    appWaterChkClrResult();
}
extern void appWaterChkStop (void)
{
    appWaterChkResult.avg = appWaterChkResult.sum / appWaterChkResult.cnt;
    start_flag = FALSE;
}
extern void appWaterChkSrv  (void)
{
    uint16_t result;
    if(start_flag)
    {
        if(halWaterChkResultReady())
        {
            result = halWaterChkGetResult();
            halWaterChkStart();
            
            appWaterChkResult.cnt++;
            appWaterChkResult.sum += result;
            if(result > appWaterChkResult.max)
                appWaterChkResult.max = result;
            if(result < appWaterChkResult.min)
                appWaterChkResult.min = result;

            if(appWaterChkResult.iir == 0)
            {
                appWaterChkResult.iir = result;
            }
            else
            {
                if(result >= appWaterChkResult.iir)
                    appWaterChkResult.iir = (appWaterChkResult.iir + result * 3) >> 2;
                if(result < appWaterChkResult.iir)
                    appWaterChkResult.iir = (appWaterChkResult.iir * 3 + result) >> 2;
            }
        }
    }
}
extern const APP_WATERCHK_RESULT_t *appWaterChkGetResult(void)
{
    return start_flag ? NULL : &appWaterChkResult;
}

/**
  * @}
  */
#endif /* APP_WATER_CHK_EN */


/************************ (C) COPYRIGHT OKMCU Ltd. *****END OF FILE****/

