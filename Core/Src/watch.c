/*
 * watch.c
 *
 * Created on: Sep 24, 2024
 * Author: MelvinJr.Gaviola
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "watch.h"
#include "lcd.h"

#define TIME_DELAY				1050U
#define SHORT_TIME_DELAY		400U
#define MAX_TEMP_VAL			100U
#define ALARM_DURATION			9U
#define MAX_WEEKDAY				7U
#define TEMP_SIZE				3U
#define TEMP_DAY_SIZE			4U
#define BELL_ASCII				0x07
#define EMPTY					(' ')
#define DEBUG_MODE				1U

#if DEBUG_MODE
 #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
 #define DEBUG_PRINT(...) /* Do nothing */
#endif

#define UPDATE_POSITION(flipper, data1, data2) \
 do { \
 if (flipper) { \
     LCD_Send_Data(data1); \
     LCD_Send_Data(data2); \
 } else { \
     LCD_Send_Data(EMPTY); \
     LCD_Send_Data(EMPTY); \
 } \
 } while (0)

extern RTC_HandleTypeDef hrtc;
extern volatile uint8_t function_mode;
extern volatile uint8_t blue_ctr;
extern volatile uint8_t green_ctr;
extern volatile uint8_t yellow_ctr;
extern volatile uint8_t toggle_chime_function;
extern TaskHandle_t blinkHandle;

static uint8_t time_format = 0U;
static uint8_t internal_toggle_chime_function = 0U;

/* Internal functions */
static void enterFunctionMode (void);
static void exitFunctionMode (void);
static void stopwatchInit (void);
static void alarmInit (void);
static void setAlarm (uint8_t hours, uint8_t minutes);
static uint8_t getDaysInMonth (uint8_t month, uint8_t year);

void blinkBacklight (void *param)
{
    (void)param;
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(SHORT_TIME_DELAY));
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(SHORT_TIME_DELAY));
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(SHORT_TIME_DELAY));
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(SHORT_TIME_DELAY));
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(SHORT_TIME_DELAY));
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
    }
}

void displayTimeTask (void *param)
{
    (void)param;
    RTC_TimeTypeDef gTime = {0};
    RTC_DateTypeDef gDate = {0};
    char temp[TEMP_SIZE] = {0};
    char tempWeekDay[TEMP_DAY_SIZE] = {0};
    uint8_t meridiem = 0U;
    const char weekDay[MAX_WEEKDAY][TEMP_DAY_SIZE] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    while (true) {
        HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
        meridiem = (gTime.Hours >= 12U) ? 1U : 0U;

        LCD_Send_Command(CUR_BEGIN_UPPER_SECOND);
        (void)sprintf(temp, "%02d", (gDate.Date % MAX_TEMP_VAL));
        LCD_Send_Data_Upper(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Upper(temp[1]);
        DEBUG_PRINT("%c", temp[1]);
        LCD_Send_Data_Upper('/');
        DEBUG_PRINT("/");
        (void)sprintf(temp, "%02d", (gDate.Month % MAX_TEMP_VAL));
        LCD_Send_Data_Upper(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Upper(temp[1]);
        DEBUG_PRINT("%c", temp[1]);
        LCD_Send_Data_Upper('/');
        DEBUG_PRINT("/");
        (void)sprintf(temp, "%02d", (gDate.Year % MAX_TEMP_VAL));
        LCD_Send_Data_Upper('2');
        DEBUG_PRINT("2");
        LCD_Send_Data_Upper('0');
        DEBUG_PRINT("0");
        LCD_Send_Data_Upper(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Upper(temp[1]);
        DEBUG_PRINT("%c", temp[1]);
        LCD_Send_Data(EMPTY);
        LCD_Send_Data(EMPTY);
        DEBUG_PRINT("%c", EMPTY);
		(void)sprintf(tempWeekDay, "%s", weekDay[(gDate.WeekDay - 1)]);
        LCD_Send_Data_Upper(tempWeekDay[0]);
        DEBUG_PRINT("%c", tempWeekDay[0]);
        LCD_Send_Data_Upper(tempWeekDay[1]);
        DEBUG_PRINT("%c", tempWeekDay[1]);
        LCD_Send_Data_Upper(tempWeekDay[2]);
        DEBUG_PRINT("%c", tempWeekDay[2]);
        DEBUG_PRINT("\n");

        if (time_format != 0U) {
            (void)sprintf(temp, "%02d", (gTime.Hours % 12U));
        } else {
            (void)sprintf(temp, "%02d", (gTime.Hours % MAX_TEMP_VAL));
        }

        LCD_Send_Data_Lower(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Lower(temp[1]);
        DEBUG_PRINT("%c", temp[1]);
        LCD_Send_Data_Lower(':');
        DEBUG_PRINT(":");
        (void)sprintf(temp, "%02d", (gTime.Minutes % MAX_TEMP_VAL));
        LCD_Send_Data_Lower(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Lower(temp[1]);
        DEBUG_PRINT("%c", temp[1]);
        LCD_Send_Data_Lower(':');
        DEBUG_PRINT(":");
        (void)sprintf(temp, "%02d", (gTime.Seconds % MAX_TEMP_VAL));
        LCD_Send_Data_Lower(temp[0]);
        DEBUG_PRINT("%c", temp[0]);
        LCD_Send_Data_Lower(temp[1]);
        DEBUG_PRINT("%c", temp[1]);

        if (time_format != 0U) {
            LCD_Send_Data_Lower(EMPTY);
            DEBUG_PRINT(" ");
            if (meridiem == 0U) {
                LCD_Send_Data_Lower('A');
                DEBUG_PRINT("A");
            } else {
                LCD_Send_Data_Lower('P');
                DEBUG_PRINT("P");
            }
            LCD_Send_Data_Lower('M');
            DEBUG_PRINT("M");
        }

        if (toggle_chime_function != 0U) {
			LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
			LCD_Send_Data(BELL_ASCII);
			DEBUG_PRINT("%c", BELL_ASCII);
		} else {
			LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
			LCD_Send_Data(EMPTY);
			DEBUG_PRINT("%c", EMPTY);
		}

        DEBUG_PRINT("\n");
        vTaskDelay(pdMS_TO_TICKS(TIME_DELAY)/10);
//        vTaskDelay(pdMS_TO_TICKS(TIME_DELAY));
    }
}

void changeFormatTask (void *param)
{
    (void)param;
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        taskENTER_CRITICAL();
        enterFunctionMode();
        time_format = (uint8_t)(~time_format);
        exitFunctionMode();
        taskEXIT_CRITICAL();
        LCD_Send_Command(CLEAR_DISPLAY);
    }
}

void stopwatchTask (void *param)
{
    (void)param;
    uint8_t prev_green = 0U;
    char temp[TEMP_SIZE] = {0};
    uint8_t seconds = 0U;
    uint8_t minutes = 0U;
    uint8_t hours = 0U;

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        DEBUG_PRINT("You are in stopwatch mode!\n");
        enterFunctionMode();
        prev_green = green_ctr;
        stopwatchInit();

        while (true) {
            if (yellow_ctr > 0U) {
                break;
            }

            if (green_ctr == prev_green) {
                if ((blue_ctr % 2U) != 0U) {
                    /* start stopwatch */
                    LCD_Send_Command(CUR_BEGIN_LOWER_SECOND);
                    (void)sprintf(temp, "%02d", (hours % MAX_TEMP_VAL));
                    LCD_Send_Data_Lower(temp[0]);
                    LCD_Send_Data_Lower(temp[1]);
                    LCD_Send_Data_Lower(':');
                    (void)sprintf(temp, "%02d", (minutes % MAX_TEMP_VAL));
                    LCD_Send_Data_Lower(temp[0]);
                    LCD_Send_Data_Lower(temp[1]);
                    LCD_Send_Data_Lower(':');
                    (void)sprintf(temp, "%02d", (seconds % MAX_TEMP_VAL));
                    LCD_Send_Data_Lower(temp[0]);
                    LCD_Send_Data_Lower(temp[1]);
                    DEBUG_PRINT("Timer: %02d:%02d:%02d\n", hours, minutes, seconds);
                    seconds++;
                    if (seconds >= 60U) {
                        seconds = 0U;
                        minutes++;
                        if (minutes >= 60U) {
                            minutes = 0U;
                            hours++;
                        }
                    }
                }
            } else {
            	stopwatchInit();
                seconds = 0U;
                minutes = 0U;
                hours = 0U;
                prev_green = green_ctr;
            }

            taskYIELD();
            HAL_Delay(TIME_DELAY);
        }

        exitFunctionMode();
    }
}

void alarmTask (void *param)
{
	RTC_TimeTypeDef sTime = {0};
	uint8_t flipper = 0U;
	uint8_t prev_blue_ctr;
	uint8_t prev_toggle_chime_function;
	char temp[TEMP_SIZE] = {0};

	(void)param;

	while (true) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		DEBUG_PRINT("You are in alarm mode!\n");
		enterFunctionMode();

		prev_blue_ctr = blue_ctr;
		prev_toggle_chime_function = toggle_chime_function;

		alarmInit();
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		(void)sprintf(temp, "%02d", (sTime.Hours % MAX_TEMP_VAL));
		LCD_Send_Data_Lower(temp[0]);
		LCD_Send_Data_Lower(temp[1]);
		LCD_Send_Data_Lower(':');
		(void)sprintf(temp, "%02d", (sTime.Minutes % MAX_TEMP_VAL));
		LCD_Send_Data_Lower(temp[0]);
		LCD_Send_Data_Lower(temp[1]);

		while (true) {
			DEBUG_PRINT("BLUE %d GREEN %d!\n", blue_ctr, green_ctr);
			/* Exit condition */
			if (yellow_ctr != 0U) {
				setAlarm(sTime.Hours, sTime.Minutes);
				break;
			}

			/* Chime function */
			if (prev_toggle_chime_function != toggle_chime_function) {
				if (toggle_chime_function != 0U) {
					internal_toggle_chime_function = 1U;
					LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
					LCD_Send_Data(BELL_ASCII);
				} else {
					internal_toggle_chime_function = 0U;
					LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
					LCD_Send_Data(EMPTY);
				}
				prev_toggle_chime_function = toggle_chime_function;
			} else {
				/* Set alarm condition */
				if (blue_ctr >= 1U) {
					/* Display hour and minutes */
					if ((green_ctr % 2U) == 1U) {
						if (prev_blue_ctr != blue_ctr) {
							sTime.Hours = (uint8_t)((sTime.Hours + 1U) % 24U);
							prev_blue_ctr = blue_ctr;
						}
						(void)sprintf(temp, "%02d", (sTime.Hours % MAX_TEMP_VAL));
						LCD_Send_Command(CUR_BEGIN_LOWER_SECOND);
						UPDATE_POSITION(flipper, temp[0], temp[1]);
					} else {
						if (prev_blue_ctr != blue_ctr) {
							sTime.Minutes = (uint8_t)((sTime.Minutes + 1U) % 60U);
							prev_blue_ctr = blue_ctr;
						}
						(void)sprintf(temp, "%02d", (sTime.Minutes % MAX_TEMP_VAL));
						LCD_Send_Command(CUR_BEGIN_LOWER_FIFTH);
						UPDATE_POSITION(flipper, temp[0], temp[1]);
					}
					flipper = (uint8_t)(~flipper);
				}
			}

			taskYIELD();
			HAL_Delay(SHORT_TIME_DELAY);
		}
		exitFunctionMode();
	}
}

void hourlyAlarmTask (void *param)
{
	RTC_AlarmTypeDef sAlarm = {0};
	RTC_TimeTypeDef sTime;

	(void)param;

	sAlarm.AlarmTime.Minutes = 0U;
	sAlarm.AlarmTime.Seconds = 1U;
	sAlarm.AlarmTime.SubSeconds = 0U;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1U;
	sAlarm.Alarm = RTC_ALARM_B;

	while (true) {
		DEBUG_PRINT("You are in vHourlyAlarmTask!\n");
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		sAlarm.AlarmTime.Hours = (uint8_t)((sTime.Hours + 1U) % 24U);

		if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
			/* Alarm Setting Error */
		}

		/* Wait for the alarm to trigger */
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (internal_toggle_chime_function != 0U) {
			(void)xTaskNotifyGive(blinkHandle);
		}
		DEBUG_PRINT("Exit vHourlyAlarmTask!\n");
	}
}

void snoozeTask (void *param)
{
	RTC_AlarmTypeDef sAlarm = {0};
	RTC_TimeTypeDef sTime;
	uint8_t prev_green_ctr;
	uint8_t prev_blue_ctr;
	uint8_t prev_yellow_ctr;

	(void)param;

	sAlarm.AlarmTime.Seconds = 1U;
	sAlarm.AlarmTime.SubSeconds = 0U;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1U;
	sAlarm.Alarm = RTC_ALARM_A;

	prev_green_ctr = green_ctr;
	prev_blue_ctr = blue_ctr;
	prev_yellow_ctr = yellow_ctr;

	while (true) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		enterFunctionMode();
		DEBUG_PRINT("You are in snooze_task!\n");

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

		sAlarm.AlarmTime.Hours = sTime.Hours;
		sAlarm.AlarmTime.Minutes = (uint8_t)((sTime.Minutes + 1U) % 60U);

		uint8_t alarm_loop = 0U;

		while (alarm_loop < ALARM_DURATION) {
			if (prev_yellow_ctr != yellow_ctr) {
				break;
			}

			(void)xTaskNotifyGive(blinkHandle);
			alarm_loop++;
			prev_yellow_ctr = yellow_ctr;
			vTaskDelay(pdMS_TO_TICKS(2500U));
		}

		if ((prev_green_ctr != green_ctr) || (prev_blue_ctr != blue_ctr)) {
			exitFunctionMode();
			continue;
		}

		if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
			/* Alarm Setting Error */
		}

		prev_green_ctr = green_ctr;
		prev_blue_ctr = blue_ctr;

		DEBUG_PRINT("Exit snooze_task!\n");
		exitFunctionMode();
	}
}


void setAlarm (uint8_t hours, uint8_t minutes)
{
    RTC_AlarmTypeDef sAlarm = {0};
    RTC_TimeTypeDef sTime;

    DEBUG_PRINT("You are in set_alarm! %d %d\n", hours, minutes);
    /* Get the current time */
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    /* Configure the alarm to trigger at the start of the next hour */
    sAlarm.AlarmTime.Hours = hours;
    sAlarm.AlarmTime.Minutes = minutes;
    sAlarm.AlarmTime.Seconds = 1U;
    sAlarm.AlarmTime.SubSeconds = 0U;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 1U;
    sAlarm.Alarm = RTC_ALARM_A;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
        /* Alarm Setting Error */
    }
}

void adjustTime (void *param)
{
    (void)param;

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        DEBUG_PRINT("You are in time setting mode!\n");
        enterFunctionMode();

        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};
        uint8_t flipper = 0U;
        char temp[TEMP_SIZE] = {0};
        uint8_t prev_green_ctr = green_ctr;
        uint8_t prev_blue_ctr = blue_ctr;

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        while (true) {
            DEBUG_PRINT("%d, %d %d\n", yellow_ctr, blue_ctr, green_ctr);

            if (yellow_ctr > 0U) {
                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                break;
            }

            if (prev_green_ctr != green_ctr) {
            	taskENTER_CRITICAL();
                blue_ctr = 0U;
                taskEXIT_CRITICAL();
                prev_green_ctr = green_ctr;
            }

            switch (green_ctr % 6U) {
                case 0U:
                    /* Set seconds */
                    if (prev_blue_ctr != blue_ctr) {
                        sTime.Seconds = 0U;
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sTime.Seconds % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_LOWER_EIGHT);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                case 1U:
                    /* Set hour */
                    if (prev_blue_ctr != blue_ctr) {
                        sTime.Hours = (uint8_t)((sTime.Hours + 1U) % 24U);
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sTime.Hours % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_LOWER_SECOND);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                case 2U:
                    /* Set minute */
                    if (prev_blue_ctr != blue_ctr) {
                        sTime.Minutes = (uint8_t)((sTime.Minutes + 1U) % 60U);
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sTime.Minutes % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_LOWER_FIFTH);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                case 3U:
                    /* Set month */
                    if (prev_blue_ctr != blue_ctr) {
                        sDate.Month = (uint8_t)((((sDate.Month + 1U) % 12U) == 0U) ? 12U : ((sDate.Month + 1U) % 12U));
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sDate.Month % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_UPPER_FIFTH);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                case 4U:
                    /* Set day */
                    if (prev_blue_ctr != blue_ctr) {
                        uint8_t max_days = getDaysInMonth(sDate.Month, sDate.Year);
                        sDate.Date = (uint8_t)((((sDate.Date + 1U) % max_days) == 0U) ? max_days : ((sDate.Date + 1U) % max_days));
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sDate.Date % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_UPPER_SECOND);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                case 5U:
                    /* Set year */
                    if (prev_blue_ctr != blue_ctr) {
                        sDate.Year = (uint8_t)((sDate.Year + 1U) % 100U);
                        prev_blue_ctr = blue_ctr;
                    }
                    (void)sprintf(temp, "%02d", (sDate.Year % MAX_TEMP_VAL));
                    LCD_Send_Command(CUR_BEGIN_UPPER_TENTH);
                    UPDATE_POSITION(flipper, temp[0], temp[1]);
                    break;
                default:
                    /* Handle invalid button press count */
                    break;
            }

            flipper = (uint8_t)(~flipper);
            taskYIELD();
            HAL_Delay(SHORT_TIME_DELAY);
        }
        exitFunctionMode();
    }
}

void enterFunctionMode (void)
{
    taskENTER_CRITICAL();

    blue_ctr = 0U;
    green_ctr = 0U;
    yellow_ctr = 0U;
    function_mode = 1U;

    taskEXIT_CRITICAL();
}

void exitFunctionMode (void)
{
    taskENTER_CRITICAL();

    blue_ctr = 0U;
    green_ctr = 0U;
    yellow_ctr = 0U;
    function_mode = 0U;

    taskEXIT_CRITICAL();
}

void stopwatchInit (void)
{
    LCD_Send_Command(CLEAR_DISPLAY);
    LCD_Send_Command(CUR_BEGIN_UPPER_SECOND);
    LCD_Send_Data_Upper('T');
    LCD_Send_Data_Upper('i');
    LCD_Send_Data_Upper('m');
    LCD_Send_Data_Upper('e');
    LCD_Send_Data_Upper('r');
    LCD_Send_Data_Lower('0');
    LCD_Send_Data_Lower('0');
    LCD_Send_Data_Lower(':');
    LCD_Send_Data_Lower('0');
    LCD_Send_Data_Lower('0');
    LCD_Send_Data_Lower(':');
    LCD_Send_Data_Lower('0');
    LCD_Send_Data_Lower('0');
}

void alarmInit (void)
{
    LCD_Send_Command(CLEAR_DISPLAY);
    LCD_Send_Command(CUR_BEGIN_UPPER_SECOND);
    LCD_Send_Data_Upper('A');
    LCD_Send_Data_Upper('l');
    LCD_Send_Data_Upper('a');
    LCD_Send_Data_Upper('r');
    LCD_Send_Data_Upper('m');
    if (toggle_chime_function != 0U) {
		LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
		LCD_Send_Data(BELL_ASCII);
		DEBUG_PRINT("%c", BELL_ASCII);
	} else {
		LCD_Send_Command(CUR_BEGIN_LOWER_SIXTEEN);
		LCD_Send_Data(EMPTY);
		DEBUG_PRINT("%c", EMPTY);
	}
}

uint8_t getDaysInMonth (uint8_t month, uint8_t year)
{
    uint8_t days;

    switch (month) {
        case 1U: case 3U: case 5U: case 7U: case 8U: case 10U: case 12U:
            days = 31U;
            break;
        case 4U: case 6U: case 9U: case 11U:
            days = 30U;
            break;
        case 2U:
            /* Check for leap year */
            if (((year % 4U == 0U) && (year % 100U != 0U)) || (year % 400U == 0U)) {
                days = 29U;
            } else {
                days = 28U;
            }
            break;
        default:
            days = 0U; /* Invalid month */
            break;
    }

    return days;
}

