/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Ethan Epp and Matthew Nguyen
 * Section:
 * Final Project
 */
 
 
#include "RTC.h"
#include "MY_ILI9341_template.h"
// Use the 32.768 kHz low-speed external clock as RTC clock source

// Helper macro to convert a value from 2 digit decimal format to BCD format
// __VALUE__ Byte to be converted
#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))
#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))

#define RTC_WEEKDAY_MONDAY              ((uint32_t)0x01) /*!< Monday    */
#define RTC_WEEKDAY_TUESDAY             ((uint32_t)0x02) /*!< Tuesday   */
#define RTC_WEEKDAY_WEDNESDAY           ((uint32_t)0x03) /*!< Wednesday */
#define RTC_WEEKDAY_THURSDAY            ((uint32_t)0x04) /*!< Thrusday  */
#define RTC_WEEKDAY_FRIDAY              ((uint32_t)0x05) /*!< Friday    */
#define RTC_WEEKDAY_SATURDAY            ((uint32_t)0x06) /*!< Saturday  */
#define RTC_WEEKDAY_SUNDAY              ((uint32_t)0x07) /*!< Sunday    */

#define RTC_MONTH_JANUARY               ((uint8_t)0x01)  /*!< January   */
#define RTC_MONTH_FEBRUARY              ((uint8_t)0x02)  /*!< February  */
#define RTC_MONTH_MARCH                 ((uint8_t)0x03)  /*!< March     */
#define RTC_MONTH_APRIL                 ((uint8_t)0x04)  /*!< April     */
#define RTC_MONTH_MAY                   ((uint8_t)0x05)  /*!< May       */
#define RTC_MONTH_JUNE                  ((uint8_t)0x06)  /*!< June      */
#define RTC_MONTH_JULY                  ((uint8_t)0x07)  /*!< July      */
#define RTC_MONTH_AUGUST                ((uint8_t)0x08)  /*!< August    */
#define RTC_MONTH_SEPTEMBER             ((uint8_t)0x09)  /*!< September */
#define RTC_MONTH_OCTOBER               ((uint8_t)0x10)  /*!< October   */
#define RTC_MONTH_NOVEMBER              ((uint8_t)0x11)  /*!< November  */
#define RTC_MONTH_DECEMBER              ((uint8_t)0x12)  /*!< December  */

void RTC_BAK_SetRegister(uint32_t BackupRegister, uint32_t Data) {
	register uint32_t tmp = 0;

	tmp = (uint32_t)(&(RTC->BKP0R));
	tmp += (BackupRegister * 4);

	/* Write the specified register */
	*(__IO uint32_t *)tmp = (uint32_t)Data;
}

void RTC_Init(void) {
	
	/* Enables the PWR Clock and Enables access to the backup domain #######*/
	/* To change the source clock of the RTC feature (LSE, LSI), you have to:
	 - Enable the power clock
	 - Enable write access to configure the RTC clock source (to be done once after reset).
	 - Reset the Back up Domain
	 - Configure the needed RTC clock source */
	RTC_Clock_Init();
	
	// Disable RTC registers write protection
	RTC_Disable_Write_Protection();
	
	// Enter in initialization mode
	RTC->ISR |= RTC_ISR_INIT;
	while( (RTC->ISR & RTC_ISR_INITF) == 0); // Wait until INITF is set after entering initialization mode
	
	// Configure the Date 
	/* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to */
	/*       provide directly the decimal value:                                */
	RTC_Set_Calendar_Date(RTC_WEEKDAY_THURSDAY, 3, RTC_MONTH_FEBRUARY, 23); /* [TODO] These values are stubs - fill in current date */
	
	// Configure the Time 
	RTC_Set_Time(1, 10, 37, 22); /* [TODO] These values are stubs - fill in current time */
  
	// Exit of initialization mode 
	RTC->ISR &= ~RTC_ISR_INIT;
	while((RTC->ISR & RTC_ISR_RSF) == 0); /* Wait for synchro */
                                          /* Note: Needed only if Shadow registers is enabled */
                                          /* LL_RTC_IsShadowRegBypassEnabled function can be used */
	
	// Enable RTC registers write protection 
	RTC_Enable_Write_Protection();

	// Writes a data in a RTC Backup data Register1 
	// to indicate date/time updated 
	RTC_BAK_SetRegister(1, 0x32F2);
}

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
#define RTC_POSITION_TR_HT    (uint32_t)POSITION_VAL(RTC_TR_HT)
#define RTC_POSITION_TR_HU    (uint32_t)POSITION_VAL(RTC_TR_HU)
#define RTC_POSITION_TR_MT    (uint32_t)POSITION_VAL(RTC_TR_MNT)
#define RTC_POSITION_TR_MU    (uint32_t)POSITION_VAL(RTC_TR_MNU)
#define RTC_POSITION_TR_ST    (uint32_t)POSITION_VAL(RTC_TR_ST)
#define RTC_POSITION_TR_SU    (uint32_t)POSITION_VAL(RTC_TR_SU)
#define RTC_POSITION_DR_YT    (uint32_t)POSITION_VAL(RTC_DR_YT)
#define RTC_POSITION_DR_YU    (uint32_t)POSITION_VAL(RTC_DR_YU)
#define RTC_POSITION_DR_MT    (uint32_t)POSITION_VAL(RTC_DR_MT)
#define RTC_POSITION_DR_MU    (uint32_t)POSITION_VAL(RTC_DR_MU)
#define RTC_POSITION_DR_DT    (uint32_t)POSITION_VAL(RTC_DR_DT)
#define RTC_POSITION_DR_DU    (uint32_t)POSITION_VAL(RTC_DR_DU)
#define RTC_POSITION_DR_WDU   (uint32_t)POSITION_VAL(RTC_DR_WDU)

void RTC_Set_Calendar_Date(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year) {
	// [TODO] Write the date values in the correct place within the RTC Date Register
	uint32_t daysUnits = Day % 10;
	uint32_t daysTens = Day / 10;
	uint32_t yearsUnits = Year % 10;
	uint32_t yearsTens = Year / 10;
	uint32_t monthsUnits = Month % 16;
	uint32_t monthsTens = Month / 16;

	RTC->DR = (yearsTens<<RTC_POSITION_DR_YT) | (yearsUnits<<RTC_POSITION_DR_YU) | (WeekDay<<RTC_POSITION_DR_WDU) | (monthsUnits<<RTC_POSITION_DR_MU) | (monthsTens<<RTC_POSITION_DR_MT) | (daysUnits<<RTC_POSITION_DR_DU) | (daysTens<<RTC_POSITION_DR_DT);

}

void RTC_Set_Time(uint32_t Format12_24, uint32_t Hour, uint32_t Minute, uint32_t Second) {
	// [TODO] Write the time values in the correct place within the RTC Time Register
	uint32_t minuteUnits = Minute % 10;
	uint32_t minuteTens =  Minute / 10;
	uint32_t hourUnits= Hour % 10;
	uint32_t hourTens = Hour / 10;
	uint32_t secondUnits= Second % 10;
	uint32_t secondTens = Second / 10;

	RTC->TR = (minuteUnits<<RTC_POSITION_TR_MU) | (minuteTens<<RTC_POSITION_TR_MT) | (hourUnits<<RTC_POSITION_TR_HU) | (hourTens<<RTC_POSITION_TR_HT)|(secondUnits<<RTC_POSITION_TR_SU)|(secondTens<<RTC_POSITION_TR_ST)|(Format12_24);
}

void RTC_Clock_Init(void) {
	
	// Enable write access to Backup domain
	if ((RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Power interface clock enable
		(void) RCC->APB1ENR1; // Delay after an RCC peripheral clock enabling
	}
	
	// Select LSE as RTC clock source 
	if ( (PWR->CR1 & PWR_CR1_DBP) == 0) {
		PWR->CR1  |= PWR_CR1_DBP;             // Enable write access to Backup domain
		while((PWR->CR1 & PWR_CR1_DBP) == 0); // Wait for Backup domain Write protection disable
	}
	
	// Reset LSEON and LSEBYP bits before configuring the LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

	// RTC Clock selection can be changed only if the Backup Domain is reset
	RCC->BDCR |=  RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	
	// Note from STM32L4 Reference Manual: 	
	// RTC/LCD Clock:  (1) LSE is in the Backup domain. (2) HSE and LSI are not.	
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0) { // Wait until LSE clock ready
		RCC->BDCR |= RCC_BDCR_LSEON;
	}
	
	// Select LSE as RTC clock source
	// BDCR = Backup Domain Control Register 
	RCC->BDCR	&= ~RCC_BDCR_RTCSEL;  // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE
	RCC->BDCR	|= RCC_BDCR_RTCSEL_0; // Select LSE as RTC clock	
	
	RCC->BDCR |= RCC_BDCR_RTCEN; // Enable RTC
	
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN; // Power interface clock disable
}

void RTC_Disable_Write_Protection(void) {
	// [TODO]
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
}
	
void RTC_Enable_Write_Protection(void) {
	// [TODO]
	RTC->WPR = 0x69;
}



char* RTC_Get_Time_String(void){
	char strTime[10] = {0};

	sprintf((char*)strTime,"%.2d:%.2d:", 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetHour()), 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetMinute()));
	return strTime;
}

uint32_t RTC_TIME_GetHour(void) {
	// [TODO]
	return (((RTC->TR & RTC_TR_HT)>>16)) | ((RTC->TR & RTC_TR_HU)>>16);
}

uint32_t RTC_TIME_GetMinute(void) {
	// [TODO]
	return (((RTC->TR & RTC_TR_MNT)>>8)) | ((RTC->TR & RTC_TR_MNU)>>8);
}

uint32_t RTC_TIME_GetSecond(void) {
	// [TODO]
	return (((RTC->TR & RTC_TR_ST)) | (RTC->TR & RTC_TR_SU));
}

uint32_t RTC_DATE_GetMonth(void) {
	// [TODO]
	return (((RTC->DR & RTC_DR_MT)>>8)) | ((RTC->DR & RTC_DR_MU)>>8);
}

uint32_t RTC_DATE_GetDay(void) {
	// [TODO]
	return (((RTC->DR & RTC_DR_DT))) | (RTC->DR & RTC_DR_DU);
}

uint32_t RTC_DATE_GetYear(void) {
	// [TODO]
	return (((RTC->DR & RTC_DR_YT)>>16)) | ((RTC->DR & RTC_DR_YU)>>16);
}

uint32_t RTC_DATE_GetWeekDay(void) {
	// [TODO]
	return (RTC->DR & RTC_DR_WDU) >> 13;
}

void Get_RTC_Calendar(char * strTime, char * strDate) {
	/* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
	/* Display time Format : hh:mm:ss */
	sprintf((char*)strTime,"%.2d:%.2d:%.2d", 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetHour()), 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetMinute()), 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetSecond()));
	/* Display date Format : mm-dd-yy */
	sprintf((char*)strDate,"%.2d-%.2d-%.2d", 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetMonth()), 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetDay()), 
		2000 + __RTC_CONVERT_BCD2BIN(RTC_DATE_GetYear()));
}


void RTC_Set_Alarm(void) {
	// [TODO]
	RTC_Disable_Write_Protection();

	// Disable alarm A
	RTC->CR &= ~RTC_CR_ALRAE;
	RTC->CR &= ~RTC_CR_ALRAIE;

	while (((RTC->ISR & RTC_ISR_ALRAWF) && (RTC->ISR & RTC_ISR_ALRBWF)) == 0); // wait until ready to write

	// alarm a: toggle every 30 sec
	RTC->ALRMAR &= ~RTC_ALRMAR_MSK1;
	RTC->ALRMAR |= RTC_ALRMAR_MSK2;
	RTC->ALRMAR |= RTC_ALRMAR_MSK3;
	RTC->ALRMAR |= RTC_ALRMAR_MSK4;
	RTC->ALRMAR |= (3UL<<4) | (0UL); // Alarm A interrupts at 30 second fields
	// Enable alarm A
	RTC->CR |= RTC_CR_ALRAE;
	RTC->CR |= RTC_CR_ALRAIE;


	// Clear interrupt flag
	EXTI->PR1 |= EXTI_PR1_PIF18; // clear interrupt flag


	// Clear Flag 
	RTC->ISR &= ~RTC_ISR_ALRAF;

	// Reenable write protection
	RTC_Enable_Write_Protection();
}

void RTC_Alarm_Enable(void) {
	// [TODO]

	// alarms are EXTI line 18
	EXTI->RTSR1 |= EXTI_RTSR1_RT18; // Disable rising edge trigger
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT18; // Enable falling edge trigger


	// Enable EXTI
	EXTI->IMR1 |= EXTI_IMR1_IM18;
	EXTI->EMR1 |= EXTI_EMR1_EM18;
	
	// Clear pending bit
	EXTI->PR1 |= EXTI_PR1_PIF18;

	// NVIC
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);
}

void RTC_Alarm_IRQHandler(void) {
	// if ((EXTI->PR1 & EXTI_PR1_PIF18) != 0) {
		// LED
		// Function to set kelvin temperature
		printf("t\n");
		RTC->ISR &= ~RTC_ISR_ALRAF;
		RTC->ISR &= ~RTC_ISR_ALRBF;
	// }
	EXTI->PR1 |= EXTI_PR1_PIF18; // clear interrupt flag
}
