/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "pybtimer.h"
#include "pybpin.h"


/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef enum {
    PYB_TIMER_0      =  0,
    PYB_TIMER_1      =  1,
    PYB_TIMER_2      =  2,
    PYB_TIMER_3      =  3,
    PYB_TIMER_4      =  4,
    PYB_TIMER_5      =  5,
    PYB_NUM_TIMERS
} pyb_timer_id_t;

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    pyb_timer_id_t timer_id;
    TIMR_TypeDef *TIMRx;
    uint32_t period;
    uint32_t remain;        // period remain
    uint8_t  mode;
    uint8_t  priority;      // 中断优先级
    mp_obj_t callback;      // 中断处理函数
} pyb_timer_obj_t;

/******************************************************************************
 DEFINE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_timer_obj_t pyb_timer_obj[PYB_NUM_TIMERS] = {{.timer_id = PYB_TIMER_0, .TIMRx = TIMR0},
                                                        {.timer_id = PYB_TIMER_1, .TIMRx = TIMR1},
                                                        {.timer_id = PYB_TIMER_2, .TIMRx = TIMR2},
                                                        {.timer_id = PYB_TIMER_3, .TIMRx = TIMR3},
                                                        {.timer_id = PYB_TIMER_4, .TIMRx = TIMR4},
                                                        {.timer_id = PYB_TIMER_5, .TIMRx = TIMR5}};

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************/
/* MicroPython bindings                                                      */

STATIC void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;

    if(self->mode == TIMR_MODE_TIMER)
    {
        uint32_t _0_1ms = self->period / 12000;     // 周期时间长度，单位0.1ms

        mp_printf(print, "Timer(%d, period=", self->timer_id);
        if(_0_1ms < 10000)
            mp_printf(print, "%d.%dms)", (_0_1ms%10000)/10, _0_1ms%10);
        else
            mp_printf(print, "%ds %d.%dms)", _0_1ms/10000, (_0_1ms%10000)/10, _0_1ms%10);
    }
    else
    {
        mp_printf(print, "Counter(%d, period=%d)", self->timer_id, self->period);
    }
}

STATIC const mp_arg_t pyb_timer_init_args[] = {
    { MP_QSTR_id,         MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_period,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 120000} },
    { MP_QSTR_callback,   MP_ARG_REQUIRED |
                          MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_mode,       MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = TIMR_MODE_TIMER} },
    { MP_QSTR_pin,        MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_priority,   MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 7} }
};
STATIC mp_obj_t pyb_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_timer_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_timer_init_args, args);

    uint timer_id = args[0].u_int;
    if(timer_id > PYB_NUM_TIMERS-1)
    {
        mp_raise_OSError(MP_ENODEV);
    }

    pyb_timer_obj_t *self = &pyb_timer_obj[timer_id];
    self->base.type = &pyb_timer_type;

    self->period = args[1].u_int;

    self->callback = args[2].u_obj;

    self->mode = args[3].u_int;
    if((self->mode != TIMR_MODE_TIMER) && (self->mode != TIMR_MODE_COUNTER))
    {
        goto invalid_args;
    }

    if(self->mode == TIMR_MODE_TIMER)
    {
        self->period *= 12000;          // 定时器模式：单位为0.1ms
    }
    else //          TIMR_MODE_COUNTER
    {
        if(args[4].u_obj == mp_const_none) goto invalid_args;
        pin_obj_t * pin = pin_find_by_name(args[4].u_obj);

        switch (self->timer_id) {
        case PYB_TIMER_0:
            if((pin->pbit % 2) == 1) mp_raise_ValueError("Timer0 Counter IN need be Even number pin, like PA0, PA2, PA4");
            pin_config(pin, FUNMUX0_TIMR0_IN, 0, 0);
            break;

        case PYB_TIMER_1:
            if((pin->pbit % 2) == 0) mp_raise_ValueError("Timer1 Counter IN need be Odd  number pin, like PA1, PA3, PA5");
            pin_config(pin, FUNMUX1_TIMR1_IN, 0, 0);
            break;

        case PYB_TIMER_2:
            if((pin->pbit % 2) == 1) mp_raise_ValueError("Timer2 Counter IN need be Even number pin, like PA0, PA2, PA4");
            pin_config(pin, FUNMUX0_TIMR2_IN, 0, 0);
            break;

        case PYB_TIMER_3:
            if((pin->pbit % 2) == 0) mp_raise_ValueError("Timer3 Counter IN need be Odd number pin, like PA1, PA3, PA5");
            pin_config(pin, FUNMUX1_TIMR3_IN, 0, 0);
            break;

        default:
            break;
        }
    }

    self->priority = args[5].u_int;
    if(self->priority > 7)
    {
        goto invalid_args;
    }
    switch (self->timer_id) {
    case PYB_TIMER_0:
        NVIC_SetPriority(TIMR0_IRQn, self->priority);
        break;

    case PYB_TIMER_1:
        NVIC_SetPriority(TIMR1_IRQn, self->priority);
        break;

    case PYB_TIMER_2:
        NVIC_SetPriority(TIMR2_IRQn, self->priority);
        break;

    case PYB_TIMER_3:
        NVIC_SetPriority(TIMR3_IRQn, self->priority);
        break;

    default:
        break;
    }

    if(self->period > 0xFFFFFF)
    {
        self->remain = self->period - 0xFFFFFF;
        TIMR_Init(self->TIMRx, self->mode, 0xFFFFFF, 1);
    }
    else
    {
        self->remain = 0;
        TIMR_Init(self->TIMRx, self->mode, self->period, 1);
    }

    return self;

invalid_args:
    mp_raise_ValueError("invalid argument(s) value");
}

STATIC mp_obj_t pyb_timer_period(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {  // get
        if(self->mode == TIMR_MODE_TIMER)
            return mp_obj_new_int(self->period/12000);              // 定时器模式：单位为0.1ms
        else
            return mp_obj_new_int(self->period);                    // 计数器模式：脉冲个数
    } else {            // set
        self->period = mp_obj_get_int(args[1]);
        if(self->mode == TIMR_MODE_TIMER) self->period *= 12000;    // 定时器模式：单位为0.1ms

        if(self->period > 0xFFFFFF)
        {
            self->remain = self->period - 0xFFFFFF;
            TIMR_SetPeriod(self->TIMRx, 0xFFFFFF);
        }
        else
        {
            self->remain = 0;
            TIMR_SetPeriod(self->TIMRx, self->period);
        }

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_period_obj, 1, 2, pyb_timer_period);

STATIC mp_obj_t pyb_timer_start(mp_obj_t self_in) {
    pyb_timer_obj_t *self = self_in;

    TIMR_Start(self->TIMRx);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_start_obj, pyb_timer_start);

STATIC mp_obj_t pyb_timer_stop(mp_obj_t self_in) {
    pyb_timer_obj_t *self = self_in;

    TIMR_Stop(self->TIMRx);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_stop_obj, pyb_timer_stop);


void TIMR_Handler(pyb_timer_obj_t *self) {
    TIMR_INTClr(self->TIMRx);

    if(self->remain < 12000)    // 12000/120000000 = 0.1ms, 12000/0xFFFFFF ≈ 0.07%
    {
        /* 使period值立即生效 */
        TIMR_Stop(self->TIMRx);
        if(self->period > 0xFFFFFF)
        {
            self->remain = self->period - 0xFFFFFF;
            TIMR_SetPeriod(self->TIMRx, 0xFFFFFF);
        }
        else
        {
            self->remain = 0;
            TIMR_SetPeriod(self->TIMRx, self->period);
        }
        TIMR_Start(self->TIMRx);

        /* 执行中断回调 */
        if(self->callback != mp_const_none)
        {
            gc_lock();
            nlr_buf_t nlr;
            if(nlr_push(&nlr) == 0)
            {
                mp_call_function_1(self->callback, self);
                nlr_pop();
            }
            else
            {
                self->callback = mp_const_none;

                printf("uncaught exception in Timer(%u) interrupt handler\n", self->timer_id);
                mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
            }
            gc_unlock();
        }
    }
    else
    {
        if(self->remain > 0xFFFFFF)
        {
            self->remain -= 0xFFFFFF;
        }
        else
        {
            /* 使period值立即生效 */
            TIMR_Stop(self->TIMRx);
            TIMR_SetPeriod(self->TIMRx, self->remain);
            TIMR_Start(self->TIMRx);

            self->remain = 0;
        }
    }
}

void TIMR0_Handler(void)
{
    TIMR_Handler(&pyb_timer_obj[PYB_TIMER_0]);
}

void TIMR1_Handler(void)
{
    TIMR_Handler(&pyb_timer_obj[PYB_TIMER_1]);
}

void TIMR2_Handler(void)
{
    TIMR_Handler(&pyb_timer_obj[PYB_TIMER_2]);
}

void TIMR3_Handler(void)
{
    TIMR_Handler(&pyb_timer_obj[PYB_TIMER_3]);
}


STATIC const mp_rom_map_elem_t pyb_timer_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_period),                  MP_ROM_PTR(&pyb_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),                   MP_ROM_PTR(&pyb_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),                    MP_ROM_PTR(&pyb_timer_stop_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_MODE_TIMER),              MP_ROM_INT(TIMR_MODE_TIMER) },
    { MP_ROM_QSTR(MP_QSTR_MODE_COUNTER),            MP_ROM_INT(TIMR_MODE_COUNTER) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);


const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_t)&pyb_timer_locals_dict,
};
