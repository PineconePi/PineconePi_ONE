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
#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#include "bufhelper.h"

#include "pybpin.h"
#include "pybspi.h"


/// \moduleref pyb
/// \class SPI - a master-driven serial protocol

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef enum {
    PYB_SPI_0      =  0,
    PYB_SPI_1      =  1,
    PYB_NUM_SPIS
} pyb_spi_id_t;

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    pyb_spi_id_t spi_id;
    SPI_TypeDef *SPIx;
    uint baudrate;
    byte polarity;
    byte phase;
    bool slave;
    byte bits;
} pyb_spi_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_spi_obj_t pyb_spi_obj[PYB_NUM_SPIS] = { {.spi_id = PYB_SPI_0, .SPIx = SPI0},
                                                   {.spi_id = PYB_SPI_1, .SPIx = SPI1}};

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pybspi_tx (pyb_spi_obj_t *self, const void *data)
{
    uint32_t txdata;

    if(self->bits <= 8)
    {
        txdata = (uint8_t)(*(char *)data);
    }
    else if(self->bits <= 16)
    {
        txdata = (uint16_t)(*(uint16_t *)data);
    }

    SPI_WriteWithWait(self->SPIx, txdata);
}

STATIC void pybspi_rx (pyb_spi_obj_t *self, void *data)
{
    uint32_t rxdata = SPI_Read(self->SPIx);

    if(self->bits <= 8)
    {
        *(char *)data = rxdata;
    }
    else if(self->bits <= 16)
    {
        *(uint16_t *)data = rxdata;
    }
}

STATIC void pybspi_transfer (pyb_spi_obj_t *self, const char *txdata, char *rxdata, uint32_t len, uint32_t *txchar) {
    for (int i = 0; i < len; i += (self->bits > 8 ? 2 : 1)) {
        pybspi_tx(self, txdata ? (const void *)&txdata[i] : txchar);
        pybspi_rx(self, rxdata ? (void *)&rxdata[i] : NULL);
    }
}

/******************************************************************************/
/* MicroPython bindings                                                      */
/******************************************************************************/
STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    pyb_spi_obj_t *self = self_in;

    if (self->baudrate > 0) {
        mp_printf(print, "SPI(%d, baudrate=%u, bits=%u, polarity=%u, phase=%u, mode=%s)",
                 self->spi_id, self->baudrate, self->bits, self->polarity, self->phase, self->slave ? "slave" : "master");
    } else {
        mp_printf(print, "SPI(%d)", self->spi_id);
    }
}

static const mp_arg_t pyb_spi_init_args[] = {
    { MP_QSTR_id,        MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_baudrate,  MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 1000000} },    // 1MHz
    { MP_QSTR_polarity,  MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_phase,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_slave,     MP_ARG_KW_ONLY  | MP_ARG_BOOL, {.u_bool= false} },
    { MP_QSTR_bits,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 8} },
    { MP_QSTR_mosi,      MP_ARG_REQUIRED |
                         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_miso,      MP_ARG_REQUIRED |
                         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_sclk,      MP_ARG_REQUIRED |
                         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_ssel,      MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} }, // 从机模式下需指定
};
STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_spi_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_spi_init_args, args);

    // check the peripheral id
    uint32_t spi_id = args[0].u_int;
    if (spi_id > PYB_NUM_SPIS-1) {
        mp_raise_OSError(MP_ENODEV);
    }

    pyb_spi_obj_t *self = &pyb_spi_obj[spi_id];
    self->base.type = &pyb_spi_type;

    self->baudrate = args[1].u_int;
    uint clkdiv = 7;
    for(int i = 0; i < 8; i++)
    {
        if(SystemCoreClock / (1 << (i+2)) < self->baudrate)
        {
            clkdiv = i;
            break;
        }
    }

    self->polarity = args[2].u_int ? 1 : 0;
    self->phase    = args[3].u_int ? 1 : 0;

    self->slave = args[4].u_bool;

    self->bits = args[5].u_int;
    if((self->bits < 4) || (self->bits > 16))
    {
        goto invalid_args;
    }

    SPI_InitStructure SPI_initStruct;

    SPI_initStruct.clkDiv = clkdiv;
    SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
    SPI_initStruct.SampleEdge = self->phase;
    SPI_initStruct.IdleLevel = self->polarity;
    SPI_initStruct.WordSize = self->bits;
    SPI_initStruct.Master = (self->slave ? 0 : 1);
    SPI_initStruct.RXHFullIEn = 0;
    SPI_initStruct.TXEmptyIEn = 0;
    SPI_Init(SPI0, &SPI_initStruct);
    SPI_Open(SPI0);

    pin_obj_t *pin_mosi = pin_find_by_name(args[6].u_obj);
    if((pin_mosi->pbit % 2) == 1) mp_raise_ValueError("SPI MOSI need be Even number pin, like PA0, PA2, PA4");
    pin_obj_t *pin_miso = pin_find_by_name(args[7].u_obj);
    if((pin_miso->pbit % 2) == 0) mp_raise_ValueError("SPI MISO need be Odd  number pin, like PA1, PA3, PA5");
    pin_obj_t *pin_sclk = pin_find_by_name(args[8].u_obj);
    if((pin_sclk->pbit % 2) == 0) mp_raise_ValueError("SPI SCLK need be Odd  number pin, like PA1, PA3, PA5");
    pin_obj_t *pin_ssel = mp_const_none;
    if(self->slave)
    {
        if(args[9].u_obj == mp_const_none)
        {
            mp_raise_ValueError("Slave mode: need specify SSEL pin");
        }
        else
        {
            pin_ssel = pin_find_by_name(args[9].u_obj);
            if((pin_ssel->pbit % 2) == 1) mp_raise_ValueError("SPI SSEL need be Even number pin, like PA0, PA2, PA4");
        }
    }

    switch (self->spi_id) {
    case PYB_SPI_0:
        pin_config(pin_mosi, FUNMUX0_SPI0_MOSI, 0, 0);
        pin_config(pin_miso, FUNMUX1_SPI0_MISO, 0, 0);
        pin_config(pin_sclk, FUNMUX1_SPI0_SCLK, 0, 0);
        if(pin_ssel)
            pin_config(pin_ssel, FUNMUX0_SPI0_SSEL, 0, 0);
        break;

    case PYB_SPI_1:
        pin_config(pin_mosi, FUNMUX0_SPI1_MOSI, 0, 0);
        pin_config(pin_miso, FUNMUX1_SPI1_MISO, 0, 0);
        pin_config(pin_sclk, FUNMUX1_SPI1_SCLK, 0, 0);
        if(pin_ssel)
            pin_config(pin_ssel, FUNMUX0_SPI1_SSEL, 0, 0);
        break;

    default:
        break;
    }

    return self;

invalid_args:
    mp_raise_ValueError("invalid argument(s) value");
}

STATIC mp_obj_t pyb_spi_write (mp_obj_t self_in, mp_obj_t buf)
{
    pyb_spi_obj_t *self = self_in;

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(buf, &bufinfo, data);

    pybspi_transfer(self, (const char *)bufinfo.buf, NULL, bufinfo.len, NULL);

    return mp_obj_new_int(bufinfo.len);     // return the number of bytes written
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_write_obj, pyb_spi_write);

STATIC mp_obj_t pyb_spi_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_nbytes,    MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_write,     MP_ARG_INT, {.u_int = 0x00} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // just receive
    uint32_t write = args[1].u_int;
    pybspi_transfer(self, NULL, vstr.buf, vstr.len, &write);

    // return the received data
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_read_obj, 1, pyb_spi_read);

STATIC mp_obj_t pyb_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,       MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_write,     MP_ARG_INT, {.u_int = 0x00} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // just receive
    uint32_t write = args[1].u_int;
    pybspi_transfer(self, NULL, vstr.buf, vstr.len, &write);

    // return the number of bytes received
    return mp_obj_new_int(vstr.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_readinto_obj, 1, pyb_spi_readinto);

STATIC mp_obj_t pyb_spi_write_readinto (mp_obj_t self, mp_obj_t writebuf, mp_obj_t readbuf) {
    // get buffers to write from/read to
    mp_buffer_info_t bufinfo_write;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_read;

    if (writebuf == readbuf) {
        // same object for writing and reading, it must be a r/w buffer
        mp_get_buffer_raise(writebuf, &bufinfo_write, MP_BUFFER_RW);
        bufinfo_read = bufinfo_write;
    } else {
        // get the buffer to write from
        pyb_buf_get_for_send(writebuf, &bufinfo_write, data_send);

        // get the read buffer
        mp_get_buffer_raise(readbuf, &bufinfo_read, MP_BUFFER_WRITE);
        if (bufinfo_read.len != bufinfo_write.len) {
            mp_raise_ValueError("invalid argument(s) value");
        }
    }

    // send and receive
    pybspi_transfer(self, (const char *)bufinfo_write.buf, bufinfo_read.buf, bufinfo_write.len, NULL);

    // return the number of transferred bytes
    return mp_obj_new_int(bufinfo_write.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_spi_write_readinto_obj, pyb_spi_write_readinto);

STATIC const mp_rom_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_write),               MP_ROM_PTR(&pyb_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),                MP_ROM_PTR(&pyb_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),            MP_ROM_PTR(&pyb_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto),      MP_ROM_PTR(&pyb_spi_write_readinto_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_EDGE_FIRST),          MP_ROM_INT(SPI_FIRST_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_EDGE_SECOND),         MP_ROM_INT(SPI_SECOND_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_LEVEL_LOW),           MP_ROM_INT(SPI_LOW_LEVEL) },
    { MP_ROM_QSTR(MP_QSTR_LEVEL_HIGH),          MP_ROM_INT(SPI_HIGH_LEVEL) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
