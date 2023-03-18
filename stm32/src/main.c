/*
 * ECE 153B - Winter 2023
 *
 * sLight of Hand by Matthew Nguyen & Ethan Epp
 */
 
#include <stdio.h> 

#include "stm32l476xx.h"
#include "SysClock.h"
#include "UART.h"
#include "ultrasonic.h"
#include <string.h>
#include "SPI_template.h"
#include "MY_ILI9341_template.h"
#include "SysTimer.h"
#include "LCD_Print_Helpers.h"

// ultrasonic variables
uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;


// ultrasonic interrupt handler
void TIM5_IRQHandler(void) {	
	if ((TIM5->SR & TIM_SR_UIF) && ((GPIOA->IDR & GPIO_IDR_ID0) == 0)) {
		// overflow that occurs within pulse
		++overflowCount;
		TIM5->SR &= ~(TIM_SR_UIF); // reset overflow flag
	}
	if (TIM5->SR & TIM_SR_CC1IF) {
		if (GPIOA->IDR & GPIO_IDR_ID0) {
			// rising edge detected in pulse
			lastValue = TIM5->CCR1;
		} else {
			// falling edge
			currentValue = TIM5->CCR1;
			timeInterval = ((overflowCount * 0xFFFFUL) + currentValue) - lastValue;
			overflowCount = 0; // reset overflow count
		}
		TIM5->SR &= ~(TIM_SR_CC1IF); // reset input capture flag
	}	
}

void Init_Ultrasonic() {
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	// RCC->CR |= RCC_CR_HSION;
	// while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready

	// Select HSI as system clock source 
	// RCC->CFGR &= ~RCC_CFGR_SW;
	// RCC->CFGR |= RCC_CFGR_SW_HSI;
	// while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source

	Input_Capture_Setup();
	Trigger_Setup();

}
// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

// void LCD_Setup_White(){
// 	ILI9341_Init(SPI1,GPIOA,5,GPIOA,6,GPIOA,7);
// 	delay(750);
// 	ILI9341_setRotation(1);
// 	delay(750);
// 	ILI9341_Fill(COLOR_WHITE);
// 	delay(750);
// 	ILI9341_printText("sLight   of hand",25,16,COLOR_BLACK,COLOR_WHITE,2);
// 	delay(250);
// 	ILI9341_printText("Light",37,13,COLOR_YELLOW,COLOR_WHITE,3);
// 	delay(250);
// 	ILI9341_printText("10:30 AM",48,128,COLOR_BLACK,COLOR_WHITE,3);
// 	delay(250);
// 	ILI9341_printText("Brightness: ",24,224,COLOR_BLACK,COLOR_WHITE,2);
// 	delay(250);
// 	ILI9341_printText("100%",165,224,COLOR_BLACK,COLOR_WHITE,2);
// 	delay(250);
// 	ILI9341_printText("Ethan Epp",5,300,COLOR_BLACK,COLOR_WHITE,1);
// 	delay(250);
// 	ILI9341_printText("Matthew Nguyen",5,310,COLOR_BLACK,COLOR_WHITE,1);

// }

// void LCD_Set_Time(char text[]){
// 	delay(250);
// 	ILI9341_printText(text,48,128,COLOR_BLACK,COLOR_WHITE,3);
// }

// void LCD_Set_Brightness(char text[]){
// 	delay(250);
// 	ILI9341_printText(text,165,224,COLOR_BLACK,COLOR_WHITE,2);
// }


int main(void) {
	
	System_Clock_Init();   // System Clock = 80 MHz
	SysTick_Init();
	
	
	LCD_Setup_White();
	LCD_Set_Time("04:24 PM");
	LCD_Set_Brightness("69%");
	
	// from uart lab -> 80 mhz -> could probably delete
	// System_Clock_Init();
	
	// Initialize Ultrasonic Sensor
	Init_Ultrasonic();

	// Initalize UART -- using USART1
	Init_USARTx(1);

	uint32_t distanceCm = 0;
	uint32_t distanceIn = 0;
	while(1) {
		// [TODO] Store your measurements on Stack
		if ((150 <= timeInterval) && (timeInterval <= 1480)) {
			// max pulse 25ms -> ~168 inches. limit to 1480 -> ~10 inches
			distanceCm = timeInterval / 58;
			distanceIn = timeInterval / 148;
			// possible idea: only send bluetooth signal if this condition is satisfied (to avoid junk data sent)
			printf("%32u\n", distanceIn);
		} else {
			// object out of range
			distanceCm = 0;
			distanceIn = 0;
		}
				// printf("Test message\n");
	}
}
