/*
 * usart.c
 *
 *  Created on: Oct 31, 2024
 *      Author: pengu
 */

#include "usart.h"

#define NUM_DECIMAL 2
#define TRUE 1
#define FALSE 0

#define NUMROWS 43
#define NUMCOLS 87




void usart_init(void){
	// calculates the baud rate
//	uint16_t baud_rate = CLK/RSCLK;

	// enables the clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

	//enables receive interrupt
//	USART2->CR1 |= (USART_CR1_RXNEIE);
	//sets over sampling to 16 bits
	USART2->CR1 &= ~(USART_CR1_OVER8);
	//sets stop bits to 00
	USART2->CR2 &= ~(USART_CR2_STOP);


	// sets baud rate
	USART2->BRR = BRR_CALC;

	// enables USART to enable
	USART2->CR1 |= USART_CR1_UE;

	//enables receiving and transmitting
	USART2->CR1 |= (USART_CR1_TE);

	// enable interrupt in NVIC
//	NVIC -> ISER[1] = (1 << (USART2_IRQn & 0x1F));

}

void usart_gpio_init(void){
	// enables GPIO clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// sets mode to alternate mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (0x2 << GPIO_MODER_MODE2_Pos | 0x2 << GPIO_MODER_MODE3_Pos);

	// sets OTYPE to Push pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);

	// sets OPSEED to maximum value
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);

	// sets alternate function to 7, which is USART
	GPIOA->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL2_Pos | 0x7 << GPIO_AFRL_AFSEL3_Pos);
}

void usart_print(char *in_string){
	// sets index to 0 and gets the first
	uint8_t i;
//	uint8_t cur_char = in_string[index];

	// while not at the end of the string
	for(i = 0; in_string[i] != 0; i++){
		// waits until transmit is finished
		while(!(USART2->ISR & USART_ISR_TXE));
		// loads the character into the transmit register
		USART2->TDR = in_string[i];
	}

}

void usart_esc(char *in_string){
	uint8_t i;
	uint8_t esc_code = 0x1b;

	// sends escape code character
	while(!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = esc_code;


	// sends rest of string
	for(i = 0; in_string[i] != 0; i++){
		// waits until transmit is finished
		while(!(USART2->ISR & USART_ISR_TXE));
		// loads the character into the transmit register
		USART2->TDR = in_string[i];
	}
}
void clear_scrn(void){
	while(!(USART2->ISR & USART_ISR_TXE));
	usart_esc("[2J");	// clears screen
	usart_esc("[H"); // resets cursor position
}

// Outputs divider
void write_divider(void){
	// makes cursor invisible
	usart_esc("[?25l");
	usart_esc("[1m");

	// prints top border
	usart_esc("[0;0H");
	for(int i = 0; i < NUMCOLS; i++) { usart_print("="); }

	// prints bottom border

	usart_esc("[43;0H");
	for(int i = 0; i < NUMCOLS; i++) { usart_print("="); }

	// prints left divider
	usart_esc("[2;87H");
	for(int i = 0; i <= NUMROWS - 2; i++){
		usart_print("|");
		usart_esc("[1B");
		usart_esc("[1D");
	}


	// prints right border
//	usart_esc("[2;87H");
//	for(int i = 0; i <= NUMROWS - 2; i++){
//		usart_print("|");
//		usart_esc("[1B");
//		usart_esc("[1D");
//	}

	usart_esc("[0m");

//                                |

	usart_esc("[43;43H");
	usart_print("0");
}

// takes in length and angle (in degrees {-60, 60}) and plots a point
void plot_point(uint16_t r, int16_t theta){
	char * str_point = malloc(10);

	float cos_val[] = {
		    1, 0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 0.9945, 0.9925,
		    0.9903, 0.9877, 0.9848, 0.9816, 0.9781, 0.9744, 0.9703, 0.9659,
		    0.9613, 0.9563, 0.9511, 0.9455, 0.9397, 0.9336, 0.9272, 0.9205,
		    0.9135, 0.9063, 0.8988, 0.891, 0.8829, 0.8746, 0.866, 0.8572,
		    0.848, 0.8387, 0.829, 0.8192, 0.809, 0.7986, 0.788, 0.7771,
		    0.766, 0.7547, 0.7431, 0.7314, 0.7193, 0.7071, 0.6947, 0.682,
		    0.6691, 0.6561, 0.6428, 0.6293, 0.6157, 0.6018, 0.5878, 0.5736,
		    0.5592, 0.5446, 0.5299, 0.515, 0.5, 0.4848, 0.4695, 0.454,
		    0.4384, 0.4226, 0.4067, 0.3907, 0.3746, 0.3584, 0.342, 0.3256,
		    0.309, 0.2924, 0.2756, 0.2588, 0.2419, 0.225, 0.2079, 0.1908,
		    0.1736, 0.1564, 0.1392, 0.1219, 0.1045, 0.0872, 0.0698, 0.0523,
		    0.0349, 0.0175, 0, -0.0175, -0.0349, -0.0523, -0.0698, -0.0872,
		    -0.1045, -0.1219, -0.1392, -0.1564, -0.1736, -0.1908, -0.2079, -0.225,
		    -0.2419, -0.2588, -0.2756, -0.2924, -0.309, -0.3256, -0.342, -0.3584,
		    -0.3746, -0.3907, -0.4067, -0.4226, -0.4384, -0.454, -0.4695, -0.4848,
		    -0.5, -0.515, -0.5299, -0.5446, -0.5592, -0.5736, -0.5878, -0.6018,
		    -0.6157, -0.6293, -0.6428, -0.6561, -0.6691, -0.682, -0.6947, -0.7071,
		    -0.7193, -0.7314, -0.7431, -0.7547, -0.766, -0.7771, -0.788, -0.7986,
		    -0.809, -0.8192, -0.829, -0.8387, -0.848, -0.8572, -0.866, -0.8746,
		    -0.8829, -0.891, -0.8988, -0.9063, -0.9135, -0.9205, -0.9272, -0.9336,
		    -0.9397, -0.9455, -0.9511, -0.9563, -0.9613, -0.9659, -0.9703, -0.9744,
		    -0.9781, -0.9816, -0.9848, -0.9877, -0.9903, -0.9925, -0.9945, -0.9962,
		    -0.9976, -0.9986, -0.9994, -0.9998, -1 };


	float sin_val[] = {
	        0, 0.0175, 0.0349, 0.0523, 0.0698, 0.0872, 0.1045, 0.1219, 0.1392,
	        0.1564, 0.1736, 0.1908,	0.2079, 0.225,0.2419, 0.2588, 0.2756,
	        0.2924, 0.309, 0.3256, 0.342, 0.3584, 0.3746, 0.3907, 0.4067,
	        0.4226, 0.4384, 0.454, 0.4695, 0.4848, 0.5, 0.515, 0.5299,
	        0.5446, 0.5592, 0.5736, 0.5878, 0.6018, 0.6157, 0.6293, 0.6428,
	        0.6561, 0.6691, 0.682, 0.6947, 0.7071, 0.7193, 0.7314, 0.7431,
	        0.7547, 0.766, 0.7771, 0.788, 0.7986, 0.809, 0.8192, 0.829,
	        0.8387, 0.848, 0.8572, 0.866, 0.8746, 0.8829, 0.891, 0.8988,
	        0.9063, 0.9135, 0.9205, 0.9272, 0.9336, 0.9397, 0.9455, 0.9511,
	        0.9563, 0.9613, 0.9659, 0.9703, 0.9744, 0.9781, 0.9816, 0.9848,
	        0.9877, 0.9903, 0.9925, 0.9945, 0.9962, 0.9976, 0.9986, 0.9994,
	        0.9998, 1, 0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 0.9945,
	        0.9925, 0.9903, 0.9877, 0.9848,  0.9816, 0.9781, 0.9744, 0.9703,
	        0.9659, 0.9613, 0.9563, 0.9511, 0.9455, 0.9397, 0.9336, 0.9272,
	        0.9205, 0.9135, 0.9063, 0.8988, 0.891, 0.8829, 0.8746, 0.866,
	        0.8572, 0.848, 0.8387, 0.829, 0.8192, 0.809, 0.7986, 0.788,
	        0.7771, 0.766, 0.7547, 0.7431, 0.7314, 0.7193, 0.7071, 0.6947,
	        0.682, 0.6691, 0.6561, 0.6428, 0.6293, 0.6157, 0.6018, 0.5878,
	        0.5736, 0.5592, 0.5446, 0.5299, 0.515, 0.5, 0.4848, 0.4695,
	        0.454, 0.4384, 0.4226, 0.4067, 0.3907, 0.3746, 0.3584, 0.342,
	        0.3256, 0.309, 0.2924, 0.2756, 0.2588, 0.2419, 0.225, 0.2079,
	        0.1908, 0.1736, 0.1564, 0.1392, 0.1219, 0.1045, 0.0872, 0.0698,
	        0.0523, 0.0349, 0.0175, 0 };

	uint8_t x = round(r * cos_val[theta+90])  + (NUMCOLS>>1);
	// gets the y coordinate by converting r and theta into rectangle, and shifting over by
	// the number of columns/2 to adjust for USART cursor coordinates
	uint8_t y = NUMROWS - round(r * sin_val[theta+90]);


//	usart_esc(str_point);

//	// clears column


	// plots point
	sprintf(str_point, "[%d;%dH", y, x);
	usart_esc(str_point);

	usart_print("#");

	free(str_point);
}

// prints an integer to the terminal
void write_terminal_int(uint32_t in_val, char *valType, uint8_t isFlt){
	 // largest value is 65534 digits + 1 for safety
	 char *int_str = malloc(7);
	 intToStr(in_val, int_str, isFlt);

	 // ensures transmition register is empty when sending
	  while(!(USART2->ISR & USART_ISR_TXE));
	  usart_print(valType);
	  usart_print(int_str);


	  free(int_str);

}

// Algorithm found on Geeks for Geeks
/* partially copied and modified to imporve for assignment performance
 * https://www.geeksforgeeks.org/how-to-convert-an-integer-to-a-string-in-c/#1-manual-conversion-using-loop
 */
void intToStr(uint32_t N, char *str, uint8_t isFlt) {
	uint16_t i = 0;
//	uint8_t numLen = strlen(str);

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // while there is still a value left in N to compute
    while (N > 0) {
    	// adds a decimal point
    	if(isFlt && i == NUM_DECIMAL) { str[i++] = '.'; }
        // Convert integer digit to character and store
      	// it in the str
        str[i++] = N % 10 + '0';
      	N /= 10;
    }

    //ads leading zeros and decimal points
    if(i < NUM_DECIMAL){
    	str[i++] = '0';
    }
    if(isFlt && i == NUM_DECIMAL) {
    	str[i++] = '.';
    	str[i++] = '0';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}
