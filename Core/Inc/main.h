/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include <stdio.h>
#include "string.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32h7xx_hal.h"
#include <math.h>
//#include "sys.h"
//#include "arm_math.h"

#include "sdram.h"
#include "Buzzer.h"
//#include "Battery.h"

//#include <uart_dma.h>
//#include <command_processor.h>


#ifdef EEPROM_ENABLE
#include "ee24.h"
#endif /*EEPROM_ENABLE*/

#ifdef GT911_ENABLE
#include "gt911.h"
#endif /*GT911_ENABLE*/

#ifdef XPT2046_ENABLE
#include "touch.h"
#endif /*XPT2046_ENABLE*/


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
    MODULE_BLUETOOTH,
    MODULE_QI_CHARGER,
    MODULE_SYNC,
    MODULE_SAMPLE_LOAD,
    MODULE_HALF_LOAD,
    MODULE_SYNCMASTER
} ModuleType;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern uint32_t get_random_number(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Buzzer_Pin GPIO_PIN_4
#define Buzzer_GPIO_Port GPIOE
#define TOUCH_RST_Pin GPIO_PIN_8
#define TOUCH_RST_GPIO_Port GPIOI
#define CARD_DETECT_Pin GPIO_PIN_13
#define CARD_DETECT_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOH
#define LED_GR_Pin GPIO_PIN_0
#define LED_GR_GPIO_Port GPIOB
#define LED_RD_Pin GPIO_PIN_1
#define LED_RD_GPIO_Port GPIOB
#define T_CLK_Pin GPIO_PIN_6
#define T_CLK_GPIO_Port GPIOH
#define TOUCH_INT_Pin GPIO_PIN_7
#define TOUCH_INT_GPIO_Port GPIOH
#define TOUCH_INT_EXTI_IRQn EXTI9_5_IRQn
#define T_MISO_Pin GPIO_PIN_3
#define T_MISO_GPIO_Port GPIOG
#define T_MOSI_Pin GPIO_PIN_3
#define T_MOSI_GPIO_Port GPIOI
#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB
#define GT911_SDA_Pin GPIO_PIN_7
#define GT911_SDA_GPIO_Port GPIOB
#define GT911_SCL_Pin GPIO_PIN_8
#define GT911_SCL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;


/*
 * ۳. انواع Release Type پیشنهادی
نماد		معنی					توضیحات
A		Alpha				نسخه آزمایشی اولیه با قابلیت‌های ناپایدار
B		Beta				نسخه آزمایشی پیشرفته با خطاهای شناخته شده
RC		Release Candidate	نسخه نهایی احتمالی برای تست نهایی
S		Stable  			نسخه پایدار و منتشرشده
D		Dev	   				نسخه در حال توسعه (Development)
 */

#define UIMAJOR 					4   		// UI TouchGFX Major Number
#define UIMINOR 					25   		// UI TouchGFX Minor Number
#define UIPATCH 					0   		// UI TouchGFX Patch Number
#define UIBUILD						7			// UI TouchGFX Build Number

#define MAJOR 						9   		// Software Major Number
#define MINOR 						18   		// Software Minor Number
#define PATCH 						0   		// Software Patch Number
#define BUILD						8			// Software Build Number

#define PCBMAJOR 					4   		// PCB Major Number
#define PCBMINOR 					0   		// PCB Minor Number
#define PCBPATCH 					2   		// PCB Patch Number
#define PCBRELEASE					'A'			// PCB Release Number

#define MANUFACTURE_YEAR 			2025  		// سال ساخت دستگاه

#define EXT_STORAGE				   16         	//SD-card capacity
#define Battery_Capacity		   10000        //Battery capacity in mAH


// وضعیت ماژول‌ها
#define BLUETOOTH_ENABLED    		1
#define QI_CHARGER_ENABLED   		0
#define SYNC_ENABLED         		1
#define SAMPLE_LOAD_ENABLED    		1
#define HALF_LOAD_ENABLED   		1
#define SYNCMASTER_ENABLED         	1


#define MAX_DIR_NAME_LEN           100
#define MAX_FILE_NAME_LEN          100
#define MAX_OPEN_DIR_NAME_LEN      100
#define MAX_WRITE_BUFF_SIZE        512
//#define MAX_READ_BUFF_SIZE		   512

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
