#include "SPI.h"
#include "SysTimer.h"
#include "stm32l476xx.h"


extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;


void SPI_GPIO_Init(void) {
	// Enable the GPIO Clock
	
	// Set Alternative Functions for SPI
	
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down

}

//SPI Init for ILI9341 on Nucleo board
void SPI_Init(void){
	// Enable SPI clock and Reset SPI
	
	// Disable SPI
	
	// Configure for Full Duplex Communication
	
	// Configure for 2-line Unidirectional Data Mode
	
	// Set Frame Format

	// Configure Clock
	
	// Set Baud Rate Prescaler.

	// Disable Hardware CRC Calculation
	
	// Set as Master and Enable Software Slave Management and NSS Pulse Management
	
	// Manage NSS using Software
	
	// Set FIFO Reception Threshold
	
	// Enable SPI
}


void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, uint8_t * rxBuffer, int size) {

	//Set up SPI TX
	
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	//Set up SPI RX
}

//Incorporate delay function (same as delay() in previous labs but with us)
void SPI_Delay(uint32_t us) {

}
