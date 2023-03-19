#include "MY_ILI9341_template.h"
#include "SysTimer.h"
#include "stm32l476xx.h"
#include "SPI_template.h"
#include "UART.h"

void LCD_Setup_White(void);

void LCD_Set_Time(char text[]);

void LCD_Set_Brightness(uint32_t distanceIn);