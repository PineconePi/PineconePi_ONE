/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_task.h"
#include "los_swtmr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#define TASK_DEFAULT_PRIO      5
#define TASK_STK_SIZE       1000

#define ONLYONCE_TIMER_VALUE      3000
#define PERIODIC_TIMER_VALUE      1000

#define TASK_LOOP_COUNT  0x7FFFFF

static UINT16 s_usSwTmrID1;
static UINT16 s_usSwTmrID2;

static CHAR *pcTextForTimer1 = "Timer1 is callback";
static CHAR *pcTextForTimer2 = "Timer2 is callback";

static void Timer1_Callback(UINT32 uwArg)
{
    UINT32 tick_last;
    printf("%s\r\n", (const CHAR *)uwArg);

    tick_last = (UINT32)LOS_TickCountGet();
    printf("tick_last1=%lu.\r\n",tick_last);
}

static void Timer2_Callback(UINT32 uwArg)
{
    UINT32 tick_last;
    static UINT32 count;
    printf("%s\r\n", (const CHAR *)uwArg);

    tick_last = (UINT32)LOS_TickCountGet();
    printf("tick_last2=%lu.\r\n",tick_last);

    printf("%d\r\n", count++);
}

UINT32 Example10_Entry(VOID) {
    UINT32 uwRet = LOS_OK;
    
    printf("Example10_Entry\r\n");

    uwRet = LOS_SwtmrCreate(
                ONLYONCE_TIMER_VALUE, 
                LOS_SWTMR_MODE_ONCE,
                Timer1_Callback,
                &s_usSwTmrID1,
                (UINT32)pcTextForTimer1
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == YES)
                , OS_SWTMR_ROUSES_ALLOW,
                OS_SWTMR_ALIGN_SENSITIVE 
#endif
                );
    if(LOS_OK != uwRet)
    {
        printf("create Timer1 failed.\r\n");
        return LOS_NOK;
    }

    uwRet = LOS_SwtmrCreate(
                PERIODIC_TIMER_VALUE, 
                LOS_SWTMR_MODE_PERIOD,
                Timer2_Callback,
                &s_usSwTmrID2,
                (UINT32)pcTextForTimer2
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == YES)
                , OS_SWTMR_ROUSES_ALLOW,
                OS_SWTMR_ALIGN_SENSITIVE 
#endif
                );
    if(LOS_OK != uwRet)
    {
        printf("create Timer1 failed.\r\n");
        return LOS_NOK;
    }

    uwRet = LOS_SwtmrStart(s_usSwTmrID1);
    if(LOS_OK != uwRet)
    {
        printf("Start Timer1 failed.\r\n");
        return LOS_NOK;
    }

    uwRet = LOS_SwtmrStart(s_usSwTmrID2);
    if(LOS_OK != uwRet)
    {
        printf("Start Timer2 failed.\r\n");
        return LOS_NOK;
    }

    return uwRet;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

