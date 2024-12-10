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
	TIM2->DIER |= (TIM_DIER_CC3IE | TIM_DIER_CC4IE);
//	NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));


	// clear the interrupt status register for Update event
	TIM2->SR &= ~(TIM_SR_UIF);

	TIM2->CCR4 = ARR_VAL>>1;



	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM2_STOP;

	// configure CC1 and CC2 to be set to TI1
	// both input captures should share the same signal

	/* Select edge conditions
	 * 	Input Capture 1 should be on positive edge
	 * 	Input Capture 2 should be on negative edge
	 *
	 * */
	// rising edge for CC1
	TIM2->CCER &= ~(TIM_CCER_CC1P_Msk | TIM_CCER_CC1NP_Msk );
	// falling edge for CC2
	TIM2->CCER |= (TIM_CCER_CC2P_Msk);

	/* Program the input prescaler.
	 * No prescaler needed for IC1 or IC2
	 * */
//	TIM4->CCMR2 &= ~(TIM_CCMR2_IC4PSC_Msk);

	// enable IC1 and IC2?
	// what does this do?
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
	TIM2->CCMR1 |= (0x1 << TIM_CCMR1_CC1S_Pos | 0x2 << TIM_CCMR1_CC2S_Pos);

	/* Enable capture from the counter into the capture register by setting the CC1E and CC2E bit in the
	TIMx_CCER register.*/
	TIM2->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

	// prescaler should be 0
	TIM2->PSC |= 0;

	// enable update event interrupt in TIM2
	TIM2->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE;

	// clear the interrupt status register for Update event
	TIM2->SR &= ~(TIM_SR_UIF);
	// enables timer
	TIM2->CR1 |= TIM_CR1_CEN;
	// enable TIM2 interrupt in NVIC
	NVIC->ISER[0] = (1 << TIM2_IRQn);

}
