#include "bsp_sdio.h"

uint8_t bsp_SDIOInit(void)
{
	uint8_t resSD;
  PORT_Init(PORTP, PIN11, PORTP_PIN11_SD_CLK, 0);
	PORT_Init(PORTP, PIN10, PORTP_PIN10_SD_CMD, 1);
	PORT_Init(PORTP, PIN9,  PORTP_PIN9_SD_D0,   1);
	PORT_Init(PORTP, PIN8,  PORTP_PIN8_SD_D1,   1);
	PORT_Init(PORTP, PIN7,  PORTP_PIN7_SD_D2,   1);
	PORT_Init(PORTP, PIN6,  PORTP_PIN6_SD_D3,   1);
	
	resSD = SDIO_Init();
	
	return resSD;
}

