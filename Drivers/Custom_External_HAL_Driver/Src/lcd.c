/*
 * lcd.c
 *
 *  Created on: Sep 24, 2024
 *      Author: MelvinJr.Gaviola
 */


#include "lcd.h"


static uint8_t Line_Position = 0;

void LCD_Init (void)
{
    HAL_Delay(20); // Wait for LCD to power up

    LCD_Send_Command(FOUR_BIT_MODE); // Initialize in 4-bit mode
	LCD_Send_Command(TWO_BY_FIVE_BY_SEVEN); // 2 lines, 5x7 matrix
	LCD_Send_Command(DISPLAY_ON_CUR_OFF); // Display on, cursor off
	LCD_Send_Command(INCREMENT_CURSOR); // Increment cursor
	LCD_Send_Command(CLEAR_DISPLAY); // Clear display
	HAL_Delay(2);

	LCD_Send_Command(CUR_BEGIN_UPPER_SECOND); // Move cursor to the beginning
}

void LCD_Send (uint8_t cmd, GPIO_PinState mode)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, mode); // RS = 0 for command, 1 for data
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // RW = 0 for write

    // Send higher nibble
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (cmd & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (cmd & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (cmd & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (cmd & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // E = 1
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // E = 0

    // Send lower nibble
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (cmd & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (cmd & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (cmd & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (cmd & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // E = 1
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // E = 0
}

void LCD_Send_Command (uint8_t cmd)
{
	LCD_Send(cmd, GPIO_PIN_RESET);
}

void LCD_Send_Data (uint8_t cmd)
{
	LCD_Send(cmd, GPIO_PIN_SET);
}

void LCD_Send_Data_Upper (uint8_t cmd)
{
	if (Line_Position) {
		LCD_Send_Command(CUR_BEGIN_UPPER_SECOND);
		Line_Position = ~Line_Position;
	}
	LCD_Send(cmd, GPIO_PIN_SET);
}

void LCD_Send_Data_Lower (uint8_t cmd)
{
	if (!Line_Position) {
		LCD_Send_Command(CUR_BEGIN_LOWER_SECOND);
		Line_Position = ~Line_Position;
	}
	LCD_Send(cmd, GPIO_PIN_SET);
}

void Update_Position_Display (uint8_t pos, uint8_t data)
{
	if (pos >= 0xC0 ) {
		Line_Position = 1;
	} else {
		Line_Position = 0;
	}
	LCD_Send(data, GPIO_PIN_SET);
}
