#ifndef _SDRAM_H
#define _SDRAM_H
//#include "sys.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_sdram.h"

//#include "fmc.h"
#include "main.h"
#include "inttypes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//This program is for learning and use only, and cannot be used for any other purpose without the author's permission
//ALIENTEK STM32F7 development board
//SDRAM driver code
//Punctual Atom @ALIENTEK
//Technical Forum: www.openedv.com
//Creation Date: 2015/11/27
//Version: V1.0
//Copyright, piracy must be investigated.
//Copyright(C) Guangzhou Xingyi Electronic Technology Co., Ltd. 2014-2024
//All rights reserved
///////////////////////////////////////////////// ////////////////////////////////
extern SDRAM_HandleTypeDef SDRAM_Handler;//SDRAM handle
//extern SDRAM_HandleTypeDef hsdram1;
#define Bank5_SDRAM_ADDR    ((uint32_t)(0XC0000000)) //SDRAM start address

//SDRAM configuration parameters
//#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
//#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
//#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
//#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
//#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
//#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
//#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
//#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
//#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
//#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
//#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

#define REFRESH_COUNT                            ((uint32_t)1386)       /*SDRAM refresh counter (120MHz SDRAM clock)*/
#define SDRAM_TIMEOUT                            ((uint32_t)0xFFFF)
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

void SDRAM_Init(void);
void SDRAM_MPU_Config(void);
uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval);
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n);
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t n);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);
#endif
