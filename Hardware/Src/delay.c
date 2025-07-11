#include "delay.h"
#include "stm32h7xx.h"

void delayInit(void) {
#ifdef TIMER1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    CURRENT_TIMER->CR1 = 0; // ریست تنظیمات
#endif

#ifdef TIMER2
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
    CURRENT_TIMER->CR1 = 0;
#endif

#ifdef TIMER3
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
    CURRENT_TIMER->CR1 = 0;
#endif

#ifdef TIMER4
    RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;
    CURRENT_TIMER->CR1 = 0;
#endif

#ifdef TIMER5
    RCC->APB1LENR |= RCC_APB1LENR_TIM5EN;
    CURRENT_TIMER->CR1 = 0;
#endif
}

void delayDeInit(void) {
#ifdef TIMER1
    RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
#endif

#ifdef TIMER2
    RCC->APB1LENR &= ~RCC_APB1LENR_TIM2EN;
#endif

#ifdef TIMER3
    RCC->APB1LENR &= ~RCC_APB1LENR_TIM3EN;
#endif

#ifdef TIMER4
    RCC->APB1LENR &= ~RCC_APB1LENR_TIM4EN;
#endif

#ifdef TIMER5
    RCC->APB1LENR &= ~RCC_APB1LENR_TIM5EN;
#endif
}

void delay_ms(uint32_t delay) {
    CURRENT_TIMER->PSC = (CURRENT_FREQ / 1000) - 1; // فرکانس ۱kHz
    CURRENT_TIMER->ARR = delay; // مقدار تأخیر
    CURRENT_TIMER->EGR |= TIM_EGR_UG; // آپدیت رجیسترها
    CURRENT_TIMER->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM; // شروع تایمر + حالت تکپالس
    while ((CURRENT_TIMER->CR1 & TIM_CR1_CEN) != 0); // انتظار تا پایان تأخیر
}

void delay_us(uint32_t delay) {
    CURRENT_TIMER->PSC = (CURRENT_FREQ / 1000000) - 1; // فرکانس ۱MHz
    CURRENT_TIMER->ARR = delay; // مقدار تأخیر
    CURRENT_TIMER->EGR |= TIM_EGR_UG; // آپدیت رجیسترها
    CURRENT_TIMER->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM; // شروع تایمر + حالت تکپالس
    while ((CURRENT_TIMER->CR1 & TIM_CR1_CEN) != 0); // انتظار تا پایان تأخیر
}





///*
// * delay.c
// *
// *  Created on: 20 ãðóä. 2017 ð.
// *      Author: Andriy
// */
//
//#include <delay.h>
//#include "stm32h7xx.h"
//#include "main.h"
//
////extern TIM_HandleTypeDef htim4;
//
//// Ôóíêö³ÿ âìèêàííÿ òàéìåðó äëÿ ïîòðåá delay
//void delayInit(void)
//{
//#ifdef TIMER1
//	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
//#endif
//
//#ifdef TIMER2
//	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
//#endif
//
//#ifdef TIMER3
//	RCC->AHB1ENR |= RCC_APB1LENR_TIM3EN;
//#endif
//
//#ifdef TIMER4
//	RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;
//#endif
//}
//
//// Ôóíêö³ÿ âèìèêàííÿ òàéìåðó ÿê íåìàº ïîòðåáè â delay
//void delayDeInit(void)
//{
//#ifdef TIMER1
//	RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
//#endif
//
//#ifdef TIMER2
//	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
//#endif
//
//#ifdef TIMER3
//	RCC->AHB1ENR &= ~RCC_APB1LENR_TIM3EN;
//#endif
//
//#ifdef TIMER4
//	RCC->APB1LENR &= ~RCC_APB1LENR_TIM4EN;
//#endif
//}
//
////Ôóíêö³ÿ ôîðìóâàííÿ çàòðèìêè â ì³ë³ñåêóíäàõ
//void delay_ms(volatile uint32_t delay)
//{
//	CURRENT_TIMER->PSC = CURRENT_FREQ/1000-1; //Âñòàíîâëþºìî ïîäð³áíþâà÷
//	CURRENT_TIMER->ARR = delay; //âñòàíîâëþºìî çíà÷åííÿ ïåðåïîâíþâàííÿ òàéìåðó, à òàêîæ ³ çíà÷åííÿ ïðè ÿêîìó ãåíåðóåòüñÿ ïîä³ÿ îíîâëåííÿ
//	CURRENT_TIMER->EGR |= TIM_EGR_UG; //Ãåíåðèðóåìî Ïîä³þ îíîâëåííÿ äëÿ çàïèñó äàíèõ â ðåã³ñòðè PSC ³ ARR
//	CURRENT_TIMER->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM; //Çàïóñêàåìî òàéìåð çàïèñîì áèòó CEN ³ âñòàíîâëþºìî ðåæèì Îäíîãî ïðîõîäó âñòàíîâëåííÿì á³òó OPM
//	while ((CURRENT_TIMER->CR1) & (TIM_CR1_CEN!=0)); //Âèêîíóºìî öèêë ïîêè ðàõóº òàéìåð äî íóëÿ
//}
//
////Ôóíêö³ÿ ôîðìóâàííÿ çàòðèìêè â ì³êðîñåêóíäàõ
//void delay_us(volatile uint32_t delay)
//{
//    CURRENT_TIMER->PSC = CURRENT_FREQ/1000000-1; ///Âñòàíîâëþºìî ïîäð³áíþâà÷
//	CURRENT_TIMER->ARR = delay; //âñòàíîâëþºìî çíà÷åííÿ ïåðåïîâíþâàííÿ òàéìåðó, à òàêîæ ³ çíà÷åííÿ ïðè ÿêîìó ãåíåðóåòüñÿ ïîä³ÿ îíîâëåííÿ
//	CURRENT_TIMER->EGR |= TIM_EGR_UG; //Ãåíåðèðóåìî Ïîä³þ îíîâëåííÿ äëÿ çàïèñó äàíèõ â ðåã³ñòðè PSC ³ ARR
//	CURRENT_TIMER->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM; //Çàïóñêàåìî òàéìåð çàïèñîì áèòó CEN ³ âñòàíîâëþºìî ðåæèì Îäíîãî ïðîõîäó âñòàíîâëåííÿì á³òó OPM
//	while ((CURRENT_TIMER->CR1) & (TIM_CR1_CEN!=0)); //Âèêîíóºìî öèêë ïîêè ðàõóº òàéìåð äî íóëÿ
//}
//
////void delay_us (uint16_t us)
////{
////	__HAL_TIM_SET_COUNTER(&htim4,0);  // set the counter value a 0
////	while (__HAL_TIM_GET_COUNTER(&htim4) < us);  // wait for the counter to reach the us input in the parameter
////}
