/*
 * lcd.h
 *
 *  Created on: Sep 24, 2024
 *      Author: MelvinJr.Gaviola
 */

#ifndef CUSTOM_EXTERNAL_HAL_DRIVER_INC_LCD_H_
#define CUSTOM_EXTERNAL_HAL_DRIVER_INC_LCD_H_

#include "stm32f4xx_hal.h"

#define FOUR_BIT_MODE						0X02
#define CLEAR_DISPLAY						0x01
#define INCREMENT_CURSOR					0x06
#define CUR_BEGIN_UPPER_FIRST				0x80
#define CUR_BEGIN_UPPER_SECOND				0x81
#define CUR_BEGIN_UPPER_THIRD				0x82
#define CUR_BEGIN_UPPER_FOURTH				0x83
#define CUR_BEGIN_UPPER_FIFTH				0x84
#define CUR_BEGIN_UPPER_EIGHT				0x87
#define CUR_BEGIN_UPPER_TENTH				0x89
#define CUR_BEGIN_UPPER_FOURTEEN			0x8D
#define CUR_BEGIN_UPPER_SIXTEEN				0x8F
#define CUR_BEGIN_LOWER_FIRST				0xC0
#define CUR_BEGIN_LOWER_SECOND				0xC1
#define CUR_BEGIN_LOWER_THIRD				0xC2
#define CUR_BEGIN_LOWER_FOURTH				0xC3
#define CUR_BEGIN_LOWER_FIFTH				0xC4
#define CUR_BEGIN_LOWER_EIGHT				0xC7
#define CUR_BEGIN_LOWER_SIXTEEN				0xCF
#define TWO_BY_FIVE_BY_SEVEN				0x28
#define DISPLAY_ON_CUR_OFF					0x0C

void LCD_Init								(void);
void LCD_Send_Command						(uint8_t cmd);
void LCD_Send_Data 							(uint8_t cmd);
void LCD_Send_Data_Upper					(uint8_t cmd);
void LCD_Send_Data_Lower					(uint8_t cmd);
void Update_Position_Display 				(uint8_t pos, uint8_t data);

#endif /* CUSTOM_EXTERNAL_HAL_DRIVER_INC_LCD_H_ */
