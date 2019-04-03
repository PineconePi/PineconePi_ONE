/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "mods/pybflash.h"


#define FLASH_BASE_ADDR             (1024 * 400)

#define FLASH_BLOCK_SIZE            (1024 * 4)
#define FLASH_BLOCK_COUNT           ((1024 * 512 - FLASH_BASE_ADDR) / FLASH_BLOCK_SIZE)
#define FLASH_SECTOR_SIZE           512
#define FLASH_SECTOR_COUNT          ((1024 * 512 - FLASH_BASE_ADDR) / FLASH_SECTOR_SIZE)
#define FLASH_SECTORS_PER_BLOCK     (FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE)


static uint32_t FLASH_Block_Cache[FLASH_BLOCK_SIZE/4];

static uint32_t FLASH_Block_Index = 0;
static uint32_t FLASH_Block_IndexPre = 0xFFFFFFFF;

static uint32_t FLASH_Cache_Dirty = 0;

DRESULT flash_disk_flush (void);

DRESULT flash_disk_init (void) {
    memset(FLASH_Block_Cache, 0, FLASH_BLOCK_SIZE);

    FLASH_Block_IndexPre = 0xFFFFFFFF;

    return RES_OK;
}

DRESULT flash_disk_read(BYTE *buff, DWORD sector, UINT count) {
    if ((sector + count > FLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    uint32_t index_in_block;
    for(uint32_t i = 0; i < count; i++)
    {
        index_in_block    = (sector + i) % FLASH_SECTORS_PER_BLOCK;
        FLASH_Block_Index = (sector + i) / FLASH_SECTORS_PER_BLOCK;

        if(FLASH_Block_Index != FLASH_Block_IndexPre)
        {
            flash_disk_flush();

            FLASH_Block_IndexPre = FLASH_Block_Index;

            memcpy(FLASH_Block_Cache, (void *)(FLASH_BASE_ADDR + FLASH_BLOCK_SIZE * FLASH_Block_Index), FLASH_BLOCK_SIZE);
        }

        // Copy the requested sector from the block cache
        memcpy (buff, &FLASH_Block_Cache[(index_in_block * FLASH_SECTOR_SIZE) / 4], FLASH_SECTOR_SIZE);
        buff += FLASH_SECTOR_SIZE;
    }

    return RES_OK;
}

DRESULT flash_disk_write(const BYTE *buff, DWORD sector, UINT count) {
    if ((sector + count > FLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    uint32_t index_in_block;
    for(uint32_t i = 0; i < count; i++)
    {
        index_in_block    = (sector + i) % FLASH_SECTORS_PER_BLOCK;
        FLASH_Block_Index = (sector + i) / FLASH_SECTORS_PER_BLOCK;

        if(FLASH_Block_Index != FLASH_Block_IndexPre)
        {
            flash_disk_flush();

            FLASH_Block_IndexPre = FLASH_Block_Index;

            memcpy(FLASH_Block_Cache, (void *)(FLASH_BASE_ADDR + FLASH_BLOCK_SIZE * FLASH_Block_Index), FLASH_BLOCK_SIZE);
        }

        // Copy the input sector to the block cache
        memcpy(&FLASH_Block_Cache[(index_in_block * FLASH_SECTOR_SIZE) / 4], buff, FLASH_SECTOR_SIZE);
        buff += FLASH_SECTOR_SIZE;

        FLASH_Cache_Dirty = 1;
    }

    return RES_OK;
}

DRESULT flash_disk_flush (void) {
    // Write back the cache if it's dirty
    if (FLASH_Cache_Dirty) {
        FLASH_Erase(FLASH_BASE_ADDR + FLASH_BLOCK_SIZE * FLASH_Block_IndexPre);
        FLASH_Write(FLASH_BASE_ADDR + FLASH_BLOCK_SIZE * FLASH_Block_IndexPre, FLASH_Block_Cache, FLASH_BLOCK_SIZE/4);

        FLASH_Cache_Dirty = 0;
    }
    return RES_OK;
}

/******************************************************************************/
// MicroPython bindings to expose the internal flash as an object with the
// block protocol.

// there is a singleton Flash object
STATIC const mp_obj_base_t pyb_flash_obj = {&pyb_flash_type};

STATIC mp_obj_t pyb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&pyb_flash_obj;
}

STATIC mp_obj_t pyb_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    DRESULT res = flash_disk_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_readblocks_obj, pyb_flash_readblocks);

STATIC mp_obj_t pyb_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    DRESULT res = flash_disk_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_writeblocks_obj, pyb_flash_writeblocks);

STATIC mp_obj_t pyb_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:      return MP_OBJ_NEW_SMALL_INT(flash_disk_init() != RES_OK);
        case BP_IOCTL_DEINIT:    flash_disk_flush();
                                 return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SYNC:      flash_disk_flush();
                                 return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(FLASH_SECTOR_COUNT);
        case BP_IOCTL_SEC_SIZE:  return MP_OBJ_NEW_SMALL_INT(FLASH_SECTOR_SIZE);
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_ioctl_obj, pyb_flash_ioctl);

STATIC const mp_rom_map_elem_t pyb_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_flash_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_flash_locals_dict, pyb_flash_locals_dict_table);

const mp_obj_type_t pyb_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = pyb_flash_make_new,
    .locals_dict = (mp_obj_t)&pyb_flash_locals_dict,
};

void pyb_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->readblocks[0] = (mp_obj_t)&pyb_flash_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)flash_disk_read; // native version
    vfs->writeblocks[0] = (mp_obj_t)&pyb_flash_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)flash_disk_write; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&pyb_flash_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&pyb_flash_obj;
}
