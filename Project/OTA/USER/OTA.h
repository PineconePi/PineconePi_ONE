#ifndef _OTA_H_
#define _OTA_H_

#include "swm320.h"
#include "ff.h"

void OTA_Init(void);
void OTA_SDtoSPIFLASH(const TCHAR* SD_path,const TCHAR* FLASH_path);
void OTA_SPIFLASHtoFLASH(const TCHAR* path, uint32_t flash_addr);
#endif