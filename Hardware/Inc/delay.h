
//#ifndef TS_DELAY_H_
//#define TS_DELAY_H_
//
//#include <stdint.h>
//
//
////#define TIMER1
////#define TIMER2
////#define TIMER3
//#define TIMER4
//
//
//
//#ifdef TIMER1
//#define CURRENT_TIMER               ((TIM_TypeDef *) TIM1)
//#endif
//
//#ifdef TIMER2
//#define CURRENT_TIMER               ((TIM_TypeDef *) TIM2)
//#endif
//
//#ifdef TIMER3
//#define CURRENT_TIMER               ((TIM_TypeDef *) TIM3)
//#endif
//
//#ifdef TIMER4
//#define CURRENT_TIMER               ((TIM_TypeDef *) TIM4)
//#endif
//
//#define CURRENT_FREQ				SystemCoreClock
//
//void delayInit(void);
//void delayDeInit(void);
//void delay_ms(volatile uint32_t delay);
//void delay_us(volatile uint32_t delay);
//
////void delay_us(uint16_t delay);
//
//#endif /* TS_DELAY_H_ */


#ifndef TS_DELAY_H_
#define TS_DELAY_H_

#include <stdint.h>

// انتخاب تایمر ۳۲ بیتی برای پشتیبانی از تأخیرهای طولانی
//#define TIMER2
#define TIMER5 // برای تایمر ۳۲ بیتی دیگر

#ifdef TIMER1
#define CURRENT_TIMER               ((TIM_TypeDef *) TIM1)
#endif

#ifdef TIMER2
#define CURRENT_TIMER               ((TIM_TypeDef *) TIM2)
#endif

#ifdef TIMER3
#define CURRENT_TIMER               ((TIM_TypeDef *) TIM3)
#endif

#ifdef TIMER4
#define CURRENT_TIMER               ((TIM_TypeDef *) TIM4)
#endif

#ifdef TIMER5
#define CURRENT_TIMER               ((TIM_TypeDef *) TIM5)
#endif

#define CURRENT_FREQ			    SystemCoreClock // فرکانس هسته CPU

void delayInit(void);
void delayDeInit(void);
void delay_ms(uint32_t delay);
void delay_us(uint32_t delay);

#endif /* TS_DELAY_H_ */
