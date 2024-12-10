/*
 * usarrt.h
 *
 *  Created on: Oct 31, 2024
 *      Author: pengu
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#define BRR_CALC (80000000 / 115200)

//#include "adc.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void usart_init(void);
void usart_gpio_init(void);

// functions to write to USART terminal
void usart_print(char *in_string);
void usart_esc(char *in_string);
void write_divider(void);
void plot_point(uint16_t r, int16_t theta);
void write_terminal_int(uint32_t in_val, char* valType, uint8_t isFlt);
void clear_scrn(void);
void intToStr(uint32_t N, char *str, uint8_t isFlt);


#endif /* INC_USART_H_ */
