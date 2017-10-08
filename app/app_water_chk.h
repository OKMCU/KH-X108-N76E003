/**
  ******************************************************************************
  * @file    app_waterchk.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_WATER_CHK_H__
#define __APP_WATER_CHK_H__

/* Includes ------------------------------------------------------------------*/
#include "..\bsp\stdint.h"
#include "firmware_conf.h"

#if APP_WATER_CHK_EN > 0
/* Exported types ------------------------------------------------------------*/
typedef struct {

    uint16_t max;
    uint16_t min;
    uint16_t avg;
    uint16_t iir;

    uint32_t sum;
    uint16_t cnt;
    
} APP_WATERCHK_RESULT_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void                         appWaterChkInit (void);
extern void                         appWaterChkStart(void);
extern void                         appWaterChkStop (void);
extern void                         appWaterChkSrv  (void);
extern const APP_WATERCHK_RESULT_t *appWaterChkGetResult(void);
#endif /* APP_WATER_CHK_EN > 0 */


#endif /* __APP_WATER_CHK_H__ */
/************************ (C) COPYRIGHT OKMCU *****END OF FILE****/







