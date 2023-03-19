#include "LCD_Print_Helpers.h"


void LCD_Setup_White(){
	SPI_GPIO_Init();
	SPI_Init();

	ILI9341_Init(SPI1,GPIOA,5,GPIOA,6,GPIOA,7);
	delay(750);
	ILI9341_setRotation(1);
	delay(750);
	ILI9341_Fill(COLOR_WHITE);
	delay(750);
	ILI9341_printText("sLight   of hand",25,16,COLOR_BLACK,COLOR_WHITE,2);
	delay(250);
	ILI9341_printText("Light",37,13,COLOR_YELLOW,COLOR_WHITE,3);
	delay(250);
	ILI9341_printText("10:30 AM",48,128,COLOR_BLACK,COLOR_WHITE,3);
	delay(250);
	ILI9341_printText("Brightness: ",24,224,COLOR_BLACK,COLOR_WHITE,2);
	delay(250);
	ILI9341_printText("100%",165,224,COLOR_BLACK,COLOR_WHITE,2);
	delay(250);
	ILI9341_printText("Ethan Epp",5,300,COLOR_BLACK,COLOR_WHITE,1);
	delay(250);
	ILI9341_printText("Matthew Nguyen",5,310,COLOR_BLACK,COLOR_WHITE,1);
	
	// start light brightness w/ 100%
	uint32_t initialBrightness;
	initialBrightness = 10;
	printf("%32u\n", initialBrightness);
	LCD_Set_Brightness(10);

}

void LCD_Set_Time(char text[]){
	delay(250);
	ILI9341_printText(text,48,128,COLOR_BLACK,COLOR_WHITE,3);
}

void LCD_Set_Brightness(uint32_t distanceIn){
	delay(250);
	if ((1 <= distanceIn) && (distanceIn <= 2)) {
		// 25%
		ILI9341_printText("25% ",165,224,COLOR_BLACK,COLOR_WHITE,2);
	}
	else if ((3 <= distanceIn) && (distanceIn <= 5)) {
		// 50%
		ILI9341_printText("50% ",165,224,COLOR_BLACK,COLOR_WHITE,2);
	}
	else if ((6 <= distanceIn) && (distanceIn <= 8)) {
		// 75%
		ILI9341_printText("75% ",165,224,COLOR_BLACK,COLOR_WHITE,2);
	}
	else if ((9 <= distanceIn) && (distanceIn <= 10)) {
		// 100%
		ILI9341_printText("100%",165,224,COLOR_BLACK,COLOR_WHITE,2);
	}
}

