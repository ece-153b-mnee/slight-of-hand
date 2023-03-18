#include "UART.h"

void UART1_Init(void) {
	// [TODO]
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // enable USART1 clock
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL); // Mask USART1 Clock source
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0; // Select system clock as USART2 clock source
}

void UART2_Init(void) {
	// [TODO]
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // enable USART2 clock
	RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL); // Mask USART2 Clock Source
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; // Select system clock as USART2 clock source
}

void UART1_GPIO_Init(void) {
	// [TODO]
	
	// Clock enable???
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // Enable clock of Port B

	// PB6 output, PB7 input
	GPIOB->MODER &= ~(GPIO_MODER_MODE6); // Mask PB6
	GPIOB->MODER &= ~(GPIO_MODER_MODE7); // Mask PB6

	GPIOB->MODER |= (GPIO_MODER_MODE6_1); // Set PB6 alt func
	GPIOB->MODER |= (GPIO_MODER_MODE7_1); //Set to PB7 to alt func
		
	
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6); // Mask afr6
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2);
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL7); // Mask afr7
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_2);
// DO WE NEED TO INITIALIZE THEM AS INPUT AND OUTPUT


// PB6 Output Initializations
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6); // Set PB6 to push pull
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6); // Set PB6 To high speed
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6); // Mask PB6 Pupdr
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0); // Set PB6 pull up
	
// PB7 Input Initializations
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7); // Set PB7 to push pull
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7); // Set PB7 To high speed
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7); // Mask PB7 Pupdr
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD7_0); // Set PB7 pull up
}

void UART2_GPIO_Init(void) {
	// [TODO]
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable clock of Port A 
	

	GPIOA->MODER &= ~(GPIO_MODER_MODE2); // Mask PA2
	GPIOA->MODER &= ~(GPIO_MODER_MODE3); // Mask PA3

	GPIOA->MODER |= (GPIO_MODER_MODE2_1); // Set PA2 to  alt func
	GPIOA->MODER |= (GPIO_MODER_MODE3_1); //Set to PA3 to alt func

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2); // Mask afr7
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL3); // Mask afr7
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2);
// DO WE NEED TO INITIALIZE THEM AS INPUT AND OUTPUT


// PA2 Output Initializations
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2); // Set PA2 to push pull
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2); // Set PA2 To high speed
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2); // Mask PA2 Pupdr
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD2_0); // Set PA2 pull up
	
// PA3 Input Initializations
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT3); // Set PA3 to push pull
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3); // Set PA3 To high speed
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD3); // Mask PA3 Pupdr
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD3_0); // Set PA3 pull up

}

void USART_Init(USART_TypeDef* USARTx) {
	// [TODO]
	
	USARTx->CR1 &= ~(USART_CR1_UE); // Disable USART in order to do writes to CR1
	USARTx->CR1 &= ~(USART_CR1_M); // Set word length to 8 bits
	USARTx->CR1 &= ~(USART_CR1_OVER8); // Set oversampling to 16
	USARTx->CR2 &= ~(USART_CR2_STOP); // Set stop bits to 1

	// deaulft clk speed 80 Mhz/9600 = 8333
	// clk speed 16mhz/9600 = 1667
	USARTx->BRR = 8333; 

	USARTx->CR1 |= (USART_CR1_TE); // Enable trasmit
	USARTx->CR1 |= (USART_CR1_RE); // Enable Recieve
	USARTx->CR1 |= (USART_CR1_UE); // Reenable USART

}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
