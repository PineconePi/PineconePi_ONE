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
#include "pybi2c.h"

/// \moduleref pyb
/// \class I2C - a two-wire serial protocol

typedef enum {
    PYB_I2C_0      =  0,
    PYB_I2C_1      =  1,
    PYB_NUM_I2CS
} pyb_i2c_id_t;

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    pyb_i2c_id_t i2c_id;
    I2C_TypeDef *I2Cx;
    uint baudrate;
} pyb_i2c_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_i2c_obj_t pyb_i2c_obj[PYB_NUM_I2CS] = { { .i2c_id = PYB_I2C_0, .I2Cx = I2C0 },
                                                   { .i2c_id = PYB_I2C_1, .I2Cx = I2C1 } };

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC bool pyb_i2c_scan_device(pyb_i2c_obj_t *self, byte devAddr) {
    self->I2Cx->MSTDAT = (devAddr << 1) | 0;
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                         (1 << I2C_MSTCMD_WR_Pos);			//发送起始位和从机地址
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待发送完成
    if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        return false;
    }

    return true;
}

STATIC bool pyb_i2c_write(pyb_i2c_obj_t *self, byte addr, byte *data, uint len) {
    self->I2Cx->MSTDAT = (addr << 1) | 0;
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                         (1 << I2C_MSTCMD_WR_Pos);			//发送起始位和从机地址
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待发送完成
    if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        goto write_err;
    }

    while (--len) {
        self->I2Cx->MSTDAT = *data++;
        self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
        while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;	//等待发送完成
        if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
        {
            goto write_err;
        }
    }

    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;	//等待发送完成

    return true;

write_err:
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;        //等待发送完成

    return false;
}

STATIC bool pyb_i2c_mem_addr_write (pyb_i2c_obj_t *self, byte addr, byte *mem_addr, uint mem_addr_len) {
    self->I2Cx->MSTDAT = (addr << 1) | 0;
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                         (1 << I2C_MSTCMD_WR_Pos);			//发送起始位和从机地址
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待发送完成
    if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        goto write_err;
    }

    while (--mem_addr_len) {
        self->I2Cx->MSTDAT = *mem_addr++;
        self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
        while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;	//等待发送完成
        if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
        {
            goto write_err;
        }
    }

    return true;

write_err:
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;        //等待发送完成

    return false;
}

STATIC bool pyb_i2c_mem_write (pyb_i2c_obj_t *self, byte addr, byte *mem_addr, uint mem_addr_len, byte *data, uint data_len) {
    if (pyb_i2c_mem_addr_write (self, addr, mem_addr, mem_addr_len)) {

        while (data_len--) {
            self->I2Cx->MSTDAT = *data++;
            self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
            while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;//等待发送完成
            if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
            {
                goto write_err;
            }
        }

        self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
        while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;    //等待发送完成

        return true;
    }

write_err:
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;        //等待发送完成

    return false;
}

STATIC bool pyb_i2c_read(pyb_i2c_obj_t *self, byte addr, byte *data, uint len) {
    self->I2Cx->MSTDAT = (addr << 1) | 1;
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                         (1 << I2C_MSTCMD_WR_Pos);          //发送Restart
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待发送完成
    if(self->I2Cx->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        goto write_err;
    }

    for(uint i = 0; i < len-1; i++)
    {
        self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_RD_Pos) |
                             (0 << I2C_MSTCMD_ACK_Pos);
        while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;	//等待接收完成
        *data++ = self->I2Cx->MSTDAT;
    }

    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_RD_Pos)  |
                         (1 << I2C_MSTCMD_ACK_Pos) |
                         (1 << I2C_MSTCMD_STO_Pos);         //读取数据、发送NACK响应，完成后发送STOP
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待接收完成
    *data++ = self->I2Cx->MSTDAT;

    return true;

write_err:
    self->I2Cx->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while(self->I2Cx->MSTCMD & I2C_MSTCMD_TIP_Msk) ;        //等待发送完成

    return false;
}

STATIC void pyb_i2c_read_into (pyb_i2c_obj_t *self, mp_arg_val_t *args, vstr_t *vstr) {
    // get the buffer to receive into
    pyb_buf_get_for_recv(args[1].u_obj, vstr);

    // receive the data
    if (!pyb_i2c_read(self, args[0].u_int, (byte *)vstr->buf, vstr->len)) {
        mp_raise_OSError(MP_EIO);
    }
}

STATIC void pyb_i2c_readmem_into (pyb_i2c_obj_t *self, mp_arg_val_t *args, vstr_t *vstr) {
    // get the buffer to receive into
    pyb_buf_get_for_recv(args[2].u_obj, vstr);

    // get the addresses
    mp_uint_t i2c_addr = args[0].u_int;
    mp_uint_t mem_addr = args[1].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = args[3].u_int >> 3;

    // write the register address to be read from
    if (pyb_i2c_mem_addr_write (self, i2c_addr, (byte *)&mem_addr, mem_addr_size)) {
        // Read the specified length of data
        if (!pyb_i2c_read (self, i2c_addr, (byte *)vstr->buf, vstr->len)) {
            mp_raise_OSError(MP_EIO);
        }
    } else {
        mp_raise_OSError(MP_EIO);
    }
}

/******************************************************************************/
/* MicroPython bindings                                                      */
/******************************************************************************/
STATIC void pyb_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;

    mp_printf(print, "I2C(%d, baudrate=%u)", self->i2c_id, self->baudrate);
}

STATIC const mp_arg_t pyb_i2c_init_args[] = {
    { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 10000} },
    { MP_QSTR_scl,      MP_ARG_REQUIRED |
                        MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_sda,      MP_ARG_REQUIRED |
                        MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
};
STATIC mp_obj_t pyb_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_i2c_init_args, args);

    uint i2c_id = args[0].u_int;
    if (i2c_id > PYB_NUM_I2CS-1) {
        mp_raise_OSError(MP_ENODEV);
    }

    pyb_i2c_obj_t *self = &pyb_i2c_obj[i2c_id];
    self->base.type = &pyb_i2c_type;

    self->baudrate = args[1].u_int;

    pin_obj_t *pin_scl = pin_find_by_name(args[2].u_obj);
    if((pin_scl->pbit % 2) == 1) mp_raise_ValueError("I2C SCL need be Even number pin, like PA0, PA2, PA4");
    pin_obj_t *pin_sda = pin_find_by_name(args[3].u_obj);
    if((pin_sda->pbit % 2) == 0) mp_raise_ValueError("I2C SDA need be Odd  number pin, like PA1, PA3, PA5");

    switch ((uint32_t)pin_scl->port) {
    case (uint32_t)GPIOA: PORT->PORTA_PULLU |= (1 << pin_scl->pbit);  break;
    case (uint32_t)GPIOC: PORT->PORTC_PULLU |= (1 << pin_scl->pbit);  break;
    default: break;
    }

    switch ((uint32_t)pin_sda->port) {
    case (uint32_t)GPIOA: PORT->PORTA_PULLU |= (1 << pin_sda->pbit);  break;
    case (uint32_t)GPIOC: PORT->PORTC_PULLU |= (1 << pin_sda->pbit);  break;
    default: break;
    }

    switch (self->i2c_id) {
    case PYB_I2C_0:
        pin_config(pin_scl, FUNMUX0_I2C0_SCL, 0, 0);
        pin_config(pin_sda, FUNMUX1_I2C0_SDA, 0, 0);
        break;

    case PYB_I2C_1:
        pin_config(pin_scl, FUNMUX0_I2C1_SCL, 0, 0);
        pin_config(pin_sda, FUNMUX1_I2C1_SDA, 0, 0);
        break;

    default:
        break;
    }


    I2C_InitStructure I2C_initStruct;

    I2C_initStruct.Master = 1;
    I2C_initStruct.Addr7b = 1;
    I2C_initStruct.MstClk = self->baudrate;
    I2C_initStruct.MstIEn = 0;
    I2C_Init(self->I2Cx, &I2C_initStruct);

    I2C_Open(self->I2Cx);

    return self;
}

STATIC mp_obj_t pyb_i2c_scan(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (uint addr = 0x08; addr <= 0x77; addr++) {
        for (int i = 0; i < 3; i++) {
            if (pyb_i2c_scan_device(self, addr)) {
                mp_obj_list_append(list, mp_obj_new_int(addr));
                break;
            }
        }
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_scan_obj, pyb_i2c_scan);

STATIC mp_obj_t pyb_i2c_writeto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_writeto_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT,  },
        { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ,  },
    };

    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_writeto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_writeto_args, args);

    uint addr = args[0].u_int;

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[1].u_obj, &bufinfo, data);

    // send the data
    if (!pyb_i2c_write(self, addr, bufinfo.buf, bufinfo.len)) {
        mp_raise_OSError(MP_EIO);
    }

    // return the number of bytes written
    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_writeto_obj, 1, pyb_i2c_writeto);

STATIC mp_obj_t pyb_i2c_readfrom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, },
        { MP_QSTR_nbytes,  MP_ARG_REQUIRED | MP_ARG_OBJ, },
    };

    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_args, args);

    vstr_t vstr;
    pyb_i2c_read_into(self, args, &vstr);

    // return the received data
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_obj, 3, pyb_i2c_readfrom);

STATIC mp_obj_t pyb_i2c_readfrom_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_into_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, },
        { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ, },
    };

    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_into_args, args);

    vstr_t vstr;
    pyb_i2c_read_into(self, args, &vstr);

    // return the number of bytes received
    return mp_obj_new_int(vstr.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_into_obj, 1, pyb_i2c_readfrom_into);

STATIC const mp_arg_t pyb_i2c_readfrom_mem_into_args[] = {
    { MP_QSTR_addr,     MP_ARG_REQUIRED  | MP_ARG_INT, },
    { MP_QSTR_memaddr,  MP_ARG_REQUIRED  | MP_ARG_INT, },
    { MP_QSTR_buf,      MP_ARG_REQUIRED  | MP_ARG_OBJ, },
    { MP_QSTR_addrsize, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
};

STATIC mp_obj_t pyb_i2c_writeto_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args), pyb_i2c_readfrom_mem_into_args, args);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[2].u_obj, &bufinfo, data);

    // get the addresses
    mp_uint_t i2c_addr = args[0].u_int;
    mp_uint_t mem_addr = args[1].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = args[3].u_int >> 3;

    // write the register address to write to.
    if (pyb_i2c_mem_write (self, i2c_addr, (byte *)&mem_addr, mem_addr_size, bufinfo.buf, bufinfo.len)) {
        return mp_const_none;
    }

    mp_raise_OSError(MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_writeto_mem_obj, 1, pyb_i2c_writeto_mem);

STATIC mp_obj_t pyb_i2c_readfrom_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_mem_args[] = {
        { MP_QSTR_addr,     MP_ARG_REQUIRED  | MP_ARG_INT, },
        { MP_QSTR_memaddr,  MP_ARG_REQUIRED  | MP_ARG_INT, },
        { MP_QSTR_nbytes,   MP_ARG_REQUIRED  | MP_ARG_OBJ, },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
    };

    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_mem_args, args);

    vstr_t vstr;
    pyb_i2c_readmem_into (self, args, &vstr);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_mem_obj, 1, pyb_i2c_readfrom_mem);

STATIC mp_obj_t pyb_i2c_readfrom_mem_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_mem_into_args, args);

    // get the buffer to read into
    vstr_t vstr;
    pyb_i2c_readmem_into(self, args, &vstr);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_mem_into_obj, 1, pyb_i2c_readfrom_mem_into);


STATIC const mp_rom_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_scan),                MP_ROM_PTR(&pyb_i2c_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto),             MP_ROM_PTR(&pyb_i2c_writeto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom),            MP_ROM_PTR(&pyb_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into),       MP_ROM_PTR(&pyb_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_mem),         MP_ROM_PTR(&pyb_i2c_writeto_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem),        MP_ROM_PTR(&pyb_i2c_readfrom_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem_into),   MP_ROM_PTR(&pyb_i2c_readfrom_mem_into_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
