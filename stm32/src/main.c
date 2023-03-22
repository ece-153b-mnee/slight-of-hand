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
	// Ultrasonic PWM initialization 
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

char strTime[12] = {0};
char strDate[12] = {0};

int main(void) {
	
	System_Clock_Init();   // System Clock = 80 MHz
	SysTick_Init();
	RTC_Init();
	RTC_Alarm_Enable();
	RTC_Set_Alarm();
	// Initialize Ultrasonic Sensor
	Init_Ultrasonic();

	// Initalize UART -- using USART1
	Init_USARTx(1);
	
	// Initialize LCD
	LCD_Setup_White();
	Get_RTC_Calendar(strTime,strDate);
	LCD_Set_Light();
	LCD_Set_Time(strTime);
	


	uint32_t distanceCm = 0;
	uint32_t distanceIn = 0;
	while(1) {
		Get_RTC_Calendar(strTime,strDate);
		LCD_Set_Time(strTime);
		// [TODO] Store your measurements on Stack
		if ((150 <= timeInterval) && (timeInterval <= 1600)) {
			// max pulse 25ms -> ~168 inches. limit to 1480 -> ~10 inches
			distanceCm = timeInterval / 58;
			distanceIn = timeInterval / 148;
			printf("%32u\n", distanceIn);
			LCD_Set_Brightness(distanceIn);
		} else {
			// object out of range
			distanceCm = 0;
			distanceIn = 0;
		}
	}
}
