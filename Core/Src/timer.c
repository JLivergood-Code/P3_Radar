/*
 * timer.c
 *
 *  Created on: Dec 6, 2024
 *      Author: pengu
 */

#include "main.h"
#include "timer.h"

// ARR val for 60 ms
#define ARR_VAL 9600000

void timer_init(void) {
// TIMER 5 Setup, calculates PWM
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

	// 20msec arr
	TIM2->ARR = ARR_VAL;

	// enables interrupts
	TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC4IE);
	NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));


	// clear the interrupt status register for Update event
	TIM2->SR &= ~(TIM_SR_UIF);

	TIM2->CCR4 = ARR_VAL>>1;

	// enables timer
	TIM2->CR1 |= TIM_CR1_CEN;

	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM2_STOP;

}
