#include "touch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cmsis_os2.h"
#include "math.h"
#include "stdlib.h"

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
// تعریف صف برای انتقال پیام‌های کالیبراسیون
//osMessageQueueId_t calibrationQueue;
//#define CALIBRATION_QUEUE_SIZE 10
// تعریف صف برای ذخیره‌سازی مختصات تاچ (X, Y)
//osMessageQueueId_t touchQueue;
//#define TOUCH_QUEUE_SIZE 10

SemaphoreHandle_t xSPIMutex;
//extern osMessageQueueId_t messageQueueHandle;
//extern osMessageQueueId_t calibrationQueueHandle;
//extern osMessageQueueId_t touchQueueHandle;

extern osMessageQueueId_t uiQueueHandle;

// تعریف Mutexها (اختیاری)
extern osMutexId_t xTPMutexHandle;
extern osMutexId_t xDataMutexHandle;

_m_tp_dev tp_dev =
{ .Init = TP_Init, .scan = TP_Scan, .Adjust = TP_Adjust, .xfac = 0, .yfac = 0,
		.xoff = 0, .yoff = 0, .sta = 0, .touchtype = 0, .width = lcdWidth,
		.height =
		lcdHeight };

//The default is data with touchtype=0.
uint8_t CMD_RDX = 0XD0;
uint8_t CMD_RDY = 0X90;

void TP_Write_Byte(uint8_t num)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (num & 0x80)
			TDIN(1);
		else
			TDIN(0);
		num <<= 1;
		TCLK(0);
		delay_us(1);
		TCLK(1);
	}
}

// SPI Read
uint16_t TP_Read_AD(uint8_t CMD)
{
	uint16_t Num = 0;
	TCS(0); // انتخاب چیپ
	TP_Write_Byte(CMD);
	delay_us(6);
	TCLK(0);
	delay_us(1);
	TCLK(1); // پاک کردن BUSY
	delay_us(1);
	TCLK(0);

	for (uint8_t i = 0; i < 16; i++)
	{
		Num <<= 1;
		TCLK(0);
		delay_us(1);
		TCLK(1);
		if (DOUT)
			Num++;
	}
	Num >>= 4;
	TCS(1); // آزادسازی چیپ
	return Num;
}

//void Send_Message(const char *msg)
//{
//	if (messageQueueHandle != NULL)
//	{
//		osMessageQueuePut(messageQueueHandle, msg, 0, 0);
//	}
//}

//Read a coordinate value (x or y)
//Continuously read READ_TIMES data, and arrange these data in ascending order,
//Then remove the lowest and highest LOST_VAL numbers and take the average
//xy: instruction (CMD_RDX/CMD_RDY)
//Return value: the data read
//#define READ_TIMES 	5 		//Read times
//#define LOST_VAL 	1	  	//Drop value
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum = 0;
	uint16_t temp;
	for (i = 0; i < READ_TIMES; i++)
		buf[i] = TP_Read_AD(xy);
	for (i = 0; i < READ_TIMES - 1; i++) //Sort
	{
		for (j = i + 1; j < READ_TIMES; j++)
		{
			if (buf[i] > buf[j]) //Ascending
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
	sum = 0;
	for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
		sum += buf[i];
	temp = sum / (READ_TIMES - 2 * LOST_VAL);
	return temp;
}
//Read x,y coordinates
//The minimum value cannot be less than 100.
//x,y: the coordinate value read
//Return value: 0, failure; 1, success.
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
	uint16_t xtemp, ytemp;
	xtemp = TP_Read_XOY(CMD_RDX);
	ytemp = TP_Read_XOY(CMD_RDY);
	//if(xtemp<100||ytemp<100)return 0;//Reading failed
	*x = xtemp;
	*y = ytemp;
	return 1; //Successful reading
}
//Read the touch screen IC twice in a row, and the deviation of these two times cannot exceed
//ERR_RANGE, if the conditions are met, the reading is considered correct, otherwise the reading is wrong.
//This function can greatly improve the accuracy
//x,y: the coordinate value read
//Return value: 0, failure; 1, success.
//#define ERR_RANGE 1000 //tolerance scope
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
	uint16_t x1, y1;
	uint16_t x2, y2;
	uint8_t flag;
	flag = TP_Read_XY(&x1, &y1);
	if (flag == 0)
		return (0);
	flag = TP_Read_XY(&x2, &y2);
	if (flag == 0)
		return (0);
	if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //Two samples before and after are within +-50
	&& ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
	{
		*x = (x1 + x2) / 2;
		*y = (y1 + y2) / 2;
		return 1;
	}
	else
		return 0;
}

///////////////////////////////////////////////// /////////////////////////////////
//Touch button scan
//tp:0, screen coordinates; 1, physical coordinates (used for special occasions such as calibration)
//Return value: current touch screen state.
//0, no touch on the touch screen; 1, touch on the touch screen
uint8_t TP_Scan(uint8_t tp)
{
	if (touch_interrupt_flag)
	{
		touch_interrupt_flag = 0; // ریست فلگ

		if (PEN == 0) //Button pressed
		{
			if (tp)
				TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]); //Read physical coordinates
			else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0])) //Read screen coordinates
			{
				tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; //Convert the result to screen coordinates
				tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
			}
			if ((tp_dev.sta & TP_PRES_DOWN) == 0) //Hasn't been pressed before
			{
				tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; //Key press
				tp_dev.x[CT_MAX_TOUCH - 1] = tp_dev.x[0]; //Record the coordinates of the first press
				tp_dev.y[CT_MAX_TOUCH - 1] = tp_dev.y[0];
			}
			//		printf("Raw X=%d, Raw Y=%d\n", tp_dev.x[0], tp_dev.y[0]); // دیباگ
		}
		else
		{
			if (tp_dev.sta & TP_PRES_DOWN)		//Was pressed before
			{
				tp_dev.sta &= ~TP_PRES_DOWN;	//Mark button release
			}
			else	//Has not been pressed before
			{
				tp_dev.x[CT_MAX_TOUCH - 1] = 0;
				tp_dev.y[CT_MAX_TOUCH - 1] = 0;
				tp_dev.x[0] = 0xffff;
				tp_dev.y[0] = 0xffff;
			}
		}
	}

	return tp_dev.sta & TP_PRES_DOWN;//Return to the current touch screen state
}
///////////////////////////////////////////////// ////////////////////////
//The base address of the address range saved in the EEPROM, which occupies 13 bytes (RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)

//Save calibration parameters									    
void TP_Save_Adjdata(void)
{
	int temp;
	//Save the calibration results!
	temp = tp_dev.xfac * 100000000;	//Save x correction factor
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE, temp, 4);
	temp = tp_dev.yfac * 100000000;	//Save y correction factor
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE + 4, temp, 4);
	//Save x offset
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE + 8, tp_dev.xoff, 2);
	//Save y offset
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE + 10, tp_dev.yoff, 2);
	//Save touch screen type
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE + 12, tp_dev.touchtype);
	temp = 0X0A;	//Mark calibrated
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE + 13, temp);
}

//Clear calibration parameters
void TP_Clear_Adjdata(void)
{
	int temp = 0;

	temp = 0X00;	//Mark calibrated
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE + 13, temp);

}

//Get the calibration value stored in EEPROM
//Return value: 1, successfully get data
// 0, acquisition failed, need to recalibrate
uint8_t TP_Get_Adjdata(void)
{
	int tempfac;
	tempfac = AT24CXX_ReadOneByte(SAVE_ADDR_BASE + 13);	//Read the mark to see if it has been calibrated!
	if (tempfac == 0X0A)	//The touch screen has been calibrated
	{
		tempfac = AT24CXX_ReadLenByte(SAVE_ADDR_BASE, 4);
		tp_dev.xfac = (float) tempfac / 100000000;//Get x calibration parameters
		tempfac = AT24CXX_ReadLenByte(SAVE_ADDR_BASE + 4, 4);
		tp_dev.yfac = (float) tempfac / 100000000;//Get y calibration parameters
		//Get x offset
		tp_dev.xoff = AT24CXX_ReadLenByte(SAVE_ADDR_BASE + 8, 2);
		//Get y offset
		tp_dev.yoff = AT24CXX_ReadLenByte(SAVE_ADDR_BASE + 10, 2);
		tp_dev.touchtype = AT24CXX_ReadOneByte(SAVE_ADDR_BASE + 12);//Read touch screen type mark
		if (tp_dev.touchtype)	    //X, Y direction is opposite to the screen
		{
			CMD_RDX = 0X90;
			CMD_RDY = 0XD0;
		}
		else				   //X and Y directions are the same as the screen
		{
			CMD_RDX = 0XD0;
			CMD_RDY = 0X90;
		}
		return 1;
	}
	return 0;
}


volatile uint8_t touch_interrupt_flag = 0;
void GPIO_Init(void) {
    // Enable clocks for ports
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN | RCC_AHB4ENR_GPIOIEN | RCC_AHB4ENR_GPIOGEN;

    // پیکربندی T_CLK (خروجی)
    GPIOH->MODER &= ~(3U << (6*2));      // Clear mode
    GPIOH->MODER |=  (1U << (6*2));      // Output mode
    GPIOH->OTYPER &= ~(1U << 6);         // Push-pull
    GPIOH->OSPEEDR |= (3U << (6*2));     // Very high speed

    // پیکربندی T_CS (خروجی)
    GPIOI->MODER &= ~(3U << (8*2));
    GPIOI->MODER |=  (1U << (8*2));
    GPIOI->OTYPER &= ~(1U << 8);
    GPIOI->OSPEEDR |= (3U << (8*2));

    // پیکربندی T_DIN (خروجی)
    GPIOI->MODER &= ~(3U << (3*2));
    GPIOI->MODER |=  (1U << (3*2));
    GPIOI->OTYPER &= ~(1U << 3);
    GPIOI->OSPEEDR |= (3U << (3*2));

    // پیکربندی T_DOUT (ورودی)
    GPIOG->MODER &= ~(3U << (3*2));      // Input mode
    GPIOG->PUPDR |=  (1U << (3*2));      // Pull-up

    // پیکربندی T_PEN (ورودی وقفه)
    GPIOH->MODER &= ~(3U << (7*2));      // Input mode
    GPIOH->PUPDR |=  (1U << (7*2));      // Pull-up

    // تنظیم وقفه برای T_PEN
    EXTI->IMR1 |= (1 << 7);              // Enable interrupt
    EXTI->FTSR1 |= (1 << 7);             // Falling edge trigger
    NVIC_EnableIRQ(EXTI9_5_IRQn);        // Enable NVIC
    NVIC_SetPriority(EXTI9_5_IRQn, 2);   // Set priority
}
//void GPIO_Init(void) {
//    // فعال کردن کلاک پورت‌ها
//    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN | RCC_AHB4ENR_GPIOIEN | RCC_AHB4ENR_GPIOGEN;
//
//    // پیکربندی T_PEN (PH7) به عنوان ورودی با وقفه
//    GPIOH->MODER &= ~GPIO_MODER_MODE7; // حالت ورودی
//    GPIOH->PUPDR |= GPIO_PUPDR_PUPD7_0; // Pull-up
//    GPIOH->OSPEEDR |= GPIO_OSPEEDR_OSPEED7; // سرعت بالا
//
//    // تنظیم وقفه روی لبه پایین‌رونده (Falling Edge)
//    EXTI->FTSR1 |= EXTI_FTSR1_TR; // لبه پایین‌رونده برای خط ۷
//    EXTI->IMR1 |= EXTI_IMR1_IM7;   // فعال کردن وقفه برای خط ۷
//
//    // تنظیم NVIC
//    NVIC_SetPriority(EXTI9_5_IRQn, 0); // اولویت وقفه
//    NVIC_EnableIRQ(EXTI9_5_IRQn);      // فعال کردن وقفه
//}

// هندلر وقفه
//void EXTI9_5_IRQHandler(void) {
//    if (EXTI->PR1 & (1 << 7)) { // به جای EXTI_PR1_PIF7
//        EXTI->PR1 = (1 << 7);   // پاک کردن فلگ وقفه
//        touch_interrupt_flag = 1;
//    }
//}

//Touch screen calibration code
//Get four calibration parameters

void TP_Init(void)
{
//	if (lcddev.id == 0X5510)		//Capacitive touch screen, 4.3 inch screen
//	{
//		GT9147_Init();
//		tp_dev.scan = GT9147_Scan;//Scan function points to GT9147 touch screen scanning
//		tp_dev.touchtype |= 0X80;			//Capacitive screen
//		tp_dev.touchtype |= lcddev.dir & 0X01;			//Landscape or portrait
//		return 0;
//	}
//	else if (lcddev.id == 0X1963)//SSD1963 7-inch screen or 7-inch 800*480/1024*600 RGB screen
//	{
//		FT5206_Init();
//		tp_dev.scan = FT5206_Scan;//Scan function points to FT5206 touch screen scan
//		tp_dev.touchtype |= 0X80;			//Capacitive screen
//		tp_dev.touchtype |= lcddev.dir & 0X01;			//Landscape or portrait
//		return 0;
//	}
//	else if (lcddev.id == 0X1018)
//	{
//		GT9271_Init();
//		tp_dev.scan = GT9271_Scan;//Scan function points to GT271 touch screen scanning
//		tp_dev.touchtype |= 0X80;			//Capacitive screen
//		tp_dev.touchtype |= lcddev.dir & 0X01;			//Landscape or portrait
//		return 0;
//	}
//	else
//	{
    GPIO_Init(); // پیکربندی پین‌ها
//		GPIO_InitTypeDef GPIO_InitStruct =
//		{ 0 };
//
//		// پیکربندی پین‌ها با HAL
//		__HAL_RCC_GPIOH_CLK_ENABLE();
//		__HAL_RCC_GPIOI_CLK_ENABLE();
//		__HAL_RCC_GPIOG_CLK_ENABLE();
//
//		// پیکربندی PH6 (T_CLK) به عنوان خروجی
//		GPIO_InitStruct.Pin = GPIO_PIN_6;
//		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//		GPIO_InitStruct.Pull = GPIO_PULLUP;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
//
//		// پیکربندی PI3 (T_MOSI) و PI8 (T_CS) به عنوان خروجی
//		GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_8;
//		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
//
//		// پیکربندی PH7 (T_PEN) به عنوان ورودی
//		GPIO_InitStruct.Pin = GPIO_PIN_7;
//		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
//
//		// پیکربندی PG3 (T_MISO) به عنوان ورودی
//		GPIO_InitStruct.Pin = GPIO_PIN_3;
//		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//	TP_Get_Adjdata();
//		if (TP_Get_Adjdata())
//			return 0;			//Already calibrated
//		else			   //Not calibrated?
//		{
////		LCD_Clear(WHITE);//Clear screen
//			TP_Adjust();  	//Screen calibration
//			TP_Save_Adjdata();
//		}
		TP_Get_Adjdata();
//	}
//	return 1;
}

//void TP_Adjust()
//{
//	uint16_t pos_temp[4][2] =
//	{ 0 };
//	uint8_t cnt = 0;
//	uint16_t d1, d2;
//	uint32_t tem1, tem2;
//	double fac;
//	uint16_t outtime = 0;
//	uint8_t touched = 0; // افزودن فلگ برای تشخیص لمس جدید
//
//	printf("Calibration started!\n");
//	UI_ShowMessage("Calibration started!");
//
//	osDelay(3000);
//
//// 	cnt=0;
////	POINT_COLOR=BLUE;
////	BACK_COLOR =WHITE;
////	LCD_Clear(WHITE);//Clear screen
////	POINT_COLOR=RED;//red
////	LCD_Clear(WHITE);//Clear screen
////	POINT_COLOR=BLACK;
////	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//Show message
//	printf("Start Calibration!!\n");
//	UI_ShowMessage(
//			"Please use the stylus to click on the cross on the screen. The cross will keep moving until the screen adjustment is complete.");
////	TP_Drow_Touch_Point(20,20,RED);//Draw point 1
//	UI_ShowButton(1);
//	tp_dev.sta = 0;				   //Eliminate the trigger signal
//	tp_dev.xfac = 0;//xfac is used to mark whether it has been calibrated, so it must be cleared before calibration! to avoid errors
//	while (1)//If it is not pressed for 10 seconds, it will automatically exit
//	{
////		tp_dev.scan(1);				   //Scan physical coordinates
////		if((tp_dev.sta&0xc000)==TP_CATH_PRES)//The button was pressed once (the button is now released.)
//		if (TP_Scan(1))
////		if ((tp_dev.sta & 0xC000) == TP_CATH_PRES && !touched)
//		{
//			touched = 1; // علامتگذاری لمس
//			outtime = 0;
//			tp_dev.sta &= ~TP_CATH_PRES;//The mark button has been processed.
//
//			pos_temp[cnt][0] = tp_dev.x[0];
//			pos_temp[cnt][1] = tp_dev.y[0];
//			cnt++;
//			touched = 1;
//			switch (cnt)
//			{
//			case 1:
//				UI_HideButton(1);
//				UI_ShowButton(2);
////					TP_Drow_Touch_Point(20,20,WHITE);				//Clear point 1
////					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//Draw point 2
//				if (showCallback)
//				{
//					showCallback(calibPoints[cnt].x, calibPoints[cnt].y,
//							showContext);
//					printf("Showing point %d: (%d, %d)\n", cnt + 1,
//							calibPoints[cnt].x, calibPoints[cnt].y);
//				}
//				break;
//			case 2:
//				UI_HideButton(2);
//				UI_ShowButton(3);
////						TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//Clear point 2
////					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//Draw point 3
//				if (showCallback)
//				{
//					showCallback(calibPoints[cnt].x, calibPoints[cnt].y,
//							showContext);
//					printf("Showing point %d: (%d, %d)\n", cnt + 1,
//							calibPoints[cnt].x, calibPoints[cnt].y);
//				}
//				break;
//			case 3:
//				UI_HideButton(3);
//				UI_ShowButton(4);
////						TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//Clear point 3
////						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//Draw point 4
//				if (showCallback)
//				{
//					showCallback(calibPoints[cnt].x, calibPoints[cnt].y,
//							showContext);
//					printf("Showing point %d: (%d, %d)\n", cnt + 1,
//							calibPoints[cnt].x, calibPoints[cnt].y);
//				}
//				break;
//			case 4:	 //All four points have been obtained
//
//				//Equal opposite sides
//				tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);	 //x1-x2
//				tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);	 //y1-y2
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d1 = sqrt(tem1 + tem2);	 //Get the distance of 1,2
//
//				tem1 = abs(pos_temp[2][0] - pos_temp[3][0]);	 //x3-x4
//				tem2 = abs(pos_temp[2][1] - pos_temp[3][1]);	 //y3-y4
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d2 = sqrt(tem1 + tem2);	 //Get the distance of 3,4
//				fac = (float) d1 / d2;
//
//				if (fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0)	//Unqualified
//				{
//					cnt = 0;
//					UI_HideButton(4);
//					UI_ShowButton(1);
//					//	    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
//					//			TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
//					//			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
//					printf("Calibration failed: Invalid points!\n");
//					continue;
//				}
//
//				tem1 = abs(pos_temp[0][0] - pos_temp[2][0]);			//x1-x3
//				tem2 = abs(pos_temp[0][1] - pos_temp[2][1]);			//y1-y3
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d1 = sqrt(tem1 + tem2);				//Get the distance of 1,3
//
//				tem1 = abs(pos_temp[1][0] - pos_temp[3][0]);			//x2-x4
//				tem2 = abs(pos_temp[1][1] - pos_temp[3][1]);			//y2-y4
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d2 = sqrt(tem1 + tem2);				//Get the distance of 2,4
//				fac = (float) d1 / d2;
//				if (fac < 0.95 || fac > 1.05)					//Unqualified
//				{
//					cnt = 0;
//					UI_HideButton(4);
//					UI_ShowButton(1);
//					//	    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
//					//			TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
//					//			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
//					printf("Calibration failed: Invalid points!\n");
//					continue;
//				}					//Correct
//
//				//Diagonal equal
//				tem1 = abs(pos_temp[1][0] - pos_temp[2][0]);			//x1-x3
//				tem2 = abs(pos_temp[1][1] - pos_temp[2][1]);			//y1-y3
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d1 = sqrt(tem1 + tem2);					//Get a distance of 1,4
//
//				tem1 = abs(pos_temp[0][0] - pos_temp[3][0]);			//x2-x4
//				tem2 = abs(pos_temp[0][1] - pos_temp[3][1]);			//y2-y4
//				tem1 *= tem1;
//				tem2 *= tem2;
//				d2 = sqrt(tem1 + tem2);				//Get the distance of 2,3
//				fac = (float) d1 / d2;
//				if (fac < 0.95 || fac > 1.05)					//Unqualified
//				{
//					cnt = 0;
//					UI_HideButton(4);
//					UI_ShowButton(1);
////								TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
////								TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
////								TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
//					printf("Calibration failed: Invalid points!\n");
//					continue;
//				}					//Correct
//
//				//Calculation results
//				tp_dev.xfac = (float) (tp_dev.width - 40)
//						/ (pos_temp[1][0] - pos_temp[0][0]);		//Get xfac
//				tp_dev.xoff = (tp_dev.width
//						- tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;	//Get xoff
//
//				tp_dev.yfac = (float) (tp_dev.height - 40)
//						/ (pos_temp[2][1] - pos_temp[0][1]);		//Get yfac
//				tp_dev.yoff = (tp_dev.height
//						- tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;	//Get yoff
//				if (abs(tp_dev.xfac) > 2 || abs(tp_dev.yfac) > 2)//The touch screen is the opposite of the preset.
//				{
//					cnt = 0;
//					UI_HideButton(4);
//					UI_ShowButton(1);
////								TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
////								TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
//					//		LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
//					printf("TP Need readjust!\n");
//					tp_dev.touchtype = !tp_dev.touchtype;//Modify the touch screen type.
//					if (tp_dev.touchtype)//X, Y direction is opposite to the screen
//					{
//						CMD_RDX = 0X90;
//						CMD_RDY = 0XD0;
//					}
//					else		//X and Y directions are the same as the screen
//					{
//						CMD_RDX = 0XD0;
//						CMD_RDY = 0X90;
//					}
//					continue;
//				}
//
//				//	POINT_COLOR=BLUE;
//				//	LCD_Clear(WHITE);//Clear screen
//				//	LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//Calibration completed
//				UI_ShowMessage("Touch Screen Adjust OK!");
//				printf("Touch Screen Adjust OK!\n");
//				delay_ms(1000);
//				TP_Save_Adjdata();
//				//	LCD_Clear(WHITE);//Clear screen
//				return;					//Calibration completed
//			}
//
//            // منتظر رها شدن لمس
//            while (TP_Scan(1)) { osDelay(10); }
//            touched = 0;
//		}
//
//        // اگر لمس رها شد، فلگ را ریست کنید
////        if (!(tp_dev.sta & 0xC000) && touched) {
////            touched = 0;
////        }
//
//
//		delay_ms(10);
//		outtime++;
//		if (outtime > 5000)
//		{
//			UI_ShowMessage(" ");
//			TP_Get_Adjdata();
//			break;
//		}
//
//	}
//}

void TP_Adjust()
{
	static uint16_t pos_temp[4][2] =
	{ 0 };
	static uint8_t cnt = 0;
	static uint16_t d1, d2 = 0;
	static uint32_t tem1, tem2 = 0;
	static double fac = 0;
	static uint16_t outtime = 0;
	static uint8_t touched = 0; // فلگ تشخیص لمس جدید

	static uint8_t calibDone = 0;

	printf("Calibration started!\n");
	UI_ShowMessage("Calibration started!");

	osDelay(3000);

//	printf("Start Calibration!!\n");
	UI_ShowMessage(
			"Please use the stylus to click on the cross on the screen. The cross will keep moving until the screen adjustment is complete.");

	osDelay(3000);
	UI_ShowButton(1);
	tp_dev.sta = 0;
	tp_dev.xfac = 0;

	while (1)
	{
		// نمایش نقطه فعلی و انتظار برای لمس
		switch (cnt)
		{
		case 0:
			UI_ShowButton(1);
			UI_ShowMessage("Please touch the red point on the top left.");
			break;
		case 1:
			UI_ShowButton(2);
			UI_ShowMessage("Please touch the red point on the top right.");
			break;
		case 2:
			UI_ShowButton(3);
			UI_ShowMessage("Please touch the red point on the bottom left.");
			break;
		case 3:
			UI_ShowButton(4);
			UI_ShowMessage("Please touch the red point on the bottom right.");
			break;
		}

		// انتظار برای لمس معتبر
		touched = 0;
		while (!touched)
		{
			tp_dev.scan(1);
			if (TP_Scan(1))
			{
				touched = 1;
				outtime = 0;
				pos_temp[cnt][0] = tp_dev.x[0];
				pos_temp[cnt][1] = tp_dev.y[0];
				printf("Point %d: X=%d, Y=%d\n", cnt + 1, pos_temp[cnt][0],
						pos_temp[cnt][1]);

				// انتظار برای رها شدن لمس
				while (TP_Scan(1))
				{
					osDelay(10);
					outtime = 0; // ریست تایم‌اوت در حین لمس
				}

				cnt++;
				if (cnt >= 4)
					break; // خروج اگر 4 نقطه ثبت شد
			}

			osDelay(10);
			outtime++;
			if (outtime > 5000)
			{
				UI_ShowMessage("Calibration timeout!");
				osDelay(1000);
				TP_Get_Adjdata();
				return;
			}
		}
		if (cnt >= 4)	 //All four points have been obtained
		{
			//Equal opposite sides
			tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);	 //x1-x2
			tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);	 //y1-y2
			tem1 *= tem1;
			tem2 *= tem2;
			d1 = sqrt(tem1 + tem2);	 //Get the distance of 1,2

			tem1 = abs(pos_temp[2][0] - pos_temp[3][0]);	 //x3-x4
			tem2 = abs(pos_temp[2][1] - pos_temp[3][1]);	 //y3-y4
			tem1 *= tem1;
			tem2 *= tem2;
			d2 = sqrt(tem1 + tem2);	 //Get the distance of 3,4
			fac = (float) d1 / d2;

			if (fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0)	//Unqualified
			{
				cnt = 0;
				UI_ShowMessage("Calibration failed! Try again...");
//					UI_HideButton(4);
//					UI_ShowButton(1);
				//	    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
				//			TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
				//			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
				printf("Calibration failed: Invalid points!\n");
				osDelay(1000);
				continue;
			}

			tem1 = abs(pos_temp[0][0] - pos_temp[2][0]);			//x1-x3
			tem2 = abs(pos_temp[0][1] - pos_temp[2][1]);			//y1-y3
			tem1 *= tem1;
			tem2 *= tem2;
			d1 = sqrt(tem1 + tem2);				//Get the distance of 1,3

			tem1 = abs(pos_temp[1][0] - pos_temp[3][0]);			//x2-x4
			tem2 = abs(pos_temp[1][1] - pos_temp[3][1]);			//y2-y4
			tem1 *= tem1;
			tem2 *= tem2;
			d2 = sqrt(tem1 + tem2);				//Get the distance of 2,4
			fac = (float) d1 / d2;
			if (fac < 0.95 || fac > 1.05)					//Unqualified
			{
				cnt = 0;
				UI_ShowMessage("Calibration failed! Try again...");
//					UI_HideButton(4);
//					UI_ShowButton(1);
				//	    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
				//			TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
				//			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
				printf("Calibration failed: Invalid points!\n");
				osDelay(1000);
				continue;
			}					//Correct

			//Diagonal equal
			tem1 = abs(pos_temp[1][0] - pos_temp[2][0]);			//x1-x3
			tem2 = abs(pos_temp[1][1] - pos_temp[2][1]);			//y1-y3
			tem1 *= tem1;
			tem2 *= tem2;
			d1 = sqrt(tem1 + tem2);					//Get a distance of 1,4

			tem1 = abs(pos_temp[0][0] - pos_temp[3][0]);			//x2-x4
			tem2 = abs(pos_temp[0][1] - pos_temp[3][1]);			//y2-y4
			tem1 *= tem1;
			tem2 *= tem2;
			d2 = sqrt(tem1 + tem2);				//Get the distance of 2,3
			fac = (float) d1 / d2;
			if (fac < 0.95 || fac > 1.05)					//Unqualified
			{
				cnt = 0;
				UI_ShowMessage("Calibration failed! Try again...");
//					UI_HideButton(4);
//					UI_ShowButton(1);
//								TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
//								TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
//								TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êý¾Ý
				printf("Calibration failed: Invalid points!\n");
				osDelay(1000);
				continue;
			}					//Correct

			//Calculation results
			tp_dev.xfac = (float) (tp_dev.width - 40)
					/ (pos_temp[1][0] - pos_temp[0][0]);		//Get xfac
			tp_dev.xoff = (tp_dev.width
					- tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;	//Get xoff

			tp_dev.yfac = (float) (tp_dev.height - 40)
					/ (pos_temp[2][1] - pos_temp[0][1]);		//Get yfac
			tp_dev.yoff = (tp_dev.height
					- tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;	//Get yoff
			if (abs(tp_dev.xfac) > 2 || abs(tp_dev.yfac) > 2)//The touch screen is the opposite of the preset.
			{
				cnt = 0;
//					UI_HideButton(4);
//					UI_ShowButton(1);
				UI_ShowMessage("Calibration failed! Try again...");
//								TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Clear point 4
//								TP_Drow_Touch_Point(20,20,RED);								//Draw point 1
				//		LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
				printf("TP Need readjust!\n");
				osDelay(1000);
				tp_dev.touchtype = !tp_dev.touchtype;//Modify the touch screen type.
				if (tp_dev.touchtype)//X, Y direction is opposite to the screen
				{
					CMD_RDX = 0X90;
					CMD_RDY = 0XD0;
				}
				else		//X and Y directions are the same as the screen
				{
					CMD_RDX = 0XD0;
					CMD_RDY = 0X90;
				}
				continue;
			}

			//	POINT_COLOR=BLUE;
			//	LCD_Clear(WHITE);//Clear screen
			//	LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//Calibration completed
			UI_ShowMessage("Calibration was successful.");
			printf("Touch Screen Adjust OK!\n");
			delay_ms(1000);
			TP_Save_Adjdata();
			calibDone = 1;
			//	LCD_Clear(WHITE);//Clear screen
//			return;//Calibration completed
			if (calibDone == 1)
			{
				//		TP_Init();
				UI_ShowMessage("The system is now restarting...");
				printf("The system is now restarting...\n");
				osDelay(2000);
				HAL_NVIC_SystemReset();
				//		calibDone = 0;
			}
		}

		// منتظر رها شدن لمس
		while (TP_Scan(1))
		{
			osDelay(10);
		}
		touched = 0;
	}

	// اگر لمس رها شد، فلگ را ریست کنید
	if (!(tp_dev.sta & 0xC000) && touched)
	{
		touched = 0;
	}

	delay_ms(10);
	outtime++;
	if (outtime > 5000)
	{
		UI_ShowMessage(" ");
		TP_Get_Adjdata();
		return;
	}
}

//void TP_Adjust()
//{
//    uint16_t pos_temp[4][2] = {0};
//    uint8_t cnt = 0;
//    uint16_t d1, d2;
//    uint32_t tem1, tem2;
//    double fac;
//    uint16_t outtime = 0;
//    uint8_t touched = 0; // فلگ تشخیص لمس جدید
//
//    printf("Calibration started!\n");
//    UI_ShowMessage("Calibration started!");
//
//    osDelay(3000);
//
//    printf("Start Calibration!!\n");
//    UI_ShowMessage(
//        "Please use the stylus to click on the cross on the screen. The cross will keep moving until the screen adjustment is complete.");
//
//    UI_ShowButton(1);
//    tp_dev.sta = 0;
//    tp_dev.xfac = 0;
//
//    while (1)
//    {
//        // نمایش نقطه فعلی و انتظار برای لمس
//        switch (cnt)
//        {
//        case 0:
//            UI_ShowButton(1);
//            break;
//        case 1:
//            UI_ShowButton(2);
//            break;
//        case 2:
//            UI_ShowButton(3);
//            break;
//        case 3:
//            UI_ShowButton(4);
//            break;
//        }
//
//        // انتظار برای لمس معتبر
//        touched = 0;
//        while (!touched)
//        {
//            tp_dev.scan(1);
//            if (TP_Scan(1))
//            {
//                touched = 1;
//                outtime = 0;
//                pos_temp[cnt][0] = tp_dev.x[0];
//                pos_temp[cnt][1] = tp_dev.y[0];
//                printf("Point %d: X=%d, Y=%d\n", cnt+1, pos_temp[cnt][0], pos_temp[cnt][1]);
//
//                // انتظار برای رها شدن لمس
//                while (TP_Scan(1))
//                {
//                    osDelay(10);
//                    outtime = 0; // ریست تایم‌اوت در حین لمس
//                }
//
//                cnt++;
//                if (cnt >= 4) break; // خروج اگر 4 نقطه ثبت شد
//            }
//
//            osDelay(10);
//            outtime++;
//            if (outtime > 5000)
//            {
//                UI_ShowMessage("Calibration timeout!");
//                TP_Get_Adjdata();
//                return;
//            }
//        }
//
//        // پردازش پس از ثبت 4 نقطه
//        if (cnt >= 4)
//        {
//            // محاسبات اعتبارسنجی نقاط
//            tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);
//            tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);
//            d1 = sqrt(tem1*tem1 + tem2*tem2);
//
//            tem1 = abs(pos_temp[2][0] - pos_temp[3][0]);
//            tem2 = abs(pos_temp[2][1] - pos_temp[3][1]);
//            d2 = sqrt(tem1*tem1 + tem2*tem2);
//            fac = (float)d1 / d2;
//
//            if (fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0)
//            {
//                // خطا: نقاط نامعتبر
//                cnt = 0;
//                UI_ShowMessage("Calibration failed! Retry...");
//                continue;
//            }
//
//            // محاسبه ضرایب کالیبراسیون
//            tp_dev.xfac = (float)(tp_dev.width - 40) / (pos_temp[1][0] - pos_temp[0][0]);
//            tp_dev.xoff = (tp_dev.width - tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;
//
//            tp_dev.yfac = (float)(tp_dev.height - 40) / (pos_temp[2][1] - pos_temp[0][1]);
//            tp_dev.yoff = (tp_dev.height - tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;
//
//            // ذخیره داده‌ها و خروج
//            TP_Save_Adjdata();
//            UI_ShowMessage("Calibration successful!");
//            printf("Calibration successful!\n");
//            return;
//        }
//    }
//}

// ارسال فرمان نمایش باتن
void UI_ShowButton(uint8_t button_id)
{
	// در تسک فرستنده
	UI_Command_t cmd;
	cmd.cmdType = SHOW_BUTTON; // یا HIDE_BUTTON
	cmd.data.buttonID = button_id; // شماره دکمه (1-4)

	// ارسال فرمان
	if (osMessageQueuePut(uiQueueHandle, &cmd, 0, osWaitForever) != osOK)
	{
		printf("UI_HideButton feild!\n");
	}
}

// ارسال فرمان مخفی‌سازی باتن
void UI_HideButton(uint8_t button_id)
{
	// در تسک فرستنده
	UI_Command_t cmd;
	cmd.cmdType = HIDE_BUTTON; // یا HIDE_BUTTON
	cmd.data.buttonID = button_id; // شماره دکمه (1-4)

	// ارسال فرمان
	if (osMessageQueuePut(uiQueueHandle, &cmd, 0, osWaitForever) != osOK)
	{
		printf("UI_HideButton feild!\n");
	}
}

// ارسال پیام به textArea
void UI_ShowMessage(const char *message)
{
	// در تسک فرستنده
	UI_Command_t cmd;
	cmd.cmdType = UPDATE_TEXT;

	// کپی رشته به ساختار (با اطمینان از عدم overflow)
	snprintf(cmd.data.text, sizeof(cmd.data.text), message);

	// ارسال به صف
	if (osMessageQueuePut(uiQueueHandle, &cmd, 0, osWaitForever) != osOK)
	{
		printf("UI_ShowMessage feild!\n");
	}
}
