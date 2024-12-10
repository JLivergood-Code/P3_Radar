/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ultrasonic.h"
#include "usart.h"
#include "servo.h"
#include "timer.h"

// offset for 1.5 seconds
#define MSEC_OFFSET 120000
#define MSEC_CNT 80000
#define MAX_ANGLE 90

void servo_degrees(int8_t degrees);
void SystemClock_Config(void);

uint32_t pos_edge = 0;
uint32_t neg_edge = 0;
uint8_t ultra_flag = 0;
uint32_t width = 0;
int8_t servo_pos = (-1 * MAX_ANGLE);
int8_t servo_mod = 1;

uint8_t clear_flag = 0;

uint32_t PWM_VAL;

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  ultrasonic_init();
  usart_init();
  usart_gpio_init();
  servo_init();
  timer_init();
  PWM_VAL = MSEC_CNT;
  TIM5->CCR1 = PWM_VAL;

  clear_scrn();
  write_divider();

  timer_init();

  __enable_irq();

  uint32_t distance = 0;



  while (1)
  {
	  if(ultra_flag){;
		  ultra_flag = 0;
		  width = width/2320;
		  if(servo_pos == MAX_ANGLE) {

			  clear_scrn();
			  write_divider();

		  }

		  plot_point(width, servo_pos);
	  }
//	  delay_us(2000);


//	  delay_us(200);
  }

}

void servo_degrees(int8_t degrees){
	// calculates PWN count
	PWM_VAL = ((MSEC_CNT * degrees) / 90) + MSEC_OFFSET;
	TIM4->CCR1 = PWM_VAL;

}

void TIM2_IRQHandler(void){
	// Activates every 60 ms
	if(TIM2->SR & TIM_SR_UIF){
		ultrasonic_trig();
		TIM2->CCR1 = (TIM2->ARR)>>1;

		servo_pos += servo_mod;

		if(servo_pos == MAX_ANGLE){
			servo_mod = -1;
		}
		if(servo_pos == (-1 * MAX_ANGLE)){
			servo_mod = 1;
		}

		servo_degrees(servo_pos);
//

		// clears UIF interrupt
		TIM2->SR &= ~(TIM_SR_UIF);
	}
	if(TIM2->SR & TIM_SR_CC1IF){
		ultrasonic_trig();
		TIM2->SR &= ~(TIM_SR_CC1IF);
	}
}

void TIM5_IRQHandler(void) {
	if(TIM5->SR & TIM_SR_UIF){
		// turns pin on
		GPIOC->ODR |= GPIO_ODR_OD3;

		// sets CCR1 to PWM val
		TIM5->CCR1 = PWM_VAL;

		// clears UIF interrupt
		TIM5->SR &= ~(TIM_SR_UIF);
	}
	if(TIM5->SR & TIM_SR_CC1IF){
		// turns wire off
		GPIOC->ODR &= ~(GPIO_ODR_OD3);

		TIM5->SR &= ~(TIM_SR_CC1IF);
	}
}

void TIM3_IRQHandler(void) {
	//The TIMx_CCR1 register gets the value of the counter on the active transition.
	//rising edge detected
	// CC1OF is also set if at least two consecutive captures occurred whereas the flag was not cleared
	if(TIM3->SR & TIM_SR_CC1IF){ //check flag for capture \ compare
	  //clear flag

		pos_edge = TIM3->CCR1;  // Read the current captured value

		TIM3->SR &= ~(TIM_SR_CC1IF);
	}
	if(TIM3->SR & TIM_SR_CC2IF){
		neg_edge = TIM3->CCR2;

		if(pos_edge && neg_edge){
			if(neg_edge >= pos_edge){
				// if the current value is greater than the previous, the period is the difference between them
				width = neg_edge - pos_edge;

				ultra_flag = 1;
			}
			else{
				//if the current time is not greater than the previous time, we have either time-traveled or the timer has overflowed
				 width = (-1 - pos_edge) + neg_edge;
			}


			pos_edge = 0;  // Update prevTime for next period measurement
			neg_edge = 0;
		}

		TIM3->SR &= ~(TIM_SR_CC2IF);
	}
}

/* Configure SysTick Timer for use with delay_us function. This will break
 * break compatibility with HAL_delay() by disabling interrupts to allow for
 * shorter delay timing.
 */
void SysTick_Init(void){
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |	       // enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk);     // select CPU clock
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);      // disable interrupt,
                                                       // breaks HAL delay function
}

/* Delay function using the SysTick timer to count CPU clock cycles for more
 * precise delay timing. Passing a time of 0 will cause an error and result
 * in the maximum delay. Short delays are limited by the clock speed and will
 * often result in longer delay times than specified. @ 4MHz, a delay of 1us
 * will result in a delay of 10-15 us.
 */
void delay_us(const uint32_t time_us) {
    // set the counts for the specified delay
    SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
    SysTick->VAL = 0;                                      // clear the timer count
    SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);        // clear the count flag
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for the flag
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
