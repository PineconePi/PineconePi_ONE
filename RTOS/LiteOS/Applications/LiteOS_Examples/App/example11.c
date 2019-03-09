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

#define BACKLIGHT_TIMER_PERIOD      5000

#define TASK_LOOP_COUNT  0x7FFFFF

static UINT32 s_uwKeyHitTskID;
static UINT16 s_usBacklightSwTmrID;
static BOOL s_SimulatedBacklightOn;

static void BacklightTimer_Callback(UINT32 uwArg) {
    UINT32 tick_last = (UINT32)LOS_TickCountGet();
    
    s_SimulatedBacklightOn = FALSE;
    
    printf("Timer expired, turning backlight OFF at time(%d).\r\n",tick_last);
}

static VOID * KeyHit_Task(UINT32 uwArg) {
    UINT32 uwRet = LOS_OK;
    for (;;) {
        printf("Press a key to turn the backlight on.\r\n");
        
        // 如果背景灯关闭，模拟打开背景灯
        if (FALSE == s_SimulatedBacklightOn) {
            s_SimulatedBacklightOn = TRUE;
        }

        // 启动软定时器，5s后触发关闭
        uwRet = LOS_SwtmrStart(s_usBacklightSwTmrID);
        if(LOS_OK != uwRet)
        {
            printf("Start Timer1 failed.\r\n");
            break;
        }
    
        LOS_TaskDelay(10000);            
    }
    
    return LOS_OK;
}

UINT32 Example11_Entry(VOID) {
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S stInitParam = {0};
    
    printf("Example11_Entry\r\n");

    stInitParam.pfnTaskEntry = KeyHit_Task;
    stInitParam.usTaskPrio = TASK_DEFAULT_PRIO;
    stInitParam.pcName = "KeyHit";
    stInitParam.uwStackSize = TASK_STK_SIZE;
    uwRet = LOS_TaskCreate(&s_uwKeyHitTskID, &stInitParam);
    if (uwRet != LOS_OK) {
        printf("KeyHit_Task create Failed!\r\n");
        return LOS_NOK;
    }

    uwRet = LOS_SwtmrCreate(
                BACKLIGHT_TIMER_PERIOD, 
                LOS_SWTMR_MODE_NO_SELFDELETE,
                BacklightTimer_Callback,
                &s_usBacklightSwTmrID,
                NULL
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

    return uwRet;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

