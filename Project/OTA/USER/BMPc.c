#include "SWM320.h"

#ifndef __BMP_H
#define __BMP_H


typedef enum
{
    eErrTypeBMP_NONE = 0,
    eErrTypeBMP_OPEN_FILE,
    eErrTypeBMP_FILE_HEAD,
    eErrTypeBMP_MALLOC,
    eErrTypeBMP_NOT_SUPPORT,
}BMP_ERR_TypeDef;



BMP_ERR_TypeDef LCD_ShowBMP( uint16_t x, uint16_t y, const char *bmpPathPtr );

#endif
