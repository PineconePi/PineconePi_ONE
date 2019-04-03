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
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"

#include "mods/pybpin.h"


/// \moduleref pyb
/// \class Pin - control I/O pins
///

#define PIN_DIR_IN          0
#define PIN_DIR_OUT         1
#define PIN_MODE_PUSH_PULL  1
#define PIN_MODE_OPEN_DRAIN 2
#define PIN_MODE_PULL_UP    4
#define PIN_MODE_PULL_DOWN  8

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
pin_obj_t *pin_find_by_name(mp_obj_t name) {
    mp_map_t *named_pins = mp_obj_dict_get_map((mp_obj_t)&pins_locals_dict);

    mp_map_elem_t *named_elem = mp_map_lookup(named_pins, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return (pin_obj_t *)named_elem->value;
    }

    mp_raise_ValueError("invalid argument(s) value");
}

pin_obj_t *pin_find_by_port_bit(GPIO_TypeDef *port, uint pbit) {
    mp_map_t *named_pins = mp_obj_dict_get_map((mp_obj_t)&pins_locals_dict);

    for (uint i = 0; i < named_pins->used; i++) {
        if ((((pin_obj_t *)named_pins->table[i].value)->port == port) &&
            (((pin_obj_t *)named_pins->table[i].value)->pbit == pbit)) {
            return (pin_obj_t *)named_pins->table[i].value;
        }
    }

    mp_raise_ValueError("invalid argument(s) value");
}

void pin_config(pin_obj_t *self, uint alt, uint dir, uint mode) {
    self->alt  = alt;
    self->dir  = dir;
    self->mode = mode;

    uint pull_up    = (mode & PIN_MODE_PULL_UP)    ? 1 : 0;
    uint pull_down  = (mode & PIN_MODE_PULL_DOWN)  ? 1 : 0;
//  uint open_drain = (mode & PIN_MODE_OPEN_DRAIN) ? 1 : 0;

    if(self->alt == 0)
    {
        if((dir != PIN_DIR_IN) && (dir != PIN_DIR_OUT))
        {
            goto invalid_args;
        }

        GPIO_Init(self->port, self->pbit, self->dir, pull_up, pull_down);
    }
    else
    {
        switch ((uint32_t)self->port) {
        case ((uint32_t)GPIOA):
            PORT_Init(PORTA, self->pbit, self->alt, 1);
            break;
        case ((uint32_t)GPIOB):
            PORT_Init(PORTB, self->pbit, self->alt, 1);
            break;
        case ((uint32_t)GPIOC):
            PORT_Init(PORTC, self->pbit, self->alt, 1);
            break;
        default:
            break;
        }
    }

    return;

invalid_args:
    mp_raise_ValueError("invalid argument(s) value");
}

/******************************************************************************
DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************/
// MicroPython bindings

STATIC void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;

    if(self->alt == 0)
    {
        mp_printf(print, "Pin('%q', dir=%s)", self->name, (self->dir == 0) ? "In" : "Out");
    }
    else
    {
        mp_printf(print, "Pin('%q', alf=%q)", self->name, self->alt);
    }
}

STATIC const mp_arg_t pin_init_args[] = {
    { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_dir,                        MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_alt,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_mode,     MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_irq,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = EXTI_FALL_EDGE} },
    { MP_QSTR_callback, MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_priority, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 7} }
};
STATIC mp_obj_t pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pin_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(pin_init_args), pin_init_args, args);

    pin_obj_t *self = pin_find_by_name(args[0].u_obj);

    uint dir = args[1].u_int;

    uint alt = args[2].u_int;

    uint mode = args[3].u_int;

    pin_config(self, alt, dir, mode);

    uint exti_mode = args[4].u_int;
    self->callback = args[5].u_obj;
    if(self->callback != mp_const_none)
    {
        EXTI_Init(self->port, self->pbit, exti_mode);

        NVIC_SetPriority(GPIOA_IRQn, args[6].u_int);
        NVIC_EnableIRQ(GPIOA_IRQn);

        EXTI_Open(self->port, self->pbit);
    }

    return self;
}

STATIC mp_obj_t pin_value(size_t n_args, const mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(GPIO_GetBit(self->port, self->pbit));
    } else {
        // set
        if (mp_obj_is_true(args[1])) {
            GPIO_SetBit(self->port, self->pbit);
        } else {
            GPIO_ClrBit(self->port, self->pbit);
        }

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

STATIC mp_obj_t pin_high(mp_obj_t self_in) {
    pin_obj_t *self = self_in;

    GPIO_SetBit(self->port, self->pbit);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_high_obj, pin_high);

STATIC mp_obj_t pin_low(mp_obj_t self_in) {
    pin_obj_t *self = self_in;

    GPIO_ClrBit(self->port, self->pbit);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_low_obj, pin_low);

STATIC mp_obj_t pin_toggle(mp_obj_t self_in) {
    pin_obj_t *self = self_in;

    GPIO_InvBit(self->port, self->pbit);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_toggle_obj, pin_toggle);

STATIC mp_obj_t pin_irq_enable(mp_obj_t self_in) {
    pin_obj_t *self = self_in;

    EXTI_Open(self->port, self->pbit);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_irq_enable_obj, pin_irq_enable);

STATIC mp_obj_t pin_irq_disable(mp_obj_t self_in) {
    pin_obj_t *self = self_in;

    EXTI_Close(self->port, self->pbit);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_irq_disable_obj, pin_irq_disable);


STATIC void GPIOX_Handler(GPIO_TypeDef *GPIOx, uint n) {
    for(uint i = 0; i < n; i++)
    {
        if(EXTI_State(GPIOx, i))
        {
            EXTI_Clear(GPIOx, i);

            pin_obj_t *self = pin_find_by_port_bit(GPIOx, i);

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

                    char name = 'A';
                    switch ((uint32_t)self->port) {
                    case ((uint32_t)GPIOA):   name = 'A';  break;
                    case ((uint32_t)GPIOB):   name = 'B';  break;
                    case ((uint32_t)GPIOC):   name = 'C';  break;
                    default: break;
                    }
                    printf("uncaught exception in P%c%d interrupt handler\n", name, self->pbit);

                    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
                }
                gc_unlock();
            }
        }

        if(GPIOx->INTSTAT == 0) break;
    }
}

void GPIOA_Handler(void)
{
    GPIOX_Handler(GPIOA, 12);
}

void GPIOB_Handler(void)
{
    GPIOX_Handler(GPIOB, 12);
}

void GPIOC_Handler(void)
{
    GPIOX_Handler(GPIOC, 7);
}


STATIC const mp_rom_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_value),                   MP_ROM_PTR(&pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),                    MP_ROM_PTR(&pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),                     MP_ROM_PTR(&pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),                  MP_ROM_PTR(&pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq_enable),              MP_ROM_PTR(&pin_irq_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq_disable),             MP_ROM_PTR(&pin_irq_disable_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),                      MP_ROM_INT(PIN_DIR_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),                     MP_ROM_INT(PIN_DIR_OUT) },
    { MP_ROM_QSTR(MP_QSTR_PUSH_PULL),               MP_ROM_INT(PIN_MODE_PUSH_PULL) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),              MP_ROM_INT(PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),                 MP_ROM_INT(PIN_MODE_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),               MP_ROM_INT(PIN_MODE_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_FALL_EDGE),               MP_ROM_INT(EXTI_FALL_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_RISE_EDGE),               MP_ROM_INT(EXTI_RISE_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_BOTH_EDGE),               MP_ROM_INT(EXTI_BOTH_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_LOW_LEVEL),               MP_ROM_INT(EXTI_LOW_LEVEL) },
    { MP_ROM_QSTR(MP_QSTR_HIGH_LEVEL),              MP_ROM_INT(EXTI_HIGH_LEVEL) },
};
STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};
