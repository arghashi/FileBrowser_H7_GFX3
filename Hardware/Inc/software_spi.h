//#pragma once
//#include "stm32h7xx_hal.h"
//#include "main.h"
//
//// تعریف پینهای SPI نرمافزاری (با توجه به سختافزار خود تغییر دهید)
//#define SOFT_SPI_SCLK_PIN    GPIO_PIN_6
//#define SOFT_SPI_SCLK_PORT   GPIOH
//#define SOFT_SPI_MOSI_PIN    GPIO_PIN_3
//#define SOFT_SPI_MOSI_PORT   GPIOI
//#define SOFT_SPI_MISO_PIN    GPIO_PIN_3
//#define SOFT_SPI_MISO_PORT   GPIOG
//#define SOFT_SPI_CS_PIN      GPIO_PIN_8
//#define SOFT_SPI_CS_PORT     GPIOI
//
//// تاخیر برای شبیهسازی کلاک (بر اساس سرعت CPU تنظیم شود)
////#define SOFT_SPI_DELAY()     //__asm volatile("nop \n nop \n nop \n nop")
//
//// کاهش تاخیر برای سرعت بالاتر
//#define SOFT_SPI_DELAY()  __asm volatile("nop") // فقط ۱ NOP
//
//// توابع پایه
//void SOFT_SPI_Init(void);
//void SOFT_SPI_WriteByte(uint8_t data);
//uint8_t SOFT_SPI_ReadByte(void);
//void SOFT_SPI_CS_High(void);
//void SOFT_SPI_CS_Low(void);

#pragma once
#include "stm32h7xx_hal.h"

// پیکربندی پینها (مطابق سختافزار خود تغییر دهید)
#define SOFT_SPI_SCLK_PIN    GPIO_PIN_6
#define SOFT_SPI_SCLK_PORT   GPIOH
#define SOFT_SPI_MOSI_PIN    GPIO_PIN_3
#define SOFT_SPI_MOSI_PORT   GPIOI
#define SOFT_SPI_MISO_PIN    GPIO_PIN_3
#define SOFT_SPI_MISO_PORT   GPIOG
#define SOFT_SPI_CS_PIN      GPIO_PIN_8
#define SOFT_SPI_CS_PORT     GPIOI

void SOFT_SPI_Init(void);
void SOFT_SPI_WriteByte(uint8_t data);
uint8_t SOFT_SPI_ReadByte(void);
void SOFT_SPI_CS_High(void);
void SOFT_SPI_CS_Low(void);
