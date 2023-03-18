#include "SPI_template.h"
#include "SysTimer.h"
#include "stm32l476xx.h"


extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;


void SPI_GPIO_Init(void) {
	// Enable the GPIO Clock



	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // enable clock for port b
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// Set to alt function
	GPIOB->MODER &= ~(GPIO_MODER_MODE3); // Mask
	GPIOB->MODER &= ~(GPIO_MODER_MODE4);
	GPIOB->MODER &= ~(GPIO_MODER_MODE5);

	GPIOB->MODER |= (GPIO_MODER_MODE3_1);
	GPIOB->MODER |= (GPIO_MODER_MODE4_1);
	GPIOB->MODER |= (GPIO_MODER_MODE5_1);

	// Set Alternative Function
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3); // mask alt func
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL4); // mask alt func
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL5); // mask alt func
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_2); // Set as alt func 5
	// GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL3_2); // Set as alt func 5
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL4_0 | GPIO_AFRL_AFSEL4_2); // Set as alt func 5
	// GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL4_2); // Set as alt func 5
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2); // Set as alt func 5
	// GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL5_2); // Set as alt func 5

	// Set to push-pull
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT4);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT5);

	// Set output speed to very high
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED4;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;

	// Set to No Pull
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD4);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD5);





	// RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // enable clock for port b


	// // Set to alt function
	// GPIOB->MODER &= ~(GPIO_MODER_MODE3); // Mask
	// GPIOB->MODER &= ~(GPIO_MODER_MODE4);
	// GPIOB->MODER &= ~(GPIO_MODER_MODE5);
		
	// GPIOB->MODER |= (GPIO_MODER_MODE3_1);


	// // Set Alternative Functions for SPI
	// GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3); // mask alt func
	// GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_2); // Set as alt func 5
	

	// // Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
	// GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3);
	// GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
	// GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3);


	// GPIOB->MODER |= (GPIO_MODER_MODE4_0);
	// GPIOB->MODER |= (GPIO_MODER_MODE5_0);
	// GPIOB->OTYPER &= ~(GPIO_OTYPER_OT4);
	// GPIOB->OTYPER &= ~(GPIO_OTYPER_OT5);
	// GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD4);
	// GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD5);
}

//SPI Init for ILI9341 on Nucleo board
void SPI_Init(void){
	// Enable SPI clock and Reset SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //a) Clock enable
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; //b) Set RCC SPI Reset bit, p 238
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST; //b) Set RCC SPI Reset bit, p 238 added
	// Disable SPI
	SPI1->CR1 &= ~SPI_CR1_SPE; //c) Disable spi enable bit to configure
	
	// Configure for Full Duplex Communication
	SPI1->CR1 &= ~SPI_CR1_RXONLY; //d) COnfigure for full duplex communication
	
	// Configure for 2-line Unidirectional Data Mode
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE; //e) 2 line unidirectional data mode
	SPI1->CR1 &= ~SPI_CR1_BIDIOE; //f) Disable output in bidirectional mode
	
	// Set Frame Format
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //g) Configure frame format as msb first 

	SPI1->CR2 &= ~SPI_CR2_DS; // f) Confirgure to 8 bit mode
	SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); //f) Configure to 8 bit mode
	SPI1->CR2 &= ~SPI_CR2_FRF; //i) set to motorola mode


	// Configure Clock
	SPI1->CR1 &= ~SPI_CR1_CPOL; // j) set clock to low polarity
	SPI1->CR1 &= ~SPI_CR1_CPHA; // k) first clock transition
	// Set Baud Rate Prescaler.
	SPI1->CR1 &= ~SPI_CR1_BR; // l) mask baud rate prescalar
	SPI1->CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1); // l) set baud rate prescalar to 16 ??? not sure

	// Disable Hardware CRC Calculation
	SPI1->CR1 &= ~SPI_CR1_CRCEN; // m) disable hardware crc calculation
	
	// Set as Master and Enable Software Slave Management and NSS Pulse Management
	SPI1->CR1 |= SPI_CR1_MSTR; // n) set to master config
	SPI1->CR1 |= SPI_CR1_SSM; // o) enable software ssm
	
	// Manage NSS using Software
	SPI1->CR2 |= SPI_CR2_NSSP; // p) Enable pulse generation
	SPI1->CR1 |= SPI_CR1_SSI; // q) configure internal slave select bit to 1 for master
	
	// Set FIFO Reception Threshold
	SPI1->CR2 |= SPI_CR2_FRXTH; // r) set fifo threshold to 1/4
	
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE; //s) Enable spi enable bit

}


void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, int size) { // null for rx
	for(int i = 0; i<size; i++){
		//Set up SPI TX
		while((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE){
			// buffer and wait for TXE flag
		}
		*((volatile uint8_t*)&SPIx->DR) = *txBuffer;

		while((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY){
			// Wait for busy flag to be low
		}
		// rxBuffer = rxBuffer + 8;
		txBuffer = txBuffer++;
	}
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	for(int i = 0; i<size; i++){
		//Set up SPI RX
		while((SPIx->SR & SPI_SR_RXNE) != SPI_SR_RXNE){
			// Wait for RXNE flag
		}
		*(rxBuffer+i) = *((volatile uint8_t*)&SPIx->DR) ; 
	}
}
 
void SPI_Send_Byte(SPI_TypeDef* SPIx, uint8_t write_data) {
	// TODO: send data from SPI1
	while((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE){
		// buffer and wait for TXE flag
	}
	*((volatile uint8_t*)&SPIx->DR) = write_data;

	while((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY){
		// Wait for busy flag to be low
	}
}
 
void SPI_Send_Byte(SPI_TypeDef* SPIx, uint8_t write_data) {
	// TODO: send data from SPI1
	while((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE){
		// buffer and wait for TXE flag
	}
	*((volatile uint8_t*)&SPIx->DR) = write_data;

	while((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY){
		// Wait for busy flag to be low
	}
}

void SPI_Send_2Byte(SPI_TypeDef* SPIx, uint16_t write_data) {
	// TODO: send data from SPI1
	while((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE){
		// buffer and wait for TXE flag
	}
	*((volatile uint16_t*)&SPIx->DR) = write_data;

	while((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY){
		// Wait for busy flag to be low
	}
}

void SPI_Receive_Byte(SPI_TypeDef* SPIx, uint8_t* read_data) {
	// TODO: receive data from SPI2
	while((SPIx->SR & SPI_SR_RXNE) != SPI_SR_RXNE){
		// Wait for RXNE flag
	}
	*read_data = *((volatile uint8_t*)&SPIx->DR) ; // no way this works lol

}


void SPI_Send_2Byte(SPI_TypeDef* SPIx, uint16_t write_data) {
	// TODO: send data from SPI1
	while((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE){
		// buffer and wait for TXE flag
	}
	*((volatile uint16_t*)&SPIx->DR) = write_data;

	while((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY){
		// Wait for busy flag to be low
	}
}

void SPI_Receive_Byte(SPI_TypeDef* SPIx, uint8_t* read_data) {
	// TODO: receive data from SPI2
	while((SPIx->SR & SPI_SR_RXNE) != SPI_SR_RXNE){
		// Wait for RXNE flag
	}
	*read_data = *((volatile uint8_t*)&SPIx->DR) ; // no way this works lol

}


//Incorporate delay function (same as delay() in previous labs but with us)
void SPI_Delay(uint32_t us) {
  uint32_t i , j;
	for(i=0;i<us;i++){
		for(j=0;j<18;j++){
			(void)i;
		}
	}
	
}
