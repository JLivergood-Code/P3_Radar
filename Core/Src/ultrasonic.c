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
	 * A6: Trigger
	 * A5: Echo -> Timer Input Captue
	 */

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// C5: output Mode; C6: Alternate Function
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6);
	GPIOA->MODER |= (1 << GPIO_MODER_MODE6_Pos);
	GPIOA->MODER |= (0x2 << GPIO_MODER_MODE5_Pos);

	// both pins set with no PUPD, OTYPER and SPEEDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6);

	GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL5_Pos);

	// SET IN TIMERS.C
	/* Timers:
	 *  TIM2 (32 bit):
	 *  	ARR = MAX Value
	 *  	Input Capture 1: Positive Edge
	 *  	Input Capture 2: Negative Edge
	 */

//	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;



	// stops timer in debug mode
//	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM3_STOP;

}

void ultrasonic_trig(void) {

	// turn trigger pin on
	GPIOA->ODR |= GPIO_ODR_OD6;
	// wait 10 micro seconds
	delay_us(10);
	// turn trigger pin off
	GPIOA->ODR &= ~(GPIO_ODR_OD6);
}


