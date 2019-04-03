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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "bufhelper.h"

#include "pybpin.h"
#include "pybpwm.h"

/// \moduleref pyb
/// \class PWM - Pulse-Width Modulation

typedef enum {
    PYB_PWM_0      =  0,
    PYB_PWM_1      =  1,
    PYB_PWM_2      =  2,
    PYB_PWM_3      =  3,
    PYB_PWM_4      =  4,
    PYB_PWM_5      =  5,
    PYB_NUM_PWMS
} pyb_pwm_id_t;

typedef struct _pyb_pwm_obj_t {
    mp_obj_base_t base;
    pyb_pwm_id_t pwm_id;
    PWM_TypeDef *PWMx;
    uint period;
    uint duty;
    uint mode;
    uint deadzone;
} pyb_pwm_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_pwm_obj_t pyb_pwm_obj[PYB_NUM_PWMS] = { { .pwm_id = PYB_PWM_0, .PWMx = PWM0 },
                                                   { .pwm_id = PYB_PWM_1, .PWMx = PWM1 },
                                                   { .pwm_id = PYB_PWM_2, .PWMx = PWM2 },
                                                   { .pwm_id = PYB_PWM_3, .PWMx = PWM3 },
                                                   { .pwm_id = PYB_PWM_4, .PWMx = PWM4 },
                                                   { .pwm_id = PYB_PWM_5, .PWMx = PWM5 } };

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/


/******************************************************************************/
/* MicroPython bindings                                                      */
/******************************************************************************/
STATIC void pyb_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pwm_obj_t *self = self_in;

    if(self->mode == PWM_MODE_INDEP)
    {
        mp_printf(print, "PWM(%d, period=%u, duty=\%%d)", self->pwm_id, self->period, self->duty);
    }
    else
    {
        mp_printf(print, "PWM(%d, period=%u, duty=%d, deadzone=%d)", self->pwm_id, self->period, self->duty, self->deadzone);
    }
}

STATIC const mp_arg_t pyb_pwm_init_args[] = {
    { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_period,   MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 20000} },
    { MP_QSTR_duty,     MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 10000} },
    { MP_QSTR_mode,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = PWM_MODE_INDEP} },
    { MP_QSTR_pin,      MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_pinA,     MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_pinB,     MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_deadzone, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
};
STATIC mp_obj_t pyb_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_pwm_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_pwm_init_args, args);

    uint pwm_id = args[0].u_int;
    if (pwm_id > PYB_NUM_PWMS-1) {
        mp_raise_OSError(MP_ENODEV);
    }

    pyb_pwm_obj_t *self = &pyb_pwm_obj[pwm_id];
    self->base.type = &pyb_pwm_type;

    self->period = args[1].u_int;
    self->duty   = args[2].u_int;
    if((self->period > 65535) || (self->duty > self->period))
    {
        goto invalid_args;
    }

    self->mode = args[3].u_int;
    if((self->mode != PWM_MODE_INDEP) && (self->mode != PWM_MODE_COMPL))
    {
        goto invalid_args;
    }

    if(self->mode == PWM_MODE_INDEP)
    {
        if(args[4].u_obj == mp_const_none)
        {
            goto invalid_args;
        }
        pin_obj_t *pin = pin_find_by_name(args[4].u_obj);
        if((self->pwm_id == PYB_PWM_0) || (self->pwm_id == PYB_PWM_2) || (self->pwm_id == PYB_PWM_4))
        {
            if((pin->pbit % 2) == 1) mp_raise_ValueError("PWM0/2/4 pin need be Even number pin, like PA0, PA2, PA4");
        }
         else
        {
            if((pin->pbit % 2) == 0) mp_raise_ValueError("PWM1/3/5 pin need be Odd  number pin, like PA1, PA3, PA5");
        }

        switch (self->pwm_id) {
        case PYB_PWM_0:
            pin_config(pin, FUNMUX0_PWM0A_OUT, 0, 0);
            break;

        case PYB_PWM_1:
            pin_config(pin, FUNMUX1_PWM1A_OUT, 0, 0);
            break;

        case PYB_PWM_2:
            pin_config(pin, FUNMUX0_PWM2A_OUT, 0, 0);
            break;

        case PYB_PWM_3:
            pin_config(pin, FUNMUX1_PWM3A_OUT, 0, 0);
            break;

        case PYB_PWM_4:
            pin_config(pin, FUNMUX0_PWM4A_OUT, 0, 0);
            break;

        case PYB_PWM_5:
            pin_config(pin, FUNMUX1_PWM5A_OUT, 0, 0);
            break;

        default:
            break;
        }
    }
    else
    {
        if((args[5].u_obj == mp_const_none) || (args[6].u_obj == mp_const_none))
        {
            goto invalid_args;
        }
        pin_obj_t *pinA = pin_find_by_name(args[5].u_obj);
        pin_obj_t *pinB = pin_find_by_name(args[6].u_obj);
        if((self->pwm_id == PYB_PWM_0) || (self->pwm_id == PYB_PWM_2) || (self->pwm_id == PYB_PWM_4))
        {
            if(((pinA->pbit % 2) == 1) || ((pinB->pbit % 2) == 1))
                mp_raise_ValueError("PWM0/2/4 pin need be Even number pin, like PA0, PA2, PA4");
        }
         else
        {
            if(((pinA->pbit % 2) == 0) || ((pinB->pbit % 2) == 0))
                mp_raise_ValueError("PWM1/3/5 pin need be Odd  number pin, like PA1, PA3, PA5");
        }

        switch (self->pwm_id) {
        case PYB_PWM_0:
            pin_config(pinA, FUNMUX0_PWM0A_OUT, 0, 0);
            pin_config(pinB, FUNMUX0_PWM0B_OUT, 0, 0);
            break;

        case PYB_PWM_1:
            pin_config(pinA, FUNMUX1_PWM1A_OUT, 0, 0);
            pin_config(pinB, FUNMUX1_PWM1B_OUT, 0, 0);
            break;

        case PYB_PWM_2:
            pin_config(pinA, FUNMUX0_PWM2A_OUT, 0, 0);
            pin_config(pinB, FUNMUX0_PWM2B_OUT, 0, 0);
            break;

        case PYB_PWM_3:
            pin_config(pinA, FUNMUX1_PWM3A_OUT, 0, 0);
            pin_config(pinB, FUNMUX1_PWM3B_OUT, 0, 0);
            break;

        case PYB_PWM_4:
            pin_config(pinA, FUNMUX0_PWM4A_OUT, 0, 0);
            pin_config(pinB, FUNMUX0_PWM4B_OUT, 0, 0);
            break;

        case PYB_PWM_5:
            pin_config(pinA, FUNMUX1_PWM5A_OUT, 0, 0);
            pin_config(pinB, FUNMUX1_PWM5B_OUT, 0, 0);
            break;

        default:
            break;
        }
    }

    self->deadzone = args[7].u_int;

    PWM_InitStructure  PWM_initStruct;

    PWM_initStruct.clk_div = PWM_CLKDIV_8;		//F_PWM = 120M/8 = 15M

    PWM_initStruct.mode = self->mode;
    PWM_initStruct.cycleA = self->period;
    PWM_initStruct.hdutyA = self->duty;
    PWM_initStruct.deadzoneA  = self->deadzone;
    PWM_initStruct.initLevelA = 0;
    PWM_initStruct.cycleB     = 0;
    PWM_initStruct.hdutyB     = 0;
    PWM_initStruct.deadzoneB  = self->deadzone;
    PWM_initStruct.initLevelB = 0;
    PWM_initStruct.HEndAIEn = 0;
    PWM_initStruct.NCycleAIEn = 0;
    PWM_initStruct.HEndBIEn = 0;
    PWM_initStruct.NCycleBIEn = 0;

    PWM_Init(self->PWMx, &PWM_initStruct);

    return self;

invalid_args:
    mp_raise_ValueError("invalid argument(s) value");
}

STATIC mp_obj_t pyb_pwm_start(mp_obj_t self_in) {
    pyb_pwm_obj_t *self = self_in;

    if(self->mode == PWM_MODE_INDEP)
    {
        PWM_Start(self->PWMx, 1, 0);
    }
    else
    {
        PWM_Start(self->PWMx, 1, 1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pwm_start_obj, pyb_pwm_start);

STATIC mp_obj_t pyb_pwm_stop(mp_obj_t self_in) {
    pyb_pwm_obj_t *self = self_in;

    if(self->mode == PWM_MODE_INDEP)
    {
        PWM_Stop(self->PWMx, 1, 0);
    }
    else
    {
        PWM_Stop(self->PWMx, 1, 1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pwm_stop_obj, pyb_pwm_stop);

STATIC mp_obj_t pyb_pwm_period(size_t n_args, const mp_obj_t *args) {
    pyb_pwm_obj_t *self = args[0];
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->period);
    } else {
        self->period = mp_obj_get_int(args[1]);
        PWM_SetCycle(self->PWMx, PWM_CH_A, self->period);

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_period_obj, 1, 2, pyb_pwm_period);

STATIC mp_obj_t pyb_pwm_duty(size_t n_args, const mp_obj_t *args) {
    pyb_pwm_obj_t *self = args[0];
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        self->duty = mp_obj_get_int(args[1]);
        PWM_SetHDuty(self->PWMx, PWM_CH_A, self->duty);

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_duty_obj, 1, 2, pyb_pwm_duty);

STATIC mp_obj_t pyb_pwm_deadzone(size_t n_args, const mp_obj_t *args) {
    pyb_pwm_obj_t *self = args[0];
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->deadzone);
    } else {
        self->deadzone = mp_obj_get_int(args[1]);
        PWM_SetDeadzone(self->PWMx, PWM_CH_A, self->deadzone);

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_deadzone_obj, 1, 2, pyb_pwm_deadzone);


STATIC const mp_rom_map_elem_t pyb_pwm_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_start),               MP_ROM_PTR(&pyb_pwm_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),                MP_ROM_PTR(&pyb_pwm_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_period),              MP_ROM_PTR(&pyb_pwm_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty),                MP_ROM_PTR(&pyb_pwm_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_deadzone),            MP_ROM_PTR(&pyb_pwm_deadzone_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_MODE_INDEP),          MP_ROM_INT(PWM_MODE_INDEP) },
    { MP_ROM_QSTR(MP_QSTR_MODE_COMPL),          MP_ROM_INT(PWM_MODE_COMPL) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pwm_locals_dict, pyb_pwm_locals_dict_table);

const mp_obj_type_t pyb_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pyb_pwm_print,
    .make_new = pyb_pwm_make_new,
    .locals_dict = (mp_obj_t)&pyb_pwm_locals_dict,
};
