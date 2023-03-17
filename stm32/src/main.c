/*
 * ECE 153B - Winter 2023
 *
 * sLight of Hand by Matthew Nguyen & Ethan Epp
 */
 
#include <stdio.h> 

#include "stm32l476xx.h"
#include "SysClock.h"
#include "UART.h"
#include <string.h>

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;

void Input_Capture_Setup() {
	// Echo connected to PA0

	// Enable GPIO Port A Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// PB6 to Alternative Function Mode
	GPIOA->MODER &= ~(3UL);
	GPIOA->MODER |= (2UL);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL0_1); // Alternative function TIM5_CH1
	GPIOA->PUPDR &= ~(3UL); // No PUPD
	

	// Enable timer 5
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	// Timer set-up
	TIM5->PSC = 15; // Prescaler value
	TIM5->CR1 |= TIM_CR1_ARPE; // enable auto-reload preload
	TIM5->ARR |= TIM_ARR_ARR; // max value

	// Input capture
	TIM5->CCMR1 &= ~(TIM_CCMR1_CC1S); // clear Capture/Compare 1 bits
	TIM5->CCMR1 |= TIM_CCMR1_CC1S_0; // input capture mapped to timer input 1

	// capture/compare enable register
	TIM5->CCER |= (TIM_CCER_CC1NP) | (TIM_CCER_CC1P) | (TIM_CCER_CC1E); // capture both rising/falling edges & enable capturing

	// DMA/Interrupt Enable Register
	TIM5->DIER |= (TIM_DIER_UIE) | (TIM_DIER_CC1IE) | (TIM_DIER_CC1DE); // enable update interrupt, & interrupt/dma requests

	// Event generation register
	TIM5->EGR |= TIM_EGR_UG; // Enable update generation
	
	// Clear update interupt flag
	TIM5->EGR &= ~(TIM_EGR_CC1G); // Clear event flag
		
	// Enable upcounting
	TIM5->CR1 &= ~(TIM_CR1_DIR); // Upcounter
	TIM5->CR1 |= TIM_CR1_CEN; // Enable Counter

	// Enable Interupt
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_SetPriority(TIM5_IRQn, 2);
}

void TIM5_IRQHandler(void) {
	// [TODO]	
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

void Trigger_Setup() {
	// Trigger connected to PA9

	// Enable GPIO Port A Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// PA9 to Alternative Function Mode
	GPIOA->MODER &= ~(3UL<<18);
	GPIOA->MODER |= (2UL<<18); // Set to alternative function mode
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9); // Mask alternative function register
	GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL9_0); // Alternative function TIM1_CH2

	
	GPIOA->PUPDR &= ~(3UL<<18); // No PUPD
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9); // Push-Pull output type
	GPIOA->OSPEEDR |= (3UL<<18); // Very High Output Speed

	// Enable TIM1_CH2 Clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	TIM1->PSC = 15; // Prescaler value
	TIM1->CR1 |= TIM_CR1_ARPE; // enable auto-reload preload
	TIM1->ARR |= TIM_ARR_ARR; // max value (??)
	
	TIM1->CCR2 = 10;

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC2CE); // Output compare mode clear

	TIM1->CCMR1 |= (TIM_CCMR1_OC2M); // PWM Mode 1

	TIM1->CCMR1 |= (TIM_CCMR1_OC2PE); // Enable output preload

	TIM1->CCER |= (TIM_CCER_CC2E); // Channel 2 Output enable

	// Timer 1 Break & Dead-time Register
	TIM1->BDTR |= (TIM_BDTR_MOE) | (TIM_BDTR_OSSR) | (TIM_BDTR_OSSI);

	// Event generation register
	TIM1->EGR |= TIM_EGR_UG; // Enable update generation

	// DMA/Interrupt register
	TIM1->DIER |= TIM_DIER_UIE; // enable update interrupt

	TIM1->SR &= ~(TIM_SR_UIF); // clear update interrupt flag

	//Enable upcounting
	TIM1->CR1 &= ~(TIM_CR1_DIR); // Upcounter
	TIM1->CR1 |= TIM_CR1_CEN; // Enable Counter
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

int main(void) {	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source

	// from uart lab
	// System_Clock_Init();

	// Initalize UART -- using USART1
	// Init_USARTx(1);

	// Input Capture Setup
	Input_Capture_Setup();
	
	// Trigger Setup
	Trigger_Setup();

	double distanceCm = 0;
	double distanceIn = 0;
	while(1) {
		// [TODO] Store your measurements on Stack
		if ((150 <= timeInterval) && (timeInterval <= 1480)) {
			// max pulse 25ms -> ~168 inches. limit to 1480 -> ~10 inches
			distanceCm = timeInterval / 58;
			distanceIn = timeInterval / 148;
			// possible idea: only send bluetooth signal if this condition is satisfied (to avoid junk data sent)
			// printf("Current Distance: %32d inches\n", distanceIn);
		} else {
			// object out of range
			distanceCm = 0;
			distanceIn = 0;
		}

	}
}
