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

void System_Clock_Init(void){
	
	uint32_t HSITrim;

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
	// must be correctly programmed according to the frequency of the CPU clock
	// (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;
		
	// Enable the Internal High Speed oscillator (HSI
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR	
	HSITrim = 16; // user-programmable trimming value that is added to HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;
	
	RCC->CR    &= ~RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	
	// Select clock source to PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; // 00 = No clock, 01 = MSI, 10 = HSI, 11 = HSE
	
	// Make PLL as 80 MHz
	// f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 16MHz * 20/2 = 160 MHz
	// f(PLL_R) = f(VCO clock) / PLLR = 160MHz/2 = 80MHz
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << 8;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | 1U << 4; // 000: PLLM = 1, 001: PLLM = 2, 010: PLLM = 3, 011: PLLM = 4, 100: PLLM = 5, 101: PLLM = 6, 110: PLLM = 7, 111: PLLM = 8

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;  // 00: PLLR = 2, 01: PLLR = 4, 10: PLLR = 6, 11: PLLR = 8	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable Main PLL PLLCLK output 

	RCC->CR   |= RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	// Select PLL selected as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	
	// Wait until System Clock has been selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// The maximum frequency of the AHB, the APB1 and the APB2 domains is 80 MHz.
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1; SYSCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB high-speed prescaler (APB1) = 1, HCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB high-speed prescaler (APB2) = 1, HCLK not divided
	
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; 
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;	
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable Main PLL PLLSAI3CLK output enable
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // Enable Main PLL PLL48M1CLK output enable
	
	RCC->CR &= ~RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == RCC_CR_PLLSAI1ON );
	
	// Configure and enable PLLSAI1 clock to generate 11.294MHz 
	// 8 MHz * 24 / 17 = 11.294MHz
	// f(VCOSAI1 clock) = f(PLL clock input) *  (PLLSAI1N / PLLM)
	// PLLSAI1CLK: f(PLLSAI1_P) = f(VCOSAI1 clock) / PLLSAI1P
	// PLLUSB2CLK: f(PLLSAI1_Q) = f(VCOSAI1 clock) / PLLSAI1Q
	// PLLADC1CLK: f(PLLSAI1_R) = f(VCOSAI1 clock) / PLLSAI1R
	RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1N;
	RCC->PLLSAI1CFGR |= 24U<<8;
	
	// SAI1PLL division factor for PLLSAI1CLK
	// 0: PLLSAI1P = 7, 1: PLLSAI1P = 17
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1P;
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1PEN;
	
	// SAI1PLL division factor for PLL48M2CLK (48 MHz clock)
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1Q;
	// RCC->PLLSAI1CFGR |= U<<21;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1QEN;
	
	// PLLSAI1 division factor for PLLADC1CLK (ADC clock)
	// 00: PLLSAI1R = 2, 01: PLLSAI1R = 4, 10: PLLSAI1R = 6, 11: PLLSAI1R = 8
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1R; 
	// RCC->PLLSAI1CFGR |= U<<25;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
	
	RCC->CR |= RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == 0);
	
	// SAI1 clock source selection
	// 00: PLLSAI1 "P" clock (PLLSAI1CLK) selected as SAI1 clock
	// 01: PLLSAI2 "P" clock (PLLSAI2CLK) selected as SAI1 clock
	// 10: PLL "P" clock (PLLSAI3CLK) selected as SAI1 clock
	// 11: External input SAI1_EXTCLK selected as SAI1 clock	
	RCC->CCIPR &= ~RCC_CCIPR_SAI1SEL;

	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
}


void System_Clock_Init_LCD(void){
	
	uint32_t HSITrim;

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
	// must be correctly programmed according to the frequency of the CPU clock
	// (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;
		
	// Enable the Internal High Speed oscillator (HSI
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR	
	HSITrim = 16; // user-programmable trimming value that is added to HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;
	
	RCC->CR    &= ~RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	
	// Select clock source to PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; // 00 = No clock, 01 = MSI, 10 = HSI, 11 = HSE
	
	// Make PLL as 80 MHz
	// f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 16MHz * 20/2 = 160 MHz
	// f(PLL_R) = f(VCO clock) / PLLR = 160MHz/2 = 80MHz
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << 8;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | 1U << 4; // 000: PLLM = 1, 001: PLLM = 2, 010: PLLM = 3, 011: PLLM = 4, 100: PLLM = 5, 101: PLLM = 6, 110: PLLM = 7, 111: PLLM = 8

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;  // 00: PLLR = 2, 01: PLLR = 4, 10: PLLR = 6, 11: PLLR = 8	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable Main PLL PLLCLK output 

	RCC->CR   |= RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	// Select PLL selected as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	
	// Wait until System Clock has been selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// The maximum frequency of the AHB, the APB1 and the APB2 domains is 80 MHz.
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1; SYSCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB high-speed prescaler (APB1) = 1, HCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB high-speed prescaler (APB2) = 1, HCLK not divided
	
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; 
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;	
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable Main PLL PLLSAI3CLK output enable
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // Enable Main PLL PLL48M1CLK output enable
	
	RCC->CR &= ~RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == RCC_CR_PLLSAI1ON );
	
	// Configure and enable PLLSAI1 clock to generate 11.294MHz 
	// 8 MHz * 24 / 17 = 11.294MHz
	// f(VCOSAI1 clock) = f(PLL clock input) *  (PLLSAI1N / PLLM)
	// PLLSAI1CLK: f(PLLSAI1_P) = f(VCOSAI1 clock) / PLLSAI1P
	// PLLUSB2CLK: f(PLLSAI1_Q) = f(VCOSAI1 clock) / PLLSAI1Q
	// PLLADC1CLK: f(PLLSAI1_R) = f(VCOSAI1 clock) / PLLSAI1R
	RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1N;
	RCC->PLLSAI1CFGR |= 24U<<8;
	
	// SAI1PLL division factor for PLLSAI1CLK
	// 0: PLLSAI1P = 7, 1: PLLSAI1P = 17
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1P;
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1PEN;
	
	// SAI1PLL division factor for PLL48M2CLK (48 MHz clock)
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1Q;
	// RCC->PLLSAI1CFGR |= U<<21;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1QEN;
	
	// PLLSAI1 division factor for PLLADC1CLK (ADC clock)
	// 00: PLLSAI1R = 2, 01: PLLSAI1R = 4, 10: PLLSAI1R = 6, 11: PLLSAI1R = 8
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1R; 
	// RCC->PLLSAI1CFGR |= U<<25;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
	
	RCC->CR |= RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == 0);
	
	// SAI1 clock source selection
	// 00: PLLSAI1 "P" clock (PLLSAI1CLK) selected as SAI1 clock
	// 01: PLLSAI2 "P" clock (PLLSAI2CLK) selected as SAI1 clock
	// 10: PLL "P" clock (PLLSAI3CLK) selected as SAI1 clock
	// 11: External input SAI1_EXTCLK selected as SAI1 clock	
	RCC->CCIPR &= ~RCC_CCIPR_SAI1SEL;

	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
}


int main(void) {	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	// RCC->CR |= RCC_CR_HSION;
	// while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// // Select HSI as system clock source 
	// RCC->CFGR &= ~RCC_CFGR_SW;
	// RCC->CFGR |= RCC_CFGR_SW_HSI;
	// while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source

	// // from uart lab
	// // System_Clock_Init();

	// // Initalize UART -- using USART1
	// // Init_USARTx(1);

	// // Input Capture Setup
	// Input_Capture_Setup();
	
	// // Trigger Setup
	// Trigger_Setup();

	// double distanceCm = 0;
	// double distanceIn = 0;
	// while(1) {
	// 	// [TODO] Store your measurements on Stack
	// 	if ((150 <= timeInterval) && (timeInterval <= 1480)) {
	// 		// max pulse 25ms -> ~168 inches. limit to 1480 -> ~10 inches
	// 		distanceCm = timeInterval / 58;
	// 		distanceIn = timeInterval / 148;
	// 		// possible idea: only send bluetooth signal if this condition is satisfied (to avoid junk data sent)
	// 		// printf("Current Distance: %32d inches\n", distanceIn);
	// 	} else {
	// 		// object out of range
	// 		distanceCm = 0;
	// 		distanceIn = 0;
	// 	}

	// }
	System_Clock_Init_LCD();   // System Clock = 80 MHz
	SysTick_Init();
	
	
	SPI_GPIO_Init();
	SPI_Init();

	
	ILI9341_Init(SPI1,GPIOA,5,GPIOA,6,GPIOA,7);
}
