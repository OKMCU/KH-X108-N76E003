/**
  ******************************************************************************
  * @file    stringx.c
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
#include "..\bsp\typedef.h"
#include "..\LIB\stringx.h"

#if LIB_STRINGX_EN > 0
char *strtok_r(char *s, const char *delim, char **save_ptr) 
{     

    char *token;     
     
    if (s == NULL) s = *save_ptr;     
     
    /* Scan leading delimiters.  */     
    s += strspn(s, delim);     
    if (*s == '\0')      
        return NULL;     
     
    /* Find the end of the token.  */     
    token = s;     
    s = strpbrk(token, delim);     
    if (s == NULL)     
        /* This token finishes the string.  */     
        *save_ptr = strchr(token, '\0');     
    else {     
        /* Terminate the token and make *SAVE_PTR point past it.  */     
        *s = '\0';     
        *save_ptr = s + 1;     
    }     
     
    return token;     
}    
#endif /* LIB_STRINGX_EN */

#if LIB_NUM_STRING_EN > 0
extern uint8_t hex2uint(char *hexStr, uint32_t *num)
{
    char ch;
    char ch1;
    uint32_t val = 0;
    int32_t test_09;
    int32_t test_AF;
    int32_t test_af;
    
    /*
    ch = *hexStr++;
    if(ch != '0')
        return FALSE;
    ch = *hexStr++;
    if(ch != 'x' && ch != 'X')
        return FALSE;
    */
    
    ch = hexStr[0];
    ch1 = hexStr[1];
    if(ch == '0')
    {
        if(ch1 == 'x' || ch1 == 'X')
        {
            hexStr += 2;
        }
    }
    //////////////////////
    
    ch = *hexStr++;
    if(ch == '\0')
        return FALSE;
    
    while(ch)
    {
        test_09 = (ch - '0') * (ch - '9');
        test_AF = (ch - 'A') * (ch - 'F');
        test_af = (ch - 'a') * (ch - 'f');
        if(test_09 * test_AF * test_af <= 0)
        {
            val <<= 4;
            if(test_09 <= 0)
            {
                val += ch - '0';
            }
            else if(test_AF <= 0)
            {
                val += ch - 'A' + 10;
            }
            else if(test_af <= 0)
            {
                val += ch - 'a' + 10;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
        ch = *hexStr++;
        
    }
        
    *num = val;
    return TRUE;
}


/*
extern uint8_t hex2int(char *hexStr, int32_t *num)
{
    char ch;
    int32_t val = 0;
    int32_t test_09;
    int32_t test_AF;
    int32_t test_af;
    
    
    ch = *hexStr++;
    if(ch != '0')
        return FALSE;
    ch = *hexStr++;
    if(ch != 'x' && ch != 'X')
        return FALSE;
    
    ch = *hexStr++;
    if(ch == '\0')
        return FALSE;
    
    while(ch)
    {
        test_09 = (ch - '0') * (ch - '9');
        test_AF = (ch - 'A') * (ch - 'F');
        test_af = (ch - 'a') * (ch - 'f');
        if(test_09 * test_AF * test_af <= 0)
        {
            val <<= 4;
            if(test_09 <= 0)
            {
                val += ch - '0';
            }
            else if(test_AF <= 0)
            {
                val += ch - 'A' + 10;
            }
            else if(test_af < 0)
            {
                val += ch - 'a' + 10;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
        ch = *hexStr++;
        
    }
        
    *num = val;
    return TRUE;
}
*/
extern uint8_t hex2int(char *hexStr, int32_t *num)
{
    uint8_t err;
    uint32_t val;
    err = hex2uint(hexStr, &val);
    *num = (int32_t)val;
    return err;
}


extern uint8_t dec2int(char *decStr, int32_t *num)
{
    char ch;
    int32_t val = 0;
    int32_t pnFlg = 0;
    
    if(decStr == NULL)
        return FALSE;

    
    
    ch = *decStr++;
    if(ch == '\0')
        return FALSE;
    if(ch == '-')
    {
        pnFlg = -1;
        ch = *decStr++;
    }
    else
    {
        pnFlg = 1;
    }
    
    while(ch > 0)
    {
        if(ch >= '0' && ch <= '9')
        {
            val *= 10;
            val += ch - '0';
        }
        else
        {
            return FALSE;
        }
        ch = *decStr++;
    }
    
    
    *num = val*pnFlg;
    return TRUE;
}

#endif /* LIB_NUM_STRING_EN */
