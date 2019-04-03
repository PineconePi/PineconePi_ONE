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
#include "py/binary.h"
#include "py/gc.h"
#include "py/mperrno.h"

#include "bufhelper.h"

#include "pybadc.h"


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYB_ADC_NUM_CHANNELS    8

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef enum {
    PYB_ADC_0      =  0,
    PYB_ADC_1      =  1,
    PYB_NUM_ADCS
} pyb_adc_id_t;

typedef struct {
    mp_obj_base_t base;
    pyb_adc_id_t adc_id;
    ADC_TypeDef *ADCx;
    uint32_t chn_max;
    bool chn_enabled[PYB_ADC_NUM_CHANNELS];
} pyb_adc_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_adc_obj_t pyb_adc_obj[PYB_NUM_ADCS] = { {.adc_id = PYB_ADC_0, .ADCx = ADC0, .chn_max = 0, .chn_enabled = {0}},
                                                   {.adc_id = PYB_ADC_1, .ADCx = ADC1, .chn_max = 0, .chn_enabled = {0}} };

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void adc_chns_open(pyb_adc_obj_t *self, uint32_t chns)
{
    ADC_ChnSelect(self->ADCx, chns);

    for(uint32_t i = 0; i < PYB_ADC_NUM_CHANNELS; i++)
    {
        if(chns & (1 << i))
        {
            switch ((uint32_t)self->ADCx) {
            case (uint32_t)ADC0:
                switch (i) {
                case 4:
                    PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);
                    break;
                case 5:
                    PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_IN5, 0);
                    break;
                case 6:
                    PORT_Init(PORTA, PIN10, PORTA_PIN10_ADC0_IN6, 0);
                    break;
                case 7:
                    PORT_Init(PORTA, PIN9,  PORTA_PIN9_ADC0_IN7,  0);
                    break;
                default:
                    break;
                }
                break;

            case (uint32_t)ADC1:
                switch (i) {
                case 0:
                    PORT_Init(PORTC, PIN7, PORTC_PIN7_ADC1_IN0, 0);
                    break;
                case 1:
                    PORT_Init(PORTC, PIN6, PORTC_PIN6_ADC1_IN1, 0);
                    break;
                case 2:
                    PORT_Init(PORTC, PIN5, PORTC_PIN5_ADC1_IN2, 0);
                    break;
                case 3:
                    PORT_Init(PORTC, PIN4, PORTC_PIN4_ADC1_IN3, 0);
                    break;
                case 4:
                    PORT_Init(PORTN, PIN0, PORTN_PIN0_ADC1_IN4, 0);
                    break;
                case 5:
                    PORT_Init(PORTN, PIN1, PORTN_PIN1_ADC1_IN5, 0);
                    break;
                case 6:
                    PORT_Init(PORTN, PIN2, PORTN_PIN2_ADC1_IN6, 0);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }

            self->chn_enabled[i] = true;
        }
        else
        {
            self->chn_enabled[i] = false;
        }
    }

    self->chn_max = 0;
    for(uint32_t i = 0; i < PYB_ADC_NUM_CHANNELS; i++)
    {
        if(self->chn_enabled[PYB_ADC_NUM_CHANNELS-1-i])
        {
            self->chn_max = 1 << (PYB_ADC_NUM_CHANNELS-1-i);
            break;
        }
    }
}


/******************************************************************************/
/* MicroPython bindings : adc object                                         */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_adc_obj_t *self = self_in;

    mp_printf(print, "ADC(%d, chns: [", self->adc_id);
    for(uint32_t i = 0, first = 1; i < PYB_ADC_NUM_CHANNELS; i++)
    {
        if(self->chn_enabled[i])
        {
            if(first) mp_printf(print, "%d", i);
            else      mp_printf(print, ", %d", i);

            first = 0;
        }
    }
    mp_printf(print, "])");
}

STATIC const mp_arg_t pyb_adc_init_args[] = {
    { MP_QSTR_id,                          MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_chns,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
};
STATIC mp_obj_t adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_adc_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_adc_init_args, args);

    // check the peripheral id
    uint32_t adc_id = args[0].u_int;
    if (adc_id > PYB_ADC_1) {
        mp_raise_OSError(MP_ENODEV);
    }

    // setup the object
    pyb_adc_obj_t *self = &pyb_adc_obj[adc_id];
    self->base.type = &pyb_adc_type;

    ADC_InitStructure ADC_initStruct;

    ADC_initStruct.clk_src = ADC_CLKSRC_VCO_DIV64;
    ADC_initStruct.clk_div = 24;
    ADC_initStruct.channels = 0;
    ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
    ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
    ADC_initStruct.Continue = 0;						//非连续模式，即单次模式
    ADC_initStruct.EOC_IEn = 0;
    ADC_initStruct.OVF_IEn = 0;
    ADC_initStruct.HFULL_IEn = 0;
    ADC_initStruct.FULL_IEn = 0;
    ADC_Init(self->ADCx, &ADC_initStruct);              //配置ADC

    ADC_Open(self->ADCx);								//使能ADC

    uint32_t chns = args[1].u_int;

    adc_chns_open(self, chns);

    return self;
}

STATIC mp_obj_t pyb_adc_chn_select(mp_obj_t self_in, mp_obj_t chns_in)
{
    pyb_adc_obj_t *self = self_in;

    uint32_t chns = mp_obj_get_int(chns_in);

    adc_chns_open(self, chns);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_adc_chn_select_obj, pyb_adc_chn_select);

STATIC mp_obj_t pyb_adc_read(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;

    mp_obj_t datas[PYB_ADC_NUM_CHANNELS];

    if(self->chn_max == 0) return mp_const_none;

    ADC_Start(self->ADCx);
    while(!ADC_IsEOC(self->ADCx, self->chn_max)) __NOP();

    uint32_t n = 0;
    for(uint32_t i = 0; i < PYB_ADC_NUM_CHANNELS; i++)
    {
        if(self->chn_enabled[i])
        {
            datas[n] = mp_obj_new_int(ADC_Read(self->ADCx, 1 << i));
            n++;
        }
    }

    return mp_obj_new_tuple(n, datas);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_adc_read_obj, pyb_adc_read);

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_chn_select), MP_ROM_PTR(&pyb_adc_chn_select_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),       MP_ROM_PTR(&pyb_adc_read_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_ADC_CH0),      MP_ROM_INT(ADC_CH0) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH1),      MP_ROM_INT(ADC_CH1) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH2),      MP_ROM_INT(ADC_CH2) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH3),      MP_ROM_INT(ADC_CH3) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH4),      MP_ROM_INT(ADC_CH4) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH5),      MP_ROM_INT(ADC_CH5) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH6),      MP_ROM_INT(ADC_CH6) },
    { MP_ROM_QSTR(MP_QSTR_ADC_CH7),      MP_ROM_INT(ADC_CH7) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};
