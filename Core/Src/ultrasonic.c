/*
 * ultrasonic.c
 *
 *  Created on: Nov 26, 2024
 *      Author: pengu
 *
 *  Designed for HC-SR04 Ultrasonic Sensor
 */

#include "main.h"
#include "ultrasonic.h"

#define MAXARR -1
#define TIM3_PRESC 0

void ultrasonic_init(void){
	/*
	 * GPIO Pins
	 * C6: Trigger
	 * A5: Echo -> Timer Input Captue
	 */

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// C5: output Mode; C6: Alternate Function
	GPIOC->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6);
	GPIOC->MODER |= (1 << GPIO_MODER_MODE5_Pos);
	GPIOC->MODER |= (0x2 << GPIO_MODER_MODE6_Pos);

	// both pins set with no PUPD, OTYPER and SPEEDR
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6);

	GPIOC->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL6_Pos);
	// ================FINISH GPIO SETUP =========================== \\

	// ================Timer Setup ================================= \\

	/* Timers:
	 *  TIM2 (32 bit):
	 *  	ARR = MAX Value
	 *  	Input Capture 1: Positive Edge
	 *  	Input Capture 2: Negative Edge
	 */

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	//this is a variable
	TIM3->ARR = MAXARR;

	// configure CC1 and CC2 to be set to TI1
	// both input captures should share the same signal

	/* Select edge conditions
	 * 	Input Capture 1 should be on positive edge
	 * 	Input Capture 2 should be on negative edge
	 *
	 * */
	// rising edge for CC1
	TIM3->CCER &= ~(TIM_CCER_CC1P_Msk | TIM_CCER_CC1NP_Msk );
	// falling edge for CC2
	TIM3->CCER |= (TIM_CCER_CC2P_Msk);

	/* Program the input prescaler.
	 * No prescaler needed for IC1 or IC2
	 * */
//	TIM4->CCMR2 &= ~(TIM_CCMR2_IC4PSC_Msk);

	// enable IC1 and IC2?
	// what does this do?
	TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
	TIM3->CCMR1 |= (0x1 << TIM_CCMR1_CC1S_Pos | 0x2 << TIM_CCMR1_CC2S_Pos);

	/* Enable capture from the counter into the capture register by setting the CC1E and CC2E bit in the
	TIMx_CCER register.*/
	TIM3->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

	// prescaler should be 0
	TIM3->PSC |= (TIM3_PRESC);

	// enable update event interrupt in TIM2
	TIM3->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE;

	// clear the interrupt status register for Update event
	TIM3->SR &= ~(TIM_SR_UIF);
	// start the timer
	TIM3->CR1 |= TIM_CR1_CEN;
	// enable TIM2 interrupt in NVIC
	NVIC->ISER[0] = (1 << TIM3_IRQn);

	// stops timer in debug mode
	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM3_STOP;

}

void ultrasonic_trig(void) {

	// turn trigger pin on
	GPIOC->ODR |= GPIO_ODR_OD5;
	// wait 10 micro seconds
	delay_us(10);
	// turn trigger pin off
	GPIOC->ODR &= ~(GPIO_ODR_OD5);
}


