/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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
#include <stdlib.h>
#include <ctype.h>

#include "py/mpconfig.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "gchelper.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"
#include "gccollect.h"

#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "lib/timeutils/timeutils.h"

#include "pybuart.h"
#include "pybflash.h"


static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";
static const char fresh_boot_py[] = "# boot.py -- run on boot-up\r\n"
                                    "# can run arbitrary Python, but best to keep it minimal\r\n"
                                    #if MICROPY_STDIO_UART
                                    "import os, machine\r\n"
                                    "os.dupterm(machine.UART(0, " MP_STRINGIFY(MICROPY_STDIO_UART_BAUD) ", txd='PA3', rxd='PA2'))\r\n"
                                    #endif
                                    ;

STATIC void init_sflash_filesystem (void);

int main (void) {
    SystemInit();

    SysTick_Config(SystemCoreClock / 1000);

    gc_init(&__HeapBase, &__HeapLimit);

    // MicroPython init
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_init(mp_sys_argv, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)

    uart_init0();
    readline_init0();

    // initialize the serial flash file system
    init_sflash_filesystem();

    // append the flash paths to the system path
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));

    // reset config variables; they should be set by boot.py
    MP_STATE_PORT(machine_config_main) = MP_OBJ_NULL;

    // run boot.py
    int ret = pyexec_file("boot.py");
    if (ret & PYEXEC_FORCED_EXIT) {
        goto err_forever;
    }
    if (!ret) {
        //mperror_signal_error();
    }

    // run main.py
    ret = pyexec_file("main.py");
    if (ret & PYEXEC_FORCED_EXIT) {
        goto err_forever;
    }
    if (!ret) {
        //mperror_signal_error();
    }

    // main script is finished, so now go into REPL mode.
    for ( ; ; ) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

err_forever:
    while(1)
    {

    }
}


STATIC void init_sflash_filesystem (void) {
    FILINFO fno;

    // Initialise the local flash filesystem.
    // init the vfs object
    fs_user_mount_t *vfs_fat = malloc(sizeof(fs_user_mount_t));
    vfs_fat->flags = 0;
    pyb_flash_init_vfs(vfs_fat);

    // Create it if needed, and mount in on /flash.
    FRESULT res = f_mount(&vfs_fat->fatfs);
    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, so create a fresh one
        uint8_t working_buf[_MAX_SS];
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT | FM_SFD, 0, working_buf, sizeof(working_buf));
        if (res != FR_OK) {
            //printf("failed to create /flash");
            while(1) __NOP();
        }
    }

    // mount the flash device (there should be no other devices mounted at this point)
    // we allocate this structure on the heap because vfs->next is a root pointer
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (vfs == NULL) {
        goto fail;
    }
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    MP_STATE_PORT(vfs_cur) = vfs;

    // create /flash/sys, /flash/lib if they don't exist
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/sys")) {
        f_mkdir(&vfs_fat->fatfs, "/sys");
    }
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/lib")) {
        f_mkdir(&vfs_fat->fatfs, "/lib");
    }

    f_chdir(&vfs_fat->fatfs, "/");

    // make sure we have a /flash/boot.py.  Create it if needed.
    res = f_stat(&vfs_fat->fatfs, "/boot.py", &fno);
    if (res != FR_OK) {
        // doesn't exist, create fresh file
        FIL fp;
        f_open(&vfs_fat->fatfs, &fp, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        f_close(&fp);
    }

    res = f_stat(&vfs_fat->fatfs, "/main.py", &fno);
    if (res != FR_OK) {
        // doesn't exist, create fresh file
        FIL fp;
        f_open(&vfs_fat->fatfs, &fp, "/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
        f_close(&fp);
    }

    return;

fail:
    //printf("failed to create /flash");
    while(1) __NOP();
}

DWORD get_fattime(void) {
    //timeutils_struct_time_t tm;
    //timeutils_seconds_since_2000_to_struct_time(pyb_rtc_get_seconds(), &tm);

    // return ((tm.tm_year - 1980) << 25) | ((tm.tm_mon) << 21)  |
    //         ((tm.tm_mday) << 16)       | ((tm.tm_hour) << 11) |
    //         ((tm.tm_min) << 5)         | (tm.tm_sec >> 1);

    return 0;
}


void NORETURN __fatal_error(const char *msg) {
   while(1) __NOP();
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void) func;
    (void) file;
    (void) line;
    (void) expr;
    //printf("Assertion failed: %s, file %s, line %d\n", expr, file, line);
    __fatal_error(NULL);
}

void nlr_jump_fail(void *val) {
    __fatal_error(NULL);
}
