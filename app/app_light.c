/**
  ******************************************************************************
  * @file    app_light.c
  * @author  SUN Wentao
  * @version V0.0.1
  * @date    08-OCT-2017
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
#include "..\app\app_tasksched.h"
//#include "..\app\app_led.h"
#include "..\app\app_light.h"
//#include "..\hal\hal_pwm.h"
#include "..\hal\hal_light.h"
/** @addtogroup Template_Project
  * @{
  */

#if APP_LIGHT_EN > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COLOR_UPDATE_INTERVAL 5300
#define COLOR_UPDATE_PAUSE    1000
#define COLOR_TABLE_VAR_SIZE  7
//#define COLOR_RES_DIV         3
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static xdata APP_LIGHT_COLOR_t lightColor;
static xdata uint8_t lightColorCnt = 0;
static code APP_LIGHT_COLOR_t colorOff = {0, 0, 0};
static code APP_LIGHT_COLOR_t colorTableSel[COLOR_TABLE_SEL_SIZE] = {
    {0, 255, 0},
    {0, 0, 255},
    {255, 0, 0},
    {255, 255,  0},
    {170, 170, 255},
    {255, 255, 255},
};
static code APP_LIGHT_COLOR_t colorTableVar[COLOR_TABLE_VAR_SIZE] = {
    {255, 0, 0},
    {255, 255, 0},
    {0, 255, 0},
    {0, 255, 255},
    {0, 0, 255},
    {255, 0, 255},
    {255, 255, 255},
};
static code uint16_t colorTableVarTime[COLOR_TABLE_VAR_SIZE] = {
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
    COLOR_UPDATE_PAUSE,
};
static xdata APP_LIGHT_COLOR_t curColor;
static xdata int8_t curColorId = -1;
static xdata int8_t nxtColorId = -1;
static xdata int8_t updColorStartTid = -1;
static xdata int8_t updColorRTid = -1;
static xdata int8_t updColorGTid = -1;
static xdata int8_t updColorBTid = -1;
static xdata uint16_t updColorRInterval = 0;
static xdata uint16_t updColorGInterval = 0;
static xdata uint16_t updColorBInterval = 0;
/* Private function prototypes -----------------------------------------------*/
static uint8_t delta(uint8_t val1, uint8_t val2);
static uint16_t calcUpdColorInterval(uint8_t delta);
static void appLightUpdateColorStart(void);
static void appLightUpdateColorStop(void);
static void appLightUpdateColorR(void);
static void appLightUpdateColorG(void);
static void appLightUpdateColorB(void);
/* Private functions ---------------------------------------------------------*/

static uint8_t delta(uint8_t val1, uint8_t val2)
{
    return val1 > val2 ? (val1-val2) : (val2-val1);
}
static uint16_t calcUpdColorInterval(uint8_t delta)
{
    if(delta == 0)
        return 0;
    return (uint16_t)(COLOR_UPDATE_INTERVAL / (uint16_t)delta);
    //return colorTableVarTime[curColorId];
}

static void appLightUpdateColorStart(void)
{
    updColorStartTid = -1;
    if(updColorRTid < 0 && updColorGTid < 0 && updColorBTid < 0)
    {
        curColorId = nxtColorId;
        nxtColorId++;
        if(nxtColorId >= sizeof(colorTableVar)/sizeof(APP_LIGHT_COLOR_t))
        {
            nxtColorId = 0;
        }
        updColorRInterval = calcUpdColorInterval(delta(curColor.red, colorTableVar[nxtColorId].red));
        updColorGInterval = calcUpdColorInterval(delta(curColor.green, colorTableVar[nxtColorId].green));
        updColorBInterval = calcUpdColorInterval(delta(curColor.blue, colorTableVar[nxtColorId].blue));
        
        updColorRTid = appTaskSchedCreate(updColorRInterval, appLightUpdateColorR);
        updColorGTid = appTaskSchedCreate(updColorGInterval, appLightUpdateColorG);
        updColorBTid = appTaskSchedCreate(updColorBInterval, appLightUpdateColorB);
    }
}

static void appLightUpdateColorStop(void)
{
    appTaskSchedDelete(updColorRTid);
    appTaskSchedDelete(updColorGTid);
    appTaskSchedDelete(updColorBTid);
    appTaskSchedDelete(updColorStartTid);
    updColorStartTid = -1;
    updColorRTid = -1;
    updColorGTid = -1;
    updColorBTid = -1;
    nxtColorId = -1;
    curColorId = -1;
}

static void appLightUpdateColorR(void)
{   
    if(curColor.red == colorTableVar[nxtColorId].red)
    {
        updColorRTid = -1;
        if(updColorRTid < 0 && updColorGTid < 0 && updColorBTid < 0)
        {
            updColorStartTid = appTaskSchedCreate(colorTableVarTime[nxtColorId], appLightUpdateColorStart);
        }
    }
    else
    {
        if(curColor.red > colorTableVar[nxtColorId].red)
        {
            curColor.red--;
        }
        else
        {
            curColor.red++;
        }
        appLightSet(&curColor);
        updColorRTid = appTaskSchedCreate(updColorRInterval, appLightUpdateColorR);
    }
}

static void appLightUpdateColorG(void)
{
    if(curColor.green == colorTableVar[nxtColorId].green)
    {
        updColorGTid = -1;
        if(updColorRTid < 0 && updColorGTid < 0 && updColorBTid < 0)
        {
            updColorStartTid = appTaskSchedCreate(colorTableVarTime[nxtColorId], appLightUpdateColorStart);
        }
    }
    else
    {
        if(curColor.green > colorTableVar[nxtColorId].green)
        {
            curColor.green--;
        }
        else
        {
            curColor.green++;
        }
        appLightSet(&curColor);
        updColorGTid = appTaskSchedCreate(updColorGInterval, appLightUpdateColorG);
    }
}

static void appLightUpdateColorB(void)
{
    if(curColor.blue == colorTableVar[nxtColorId].blue)
    {
        updColorBTid = -1;
        if(updColorRTid < 0 && updColorGTid < 0 && updColorBTid < 0)
        {
            updColorStartTid = appTaskSchedCreate(colorTableVarTime[nxtColorId], appLightUpdateColorStart);
        }
    }
    else
    {
        if(curColor.blue > colorTableVar[nxtColorId].blue)
        {
            curColor.blue--;
        }
        else
        {
            curColor.blue++;
        }
        appLightSet(&curColor);
        updColorBTid = appTaskSchedCreate(updColorBInterval, appLightUpdateColorB);
    }
}
/* Public functions ----------------------------------------------------------*/
void appLightInit(void)
{
    halLightInit();
}

void appLightSet(const APP_LIGHT_COLOR_t *color)
{
    halLightSet(HAL_LIGHT_R, color->red);
    halLightSet(HAL_LIGHT_G, color->green);
    halLightSet(HAL_LIGHT_B, color->blue);
}
void appLightSetMode(uint8_t lightMode)
{
    appLightUpdateColorStop();
    if(lightMode == 0)
    {
        appLightSet(&colorOff);
    }
    else if(lightMode <=  2)
    {
        if(lightMode == 1)
        {
            curColorId = 0;
            nxtColorId = 0;
            curColor.red = colorTableVar[curColorId].red;
            curColor.green = colorTableVar[curColorId].green;
            curColor.blue = colorTableVar[curColorId].blue;
            appLightSet(&curColor);
            updColorStartTid = appTaskSchedCreate(colorTableVarTime[curColorId], appLightUpdateColorStart);
        }
        else//lightMode == 2
        {
            //appLightUpdateColorStop();
        }
    }
    else if(lightMode >= 3 && lightMode <= COLOR_TABLE_SEL_SIZE+2)
    {
        appLightSet(&colorTableSel[lightMode-3]);
    }
}
/*
void appLightSrv(void)
{
    halLightUpdate();
}
*/
/*
void appLightSrv(void)
{
    lightColorCnt++;
    if(lightColorCnt > (UINT8_MAX / COLOR_RES_DIV))
    {
        lightColorCnt = 0;
    }
    
    if(lightColor.red == 0)
    {
        appLedOff(LED_R);
    }
    else if(lightColor.red == (UINT8_MAX / COLOR_RES_DIV))
    {
        appLedOn(LED_R);
    }
    else
    {
        if(lightColorCnt <= lightColor.red)
        {
            appLedOn(LED_R);
        }
        else
        {
            appLedOff(LED_R);
        }
    }

    if(lightColor.green == 0)
    {
        appLedOff(LED_G);
    }
    else if(lightColor.green == (UINT8_MAX / COLOR_RES_DIV))
    {
        appLedOn(LED_G);
    }
    else
    {
        if(lightColorCnt <= lightColor.green)
        {
            appLedOn(LED_G);
        }
        else
        {
            appLedOff(LED_G);
        }
    }

    if(lightColor.blue == 0)
    {
        appLedOff(LED_B);
    }
    else if(lightColor.blue == (UINT8_MAX / COLOR_RES_DIV))
    {
        appLedOn(LED_B);
    }
    else
    {
        if(lightColorCnt <= lightColor.blue)
        {
            appLedOn(LED_B);
        }
        else
        {
            appLedOff(LED_B);
        }
    }
}
*/
#endif /* APP_LIGHT_EN */

/**
  * @}
  */


/************************ (C) COPYRIGHT OKMCU ltd. *****END OF FILE****/