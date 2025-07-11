/*
 * Buzzer.h
 *
 *  Created on: May 17, 2023
 *      Author: Nima
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "stm32h7xx_hal.h"


typedef struct
{
  GPIO_TypeDef  *buzz_gpio;
  uint16_t       buzz_pin;
}buzzer_t;



void BuzzerInit(buzzer_t *buzzer, GPIO_TypeDef *buzz_gpio, uint16_t buzz_pin);
void BuzzerBeep(buzzer_t *buzzer, uint32_t Up_delay, uint32_t Down_delay, uint32_t Repeat);


#endif /* INC_BUZZER_H_ */
