/*
 * servo.c
 *
 *  Created on: Nov 27, 2024
 *      Author: pengu
 */

#include "main.h"
#include "servo.h"

#define ARR_VAL 1600000



void servo_init(void){
	/*
	 * GPIO setup
	 * PC3: PWN Out
	 */

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	GPIOC->MODER &= ~(GPIO_MODER_MODE3);
	GPIOC->MODER |= (1 << GPIO_MODER_MODE3_Pos);

	// both pins set with no PUPD, OTYPER and SPEEDR
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD3);
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT3);
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED3);

	// TIMER 5 Setup, calculates PWM
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	// 20msec arr
	TIM5->ARR = ARR_VAL;

	// enables interrupts
	TIM5->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);
	NVIC->ISER[1] = (1 << (TIM5_IRQn & 0x1F));


	// clear the interrupt status register for Update event
	TIM5->SR &= ~(TIM_SR_UIF);

	// enables timer
	TIM5->CR1 |= TIM_CR1_CEN;

	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM5_STOP;

}


