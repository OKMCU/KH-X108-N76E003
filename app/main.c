/**
  ******************************************************************************
  * @file    main.c
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
#include "..\bsp\bsp.h"
#include "..\app\app_cmd.h"
#include "..\app\app_tasksched.h"
#include "..\app\app_button.h"
#include "..\app\app_led.h"
#include "..\app\app_spray.h"
#include "..\app\app_water_chk.h"
#include "..\app\app_light.h"
#include "..\app\app_buzzer.h"
#include "..\app\app_events.h"
#include "..\hal\hal_flash.h"
#include "firmware_conf.h"

void main (void)
{
    bspInit();
#if APP_LED_EN > 0
    appLedInit();
#endif /* APP_LED_EN > 0 */
#if APP_LIGHT_EN > 0
    appLightInit();
#endif /* APP_LIGHT_EN > 0 */
#if APP_BUTTON_EN > 0
    appButtonInit();
#endif /* APP_BUTTON_EN > 0 */
#if APP_SPRAY_EN > 0
    appSprayInit();
#endif /* APP_SPRAY_EN > 0 */
#if APP_WATER_CHK_EN > 0
    appWaterChkInit();
#endif /* APP_WATER_CHK_EN > 0 */

#if APP_BUZZER_EN > 0
    appBuzzerInit();
#endif /* APP_BUZZER_EN > 0 */

    appEventPowerOnReset();

#if BSP_UART_EN > 0
    printf("System Started!\r\n");
    printf("# ");
#endif /* BSP_UART_EN > 0 */
    
    while(1)
    {
#if APP_CMD_EN > 0
    	appCmdSrv();
#endif /* APP_CMD_EN > 0 */
#if APP_TASKSCHED_EN > 0
        appTaskSchedSrv();
#endif /* APP_TASKSCHED_EN > 0 */
#if APP_WATER_CHK_EN > 0
        appWaterChkSrv();
#endif /* APP_WATER_CHK_EN > 0 */
    }
}

