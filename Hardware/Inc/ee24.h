#ifndef	_EE24_H
#define	_EE24_H

#ifdef __cplusplus
extern "C" {
#endif
/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.2.1
  
  
  Reversion History:
  
  (2.2.1)
  Fix erase chip bug.
  
  (2.2.0)
  Add erase chip function.
  
  (2.1.0)
  Fix write bytes.
  
  (2.0.0)
  Rewrite again.

*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ee24Config.h"
#include "stm32h7xx_hal.h"

bool  ee24_isConnected(void);

void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len);

bool  ee24_write(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);
bool  ee24_read(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);
bool  ee24_eraseChip(void);

void  EEPROM_Write_NUM (uint16_t address,  float  fdata, uint32_t timeout);
float EEPROM_Read_NUM (uint16_t address, uint32_t timeout);

void  EEPROM_Write_LONG (uint16_t address,  long  fdata, uint32_t timeout);
long  EEPROM_Read_LONG (uint16_t address, uint32_t timeout);

//#pragma pack(push, 1)
//typedef struct {
//    long offset;
//    float calibration_factor;
//    uint32_t checksum;
//} EEPROM_Data;
//#pragma pack(pop)

HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t *data, size_t len);
HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t *data, size_t len);

// در فایل ee24.h
HAL_StatusTypeDef EEPROM_Write_PageSafe(uint16_t memAddress, uint8_t *data, size_t len);
HAL_StatusTypeDef EEPROM_Read_PageSafe(uint16_t memAddress, uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
