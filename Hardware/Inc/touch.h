#ifndef __TOUCH_H__
#define __TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"  // اضافه کردن کتابخانه HAL
#include "main.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmsis_os2.h"

//#include "sys.h"
//#include "gt9147.h"
//#include "ft5206.h"
//#include "gt9271.h"
///////////////////////////////////////////////// ////////////////////////////////
//This program is for learning and use only, and cannot be used for any other purpose without the author's permission
//EWB-STM32H7 development board
//Touch screen driver (support ADS7843/7846/UH7843/7846/XPT2046/TSC2046/GT9147/GT9271/FT5206, etc.) code
//Punctual  @kavirelectronic
//Technical Forum: www.kavirElectronic.ir
//Creation Date: 2020/07/22
//Version: V1.0
//Copyright, piracy must be investigated.
//Copyright(C) Guangzhou Xingyi Electronic Technology Co., Ltd. 2014-2024
//All rights reserved
//************************************************ ********************************
//Modify the description 
//no
///////////////////////////////////////////////// ////////////////////////////////

// تعریف ماکروها
#define CT_MAX_TOUCH   5    // حداکثر تعداد لمس همزمان
#define TP_PRES_DOWN   0x80 // وضعیت لمس فعال
#define TP_CATH_PRES   0x40 // وضعیت لمس غیرفعال
#define ERR_RANGE      1000   // محدوده خطای مجاز
#define READ_TIMES     5    // تعداد دفعات خواندن ADC
#define LOST_VAL       2    //
#define SAVE_ADDR_BASE 0x7D00

#define lcdWidth	   1024
#define lcdHeight	   600




////Touch screen controller
//typedef struct
//{
//	uint8_t (*init)(void);			//Initialize the touch screen controller
//	uint8_t (*scan)(uint8_t);				//Scan the touch screen. 0, screen scan; 1, physical coordinates;
//	void (*adjust)(void);		//Touch screen calibration
//	uint16_t x[CT_MAX_TOUCH]; 		//Current coordinates
//	uint16_t y[CT_MAX_TOUCH];		//The capacitive screen has up to 10 sets of coordinates, and the resistive screen uses x[0], y[0] to represent: the coordinates of the touch screen during this scan, use
//								//x[9],y[9] store the coordinates of the first press.
//	uint16_t sta;					//Pen state
//								//b15: press 1/release 0;
//	                            //b14:0, no button is pressed; 1, there is a button pressed.
//								//b13~b10: reserved
//								//b9~b0: The number of pressed points on the capacitive touch screen (0 means not pressed, 1 means pressed)
///////////////////////Touch screen calibration parameters (capacitive screen does not need calibration)/////////////////// ///

typedef struct {
    void (*Init)(void);          // تابع راه‌اندازی
    uint8_t (*scan)(uint8_t);    // تابع اسکن
    void (*Adjust)(void);        // تابع کالیبراسیون
    float xfac, yfac;            // ضرایب تبدیل X/Y
    int16_t xoff, yoff;          // آفست‌های X/Y
    uint16_t x[CT_MAX_TOUCH];    // مختصات X (خام/تبدیل‌شده)
    uint16_t y[CT_MAX_TOUCH];    // مختصات Y (خام/تبدیل‌شده)
    uint8_t sta;                 // وضعیت تاچ اسکرین
    uint8_t touchtype;           // نوع تاچ اسکرین (0: مقاومتی، 1: خازنی)
    uint16_t width;
    uint16_t height;
} _m_tp_dev;

extern _m_tp_dev tp_dev;	 	//The touch screen controller is defined in touch.c

// ساختار داده‌ای برای ذخیره‌سازی مختصات
typedef struct {
    uint16_t x;
    uint16_t y;
} TouchPoint;


typedef enum {
    UPDATE_TEXT,
    SHOW_BUTTON,
    HIDE_BUTTON
} UI_CommandType_t;

typedef struct {
    UI_CommandType_t cmdType;
    union {
        uint8_t buttonID; // برای دکمه‌ها
        char text[128];    // حداکثر طول رشته (تغییر به مقدار مورد نیاز)
    } data;
} UI_Command_t;



// ایجاد صف UI



extern _m_tp_dev tp_dev;	 	//The touch screen controller is defined in touch.c
extern TouchPoint tp;
//Resistive screen chip connection pin


#define PEN_PORT         GPIOH
#define PEN_PIN          GPIO_PIN_7

#define DOUT_PORT        GPIOG
#define DOUT_PIN         GPIO_PIN_3

#define TDIN_PORT        GPIOI
#define TDIN_PIN         GPIO_PIN_3

#define TCLK_PORT        GPIOH
#define TCLK_PIN         GPIO_PIN_6

#define TCS_PORT         GPIOI
#define TCS_PIN          GPIO_PIN_8

// توابع GPIO با HAL
#define TDIN(x)          HAL_GPIO_WritePin(TDIN_PORT, TDIN_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define TCLK(x)          HAL_GPIO_WritePin(TCLK_PORT, TCLK_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define TCS(x)           HAL_GPIO_WritePin(TCS_PORT, TCS_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define DOUT             HAL_GPIO_ReadPin(DOUT_PORT, DOUT_PIN)
#define PEN              HAL_GPIO_ReadPin(PEN_PORT, PEN_PIN)

//#define TDIN(x)   (x ? (GPIOI->BSRR = GPIO_BSRR_BS3) : (GPIOI->BSRR = GPIO_BSRR_BR3))
//#define TCLK(x)   (x ? (GPIOH->BSRR = GPIO_BSRR_BS6) : (GPIOH->BSRR = GPIO_BSRR_BR6))
//#define TCS(x)    (x ? (GPIOI->BSRR = GPIO_BSRR_BS8) : (GPIOI->BSRR = GPIO_BSRR_BR8))
//#define DOUT      (GPIOG->IDR & GPIO_IDR_ID3)
//#define PEN       (GPIOH->IDR & GPIO_IDR_ID7)

//void delay_us(uint32_t us);

// تعریف IRQ Handler
void EXTI9_5_IRQHandler(void);

// تعریف فلگ وقفه (اختیاری)
extern volatile uint8_t touch_interrupt_flag;
   
//Resistive screen function
void TP_Write_Byte(uint8_t num);									//Write a data to the control chip
uint16_t TP_Read_AD(uint8_t CMD);									//Read AD conversion value
//void Send_Message(const char *msg);
uint16_t TP_Read_XOY(uint8_t xy);									//Coordinate reading with filter (X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);						//Bidirectional reading (X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);						//Bidirectional coordinate reading with enhanced filtering

void TP_Save_Adjdata(void);											//Save calibration parameters
uint8_t TP_Get_Adjdata(void);										//Read calibration parameters
void TP_Adjust(void);												//Touch screen calibration
//Resistive screen/capacitive screen shared function
uint8_t TP_Scan(uint8_t tp);										//scanning
void GPIO_Init(void);
void TP_Init(void);													//initialization
void TP_Clear_Adjdata(void); 										//clear ajdast in 24cxx

// ارسال فرمان نمایش باتن
void UI_ShowButton(uint8_t button_id);

// ارسال فرمان مخفی‌سازی باتن
void UI_HideButton(uint8_t button_id);

// ارسال پیام به textArea
void UI_ShowMessage(const char* message);


#ifdef __cplusplus
}
#endif

#endif

















