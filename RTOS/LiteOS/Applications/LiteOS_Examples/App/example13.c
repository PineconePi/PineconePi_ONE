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
#include "los_sem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static UINT32 s_uwHandleTskID;

static UINT32 s_uwSemID;

#define TASK_DEFAULT_PRIO      5
#define TASK_STK_SIZE       1000
#define NUM_OF_TASKS          10

#define RES_COUNT              3

#define TASK_LOOP_COUNT     0x7FFFFF

typedef struct {
    UINT32 handleID;
    BOOL isUsed;
} ResHandler;

static ResHandler s_stResHandler[RES_COUNT] = {
            { 100, FALSE },
            { 200, FALSE },
            { 300, FALSE }
        };

static VOID * Handle_Task(UINT32 uwArg) {
    UINT32 uwRet = LOS_OK;
    UINT32 i = NULL;
    
    printf("Handle_Task(%d) should be Pending.\r\n", uwArg);
    uwRet = LOS_SemPend(s_uwSemID, LOS_WAIT_FOREVER);
    if (LOS_OK == uwRet) {        
        for (i = 0; i < RES_COUNT; i++) {
            if (s_stResHandler[i].isUsed == FALSE) {
                s_stResHandler[i].isUsed = TRUE;
                break;
            }
        }
        printf("I am working on it(%d)\r\n", s_stResHandler[i].handleID);
        LOS_TaskDelay(1000);        

        // 1s后处理完成，释放信号量
        s_stResHandler[i].isUsed = FALSE;
        LOS_SemPost(s_uwSemID);
    }
    
    printf("Handle_Task(%d) should be finish.\r\n", uwArg);
    return LOS_OK;
}

UINT32 Example13_Entry(VOID) {
    UINT32 uwRet = LOS_OK;
    UINT32 i = NULL;
    TSK_INIT_PARAM_S stInitParam = {0};
    
    puts("Example13_Entry\r\n");

    uwRet = LOS_SemCreate(RES_COUNT, &s_uwSemID);
    if (uwRet != LOS_OK) {
        printf("LOS_SemCreate Failed:%x!\r\n", uwRet);
        return LOS_NOK;
    }
    
    for (i = 0; i < NUM_OF_TASKS; i++) {
        stInitParam.pfnTaskEntry = Handle_Task;
        stInitParam.usTaskPrio = TASK_DEFAULT_PRIO;
        stInitParam.pcName = "Tasks";
        stInitParam.uwStackSize = TASK_STK_SIZE;
        stInitParam.uwArg = i;
        uwRet = LOS_TaskCreate(&s_uwHandleTskID, &stInitParam);
        if (uwRet != LOS_OK) {
            printf("Handle_Task create Failed!\r\n");
            return LOS_NOK;
        }
    }
    return uwRet;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

