#ifndef	_EE24CONFIG_H
#define	_EE24CONFIG_H

#include "stm32h7xx_hal.h"
extern I2C_HandleTypeDef hi2c3;

#define  _EEPROM_SIZE_KBIT     256
#define	 _EEPROM_I2C           hi2c3
#define	 _EEPROM_USE_FREERTOS  1
#define	 _EEPROM_ADDRESS       0xA0 //0xA0
#define	 _EEPROM_USE_WP_PIN    0

// در فایل ee24Config.h
#if (_EEPROM_SIZE_KBIT == 1) || (_EEPROM_SIZE_KBIT == 2)
#define _EEPROM_PSIZE     8
#elif (_EEPROM_SIZE_KBIT == 4) || (_EEPROM_SIZE_KBIT == 8) || (_EEPROM_SIZE_KBIT == 16)
#define _EEPROM_PSIZE     16
#elif (_EEPROM_SIZE_KBIT == 32) || (_EEPROM_SIZE_KBIT == 64)
#define _EEPROM_PSIZE     32
#elif (_EEPROM_SIZE_KBIT == 128) || (_EEPROM_SIZE_KBIT == 256) || (_EEPROM_SIZE_KBIT == 512)
#define _EEPROM_PSIZE     64  // اندازه صحیح برای AT24C256
#else
#define _EEPROM_PSIZE     64
#endif



#endif

