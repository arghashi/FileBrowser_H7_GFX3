/*
 * Buzzer.c
 *
 *  Created on: May 17, 2023
 *      Author: Nima
 */

#include "Buzzer.h"

#define	  _BUZZER_USE_FREERTOS	0

#if (_BUZZER_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define buzzer_delay(x)    osDelay(x)
#else
#define buzzer_delay(x)    HAL_Delay(x)
#endif



void BuzzerInit(buzzer_t *buzzer, GPIO_TypeDef *buzz_gpio, uint16_t buzz_pin)
{
	buzzer->buzz_gpio = buzz_gpio;
	buzzer->buzz_pin  = buzz_pin;

    GPIO_InitTypeDef  gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Pin   = buzz_pin;
    HAL_GPIO_Init(buzz_gpio, &gpio);
}
void BuzzerBeep(buzzer_t *buzzer, uint32_t Up_delay, uint32_t Down_delay, uint32_t Repeat)
{
	for(int i = 0 ; i < Repeat ;)
	{
		HAL_GPIO_TogglePin(buzzer->buzz_gpio, buzzer->buzz_pin);
		buzzer_delay(Up_delay);
		HAL_GPIO_TogglePin(buzzer->buzz_gpio, buzzer->buzz_pin);
		buzzer_delay(Down_delay);
		i++;
	}

}
