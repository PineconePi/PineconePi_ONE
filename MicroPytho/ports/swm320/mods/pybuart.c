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
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mphal.h"

#include "lib/utils/interrupt_char.h"

#include "mods/moduos.h"
#include "mods/pybpin.h"
#include "mods/pybuart.h"


/// \moduleref pyb
/// \class UART - duplex serial communication bus

/******************************************************************************
 DEFINE CONSTANTS
 *******-***********************************************************************/
#define PYBUART_FRAME_TIME_US(baud)             ((11 * 1000000) / baud)

#define PYBUART_RX_BUFFER_LEN                   (256)

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef enum {
    PYB_UART_0      =  0,
    PYB_UART_1      =  1,
    PYB_UART_2      =  2,
    PYB_UART_3      =  3,
    PYB_NUM_UARTS
} pyb_uart_id_t;

struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    pyb_uart_id_t uart_id;
    UART_TypeDef *UARTx;
    uint baudrate;
    byte databits;
    byte parity;
    byte stopbits;

    byte *read_buf;                     // read buffer pointer
    volatile uint16_t read_buf_head;    // indexes first empty slot
    uint16_t read_buf_tail;             // indexes first full slot (not full if equals head)
};

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_uart_obj_t pyb_uart_obj[PYB_NUM_UARTS] = {
    {.uart_id = PYB_UART_0, .UARTx = UART0, .read_buf = NULL},
    {.uart_id = PYB_UART_1, .UARTx = UART1, .read_buf = NULL},
    {.uart_id = PYB_UART_2, .UARTx = UART2, .read_buf = NULL},
    {.uart_id = PYB_UART_3, .UARTx = UART3, .read_buf = NULL},
};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void uart_init0 (void) {
    // save references of the UART objects, to prevent the read buffers from being trashed by the gc
    MP_STATE_PORT(pyb_uart_objs)[0] = &pyb_uart_obj[0];
    MP_STATE_PORT(pyb_uart_objs)[1] = &pyb_uart_obj[1];
    MP_STATE_PORT(pyb_uart_objs)[2] = &pyb_uart_obj[2];
    MP_STATE_PORT(pyb_uart_objs)[3] = &pyb_uart_obj[3];
}

int uart_rx_any(pyb_uart_obj_t *self) {
    if (self->read_buf_tail != self->read_buf_head) {
        // buffering via irq
        return (self->read_buf_head > self->read_buf_tail) ? self->read_buf_head - self->read_buf_tail :
                PYBUART_RX_BUFFER_LEN - self->read_buf_tail + self->read_buf_head;
    }
    else {
        // no buffering
        return self->UARTx->FIFO & UART_FIFO_RXLVL_Msk;
    }
}

int uart_rx_char(pyb_uart_obj_t *self) {
    if (self->read_buf_tail != self->read_buf_head) {
        // buffering via irq
        int data = self->read_buf[self->read_buf_tail];
        self->read_buf_tail = (self->read_buf_tail + 1) % PYBUART_RX_BUFFER_LEN;
        return data;
    } else {
        // no buffering
        return self->UARTx->DATA & UART_DATA_DATA_Msk;
    }
}

// Waits at most timeout microseconds for at least 1 char to become ready for reading
// Returns true if something available, false if not.
bool uart_rx_wait (pyb_uart_obj_t *self) {
    int timeout = PYBUART_FRAME_TIME_US(self->baudrate) * 10;   // 10 charaters time
    for ( ; ; ) {
        if (uart_rx_any(self)) {
            return true; // we have at least 1 char ready for reading
        }
        if (timeout > 0) {
            mp_hal_delay_us(1);
            timeout--;
        }
        else {
            return false;
        }
    }
}

bool uart_tx_char(pyb_uart_obj_t *self, int c) {
    while(UART_IsTXFIFOFull(self->UARTx)) __NOP();

    self->UARTx->DATA = c;

    return true;
}

bool uart_tx_strn(pyb_uart_obj_t *self, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (!uart_tx_char(self, *str)) {
            return false;
        }
    }
    return true;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************/
/* MicroPython bindings                                                      */

STATIC void pyb_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_uart_obj_t *self = self_in;

    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, ", self->uart_id, self->baudrate,
                                                        self->databits == UART_DATA_8BIT ? 8 : 9);
    switch (self->parity) {
    case UART_PARITY_NONE:  mp_printf(print, "parity=None, "); break;
    case UART_PARITY_EVEN:  mp_printf(print, "parity=Even, "); break;
    case UART_PARITY_ODD:   mp_printf(print, "parity=Odd, ");  break;
    case UART_PARITY_ZERO:  mp_printf(print, "parity=Zero, "); break;
    case UART_PARITY_ONE:   mp_printf(print, "parity=One, ");  break;
    default:break;
    }
    mp_printf(print, "stop=%u)", self->stopbits == UART_STOP_1BIT ? 1 : 2);
}

STATIC const mp_arg_t pyb_uart_init_args[] = {
    { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 9600} },
    { MP_QSTR_bits,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = UART_DATA_8BIT} },
    { MP_QSTR_parity,   MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = UART_PARITY_NONE} },
    { MP_QSTR_stop,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = UART_STOP_1BIT} },
    { MP_QSTR_txd,      MP_ARG_REQUIRED |
                        MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_rxd,      MP_ARG_REQUIRED |
                        MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
};
STATIC mp_obj_t pyb_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_uart_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_uart_init_args, args);

    uint uart_id = args[0].u_int;
    if (uart_id > PYB_NUM_UARTS-1) {
        mp_raise_OSError(MP_ENODEV);
    }

    pyb_uart_obj_t *self = &pyb_uart_obj[uart_id];
    self->base.type = &pyb_uart_type;

    self->baudrate = args[1].u_int;

    self->databits = args[2].u_int;

    self->parity   = args[3].u_int;

    self->stopbits = args[4].u_int;
    if((self->stopbits != UART_STOP_1BIT) && (self->stopbits != UART_STOP_2BIT))
    {
        goto invalid_args;
    }

    pin_obj_t *pin_txd = pin_find_by_name(args[5].u_obj);
    if((pin_txd->pbit % 2) == 0) mp_raise_ValueError("UART TXD need be Odd  number pin, like PA1, PA3, PA5");
    pin_obj_t *pin_rxd = pin_find_by_name(args[6].u_obj);
    if((pin_rxd->pbit % 2) == 1) mp_raise_ValueError("UART RXD need be Even number pin, like PA0, PA2, PA4");

    switch (self->uart_id) {
    case PYB_UART_0:
        pin_config(pin_txd, FUNMUX1_UART0_TXD, 0, 0);
        pin_config(pin_rxd, FUNMUX0_UART0_RXD, 0, 0);
        break;

    case PYB_UART_1:
        pin_config(pin_txd, FUNMUX1_UART1_TXD, 0, 0);
        pin_config(pin_rxd, FUNMUX0_UART1_RXD, 0, 0);
        break;

    case PYB_UART_2:
        pin_config(pin_txd, FUNMUX1_UART2_TXD, 0, 0);
        pin_config(pin_rxd, FUNMUX0_UART2_RXD, 0, 0);
        break;

    case PYB_UART_3:
        pin_config(pin_txd, FUNMUX1_UART3_TXD, 0, 0);
        pin_config(pin_rxd, FUNMUX0_UART3_RXD, 0, 0);
        break;

    default:
        break;
    }

    self->read_buf_head = 0;
    self->read_buf_tail = 0;
    self->read_buf = MP_OBJ_NULL; // free the read buffer before allocating again
    self->read_buf = m_new(byte, PYBUART_RX_BUFFER_LEN);

    UART_InitStructure UART_initStruct;

    UART_initStruct.Baudrate = self->baudrate;
    UART_initStruct.DataBits = self->databits;
    UART_initStruct.Parity = self->parity;
    UART_initStruct.StopBits = self->stopbits;
    UART_initStruct.RXThreshold = 4;
    UART_initStruct.RXThresholdIEn = 1;
    UART_initStruct.TimeoutTime = 10;   //10个字符时间内未接收到新的数据则触发超时中断
    UART_initStruct.TimeoutIEn = 1;
    UART_initStruct.TXThresholdIEn = 0;
    UART_Init(self->UARTx, &UART_initStruct);
    UART_Open(self->UARTx);

    return self;

invalid_args:
    mp_raise_ValueError("invalid argument(s) value");
}

STATIC mp_obj_t pyb_uart_any(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;

    return mp_obj_new_int(uart_rx_any(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_any_obj, pyb_uart_any);

STATIC mp_uint_t pyb_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!uart_rx_wait(self)) {
        // return MP_EAGAIN error to indicate non-blocking (then read() method returns None)
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // read the data
    byte *orig_buf = buf;
    for ( ; ; ) {
        *buf++ = uart_rx_char(self);
        if (--size == 0 || !uart_rx_wait(self)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t pyb_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    const char *buf = buf_in;

    // write the data
    if (!uart_tx_strn(self, buf, size)) {
        mp_raise_OSError(MP_EIO);
    }
    return size;
}

STATIC mp_uint_t pyb_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    mp_uint_t ret;

    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && uart_rx_any(self)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (!UART_IsTXFIFOFull(self->UARTx))) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}


STATIC void UARTX_Handler(uint32_t uart_id) {
    pyb_uart_obj_t *self = &pyb_uart_obj[uart_id];
    uint32_t chr;

    if(UART_INTRXThresholdStat(self->UARTx) || UART_INTTimeoutStat(self->UARTx))
    {
        while(UART_IsRXFIFOEmpty(self->UARTx) == 0)
        {
            if(UART_ReadByte(self->UARTx, &chr) == 1)
            {
                if (MP_STATE_PORT(os_term_dup_obj) && MP_STATE_PORT(os_term_dup_obj)->stream_o == self && chr == mp_interrupt_char) {
                    // raise an exception when interrupts are finished
                    mp_keyboard_interrupt();
                } else {
                    uint next_head = (self->read_buf_head + 1) % PYBUART_RX_BUFFER_LEN;
                    if (next_head != self->read_buf_tail) {
                        self->read_buf[self->read_buf_head] = chr;
                        self->read_buf_head = next_head;
                    }
                }
            }
        }
    }
}

void UART0_Handler(void) {
    UARTX_Handler(PYB_UART_0);
}

void UART1_Handler(void) {
    UARTX_Handler(PYB_UART_1);
}

void UART2_Handler(void) {
    UARTX_Handler(PYB_UART_2);
}

void UART3_Handler(void) {
    UARTX_Handler(PYB_UART_3);
}


STATIC const mp_stream_p_t uart_stream_p = {
    .read = pyb_uart_read,
    .write = pyb_uart_write,
    .ioctl = pyb_uart_ioctl,
    .is_text = false,
};


STATIC const mp_rom_map_elem_t pyb_uart_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_any),         MP_ROM_PTR(&pyb_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),        MP_ROM_PTR(&mp_stream_read_obj) },                  // read([nbytes])
    { MP_ROM_QSTR(MP_QSTR_readline),    MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },   // readline()
    { MP_ROM_QSTR(MP_QSTR_readinto),    MP_ROM_PTR(&mp_stream_readinto_obj) },              // readinto(buf[, nbytes])
    { MP_ROM_QSTR(MP_QSTR_write),       MP_ROM_PTR(&mp_stream_write_obj) },                 // write(buf)

    // class constants
    { MP_ROM_QSTR(MP_QSTR_DATA_8BIT),   MP_ROM_INT(UART_DATA_8BIT) },
    { MP_ROM_QSTR(MP_QSTR_DATA_9BIT),   MP_ROM_INT(UART_DATA_9BIT) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(UART_PARITY_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ODD),  MP_ROM_INT(UART_PARITY_ODD) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(UART_PARITY_EVEN) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ONE),  MP_ROM_INT(UART_PARITY_ONE) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ZERO), MP_ROM_INT(UART_PARITY_ZERO) },
    { MP_ROM_QSTR(MP_QSTR_STOP_1BIT),   MP_ROM_INT(UART_STOP_1BIT) },
    { MP_ROM_QSTR(MP_QSTR_STOP_2BIT),   MP_ROM_INT(UART_STOP_2BIT) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

const mp_obj_type_t pyb_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = pyb_uart_print,
    .make_new = pyb_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};
