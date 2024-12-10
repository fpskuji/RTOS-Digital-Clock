/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "watch.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    MINIMAL_PRIORITY_TASK = 1,
    LOW_PRIORITY_TASK = 2,
    HIGH_PRIORITY_TASK = 3,
    BUTTON_PRIORITY = 4
} TaskPriority;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static const TickType_t TICK_DELAY_50_MS 	= pdMS_TO_TICKS(50U);
static const TickType_t FUNCTION_DELAY 		= 2000U;

#define DEBUG_MODE				1U

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...) // Do nothing
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
TaskHandle_t redButtonHandle =		NULL;
TaskHandle_t blueButtonHandle =		NULL;
TaskHandle_t greenButtonHandle =	NULL;
TaskHandle_t yellowButtonHandle =	NULL;
TaskHandle_t displayTimeHandle =	NULL;
TaskHandle_t changeFormatHandle =	NULL;
TaskHandle_t stopwatchHandle =		NULL;
TaskHandle_t adjustTimeHandle =		NULL;
TaskHandle_t hourlyAlarmHandle =	NULL;
TaskHandle_t alarmTaskHandle =		NULL;
TaskHandle_t snoozeTaskHandle =		NULL;
TaskHandle_t blinkHandle =			NULL;

volatile uint8_t function_mode =			0;
volatile uint8_t blue_ctr =					0;
volatile uint8_t green_ctr =				0;
volatile uint8_t yellow_ctr =				0;
volatile uint8_t toggle_chime_function =	0;

void redButtonHandler			(void *param);
void blueButtonHandler			(void *param);
void greenButtonHandler			(void *param);
void yellowButtonHandler		(void *param);
void blueHoldButtonHandler		(void *param);
void hourlyAlarmHandler			(void *param);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	BaseType_t xReturned;

	/* Create the task, storing the handle. */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();

//  Task 1
	xReturned = xTaskCreate(
			redButtonHandler,					/* Function that implements the task. */
			"redButtonHandler",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			"redButtonHandler running...",		/* Parameter passed into the task. */
			BUTTON_PRIORITY,					/* Priority at which the task is created. */
			&redButtonHandle );					/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("redButtonHandler task created successfully\n");
	} else {
		DEBUG_PRINT("redButtonHandler task creation failed\n");
	}

//  Task 2
	xReturned = xTaskCreate(
			blueButtonHandler,					/* Function that implements the task. */
			"blueButtonHandler",				/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			"blueButtonHandler running...",		/* Parameter passed into the task. */
			BUTTON_PRIORITY,					/* Priority at which the task is created. */
			&blueButtonHandle );				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
	    DEBUG_PRINT("blueButtonHandler task created successfully\n");
	} else {
	    DEBUG_PRINT("blueButtonHandler task creation failed\n");
	}

//  Task 3
	xReturned = xTaskCreate(
			greenButtonHandler,					/* Function that implements the task. */
			"greenButtonHandler",				/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			"greenButtonHandler running...",	/* Parameter passed into the task. */
			BUTTON_PRIORITY,					/* Priority at which the task is created. */
			&greenButtonHandle );				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("greenButtonHandler task created successfully\n");
	} else {
		DEBUG_PRINT("greenButtonHandler task creation failed\n");
	}

//  Task 4
	xReturned = xTaskCreate(
			yellowButtonHandler,				/* Function that implements the task. */
			"yellowButtonHandler",				/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			"yellowButtonHandler running...",	/* Parameter passed into the task. */
			BUTTON_PRIORITY,					/* Priority at which the task is created. */
			&yellowButtonHandle );				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("yellowButtonHandler task created successfully\n");
	} else {
		DEBUG_PRINT("yellowButtonHandler task creation failed\n");
	}

//  Task 5
	xReturned = xTaskCreate(
			displayTimeTask,					/* Function that implements the task. */
			"displayTimeTask",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			LOW_PRIORITY_TASK,					/* Priority at which the task is created. */
			&displayTimeHandle);				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("display_time task created successfully\n");
	} else {
		DEBUG_PRINT("display_time task creation failed\n");
	}

//  Task 6
	xReturned = xTaskCreate(
			changeFormatTask,					/* Function that implements the task. */
			"changeFormatTask",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			HIGH_PRIORITY_TASK,					/* Priority at which the task is created. */
			&changeFormatHandle);				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("change_format task created successfully\n");
	} else {
		DEBUG_PRINT("change_format task creation failed\n");
	}

//  Task 7
	xReturned = xTaskCreate(
			stopwatchTask,						/* Function that implements the task. */
			"stopwatchTask",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			HIGH_PRIORITY_TASK,					/* Priority at which the task is created. */
			&stopwatchHandle);					/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("stopwatch task created successfully\n");
	} else {
		DEBUG_PRINT("stopwatch task creation failed\n");
	}

//  Task  8
	xReturned = xTaskCreate(
			adjustTime,							/* Function that implements the task. */
			"adjustTime",						/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			HIGH_PRIORITY_TASK,					/* Priority at which the task is created. */
			&adjustTimeHandle);					/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("adjust time task created successfully\n");
	} else {
		DEBUG_PRINT("adjust time task creation failed\n");
	}

//  Task 9
	xReturned = xTaskCreate(
			hourlyAlarmTask,					/* Function that implements the task. */
			"hourlyAlarmTask",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			LOW_PRIORITY_TASK,					/* Priority at which the task is created. */
			&hourlyAlarmHandle);				/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("hourlyAlarm task created successfully\n");
	} else {
		DEBUG_PRINT("hourlyAlarm task creation failed\n");
	}

//  Task 10
	xReturned = xTaskCreate(
			blinkBacklight,						/* Function that implements the task. */
			"blinkBacklight",					/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			MINIMAL_PRIORITY_TASK,				/* Priority at which the task is created. */
			&blinkHandle);						/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("hourlyAlarm task created successfully\n");
	} else {
		DEBUG_PRINT("hourlyAlarm task creation failed\n");
	}

//  Task 11
	xReturned = xTaskCreate(
			alarmTask,							/* Function that implements the task. */
			"alarmTask",						/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			HIGH_PRIORITY_TASK,					/* Priority at which the task is created. */
			&alarmTaskHandle);					/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("Alarm task created successfully\n");
	} else {
		DEBUG_PRINT("Alarm task creation failed\n");
	}

//  Task 12
	xReturned = xTaskCreate(
			snoozeTask,							/* Function that implements the task. */
			"snoozeTask",						/* Text name for the task. */
			configMINIMAL_STACK_SIZE,			/* Stack size in words, not bytes. */
			NULL,								/* Parameter passed into the task. */
			LOW_PRIORITY_TASK,					/* Priority at which the task is created. */
			&snoozeTaskHandle);					/* Used to pass out the created task's handle. */

	if (xReturned == pdPASS) {
		DEBUG_PRINT("Alarm task created successfully\n");
	} else {
		DEBUG_PRINT("Alarm task creation failed\n");
	}

	vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 16;
  sTime.Minutes = 59;
  sTime.Seconds = 20;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_SEPTEMBER;
  sDate.Date = 24;
  sDate.Year = 24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 1;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm B
  */
  sAlarm.Alarm = RTC_ALARM_B;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|RW_Pin|E_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Anode_GPIO_Port, Anode_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RS_Pin RW_Pin E_Pin D4_Pin
                           D5_Pin D6_Pin D7_Pin Anode_Pin */
  GPIO_InitStruct.Pin = RS_Pin|RW_Pin|E_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin|Anode_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void redButtonHandler (void *param)
{
    while (true) {
    	DEBUG_PRINT("LOG - [Enter] [%s] (%s)\n", __func__, (char*)param);

    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    	HAL_GPIO_TogglePin(GPIOA, Anode_Pin);

    	DEBUG_PRINT("LOG - [Exit ] [%s] (%s)\n\n", __func__, (char*)param);
    }

    vTaskDelete(NULL);
}

void blueButtonHandler (void *param)
{
	uint8_t notify_result;
	TickType_t delay_type = portMAX_DELAY;

	while (true) {
    	DEBUG_PRINT("LOG - [Enter] [%s] (%s)\n", __func__, (char*)param);

		while (true) {
			notify_result = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(delay_type));

			uint32_t pressStartTime = xTaskGetTickCount();

			// Wait for button release
			while (HAL_GPIO_ReadPin(GPIOC, RW_Pin) == GPIO_PIN_SET)
			{
				// Delay to debounce
				vTaskDelay(TICK_DELAY_50_MS);
			}

			// Calculate press duration
			uint32_t pressDuration = xTaskGetTickCount() - pressStartTime;

			if (pressDuration > pdMS_TO_TICKS(FUNCTION_DELAY)) {
				// Long press detected
				toggle_chime_function = ~toggle_chime_function;
				taskENTER_CRITICAL();
				blue_ctr = 0;
				taskEXIT_CRITICAL();

			} else {
				if (function_mode) {
					delay_type = portMAX_DELAY;
				} else {
					if (notify_result == pdTRUE) {
						delay_type = FUNCTION_DELAY;
					} else {
						break;
					}
				}
				taskENTER_CRITICAL();
				blue_ctr++;
				taskEXIT_CRITICAL();
			}
		}

		switch (blue_ctr) {
			case 1:
				DEBUG_PRINT("changeFormatHandle Sent!\n");
				xTaskNotifyGive(changeFormatHandle);
				vTaskDelay(TICK_DELAY_50_MS);
				delay_type = portMAX_DELAY;
				break;
			default:
				DEBUG_PRINT("No assigned function!\n");
				blue_ctr = 0;
				delay_type = portMAX_DELAY;
				break;
		}

    	DEBUG_PRINT("LOG - [Exit ] [%s] (%s)\n\n", __func__, (char*)param);
    }

    vTaskDelete(NULL);
}

void greenButtonHandler (void *param)
{
	uint8_t notify_result;
	TickType_t delay_type = portMAX_DELAY;

	while (true) {
    	DEBUG_PRINT("LOG - [Enter] [%s] (%s)\n", __func__, (char*)param);

		while (true) {
			notify_result = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(delay_type));

			if (function_mode) {
				delay_type = portMAX_DELAY;
			} else {
				if (notify_result == pdTRUE) {
					delay_type = FUNCTION_DELAY;
				} else {
					break;
				}
			}
			taskENTER_CRITICAL();
			green_ctr++;
			taskEXIT_CRITICAL();
		}

		switch (green_ctr) {
			default:
				DEBUG_PRINT("No assigned function!\n");
				green_ctr = 0;
				delay_type = portMAX_DELAY;
				break;
		}

    	DEBUG_PRINT("LOG - [Exit ] [%s] (%s)\n\n", __func__, (char*)param);
    }

    vTaskDelete(NULL);
}

void yellowButtonHandler (void *param)
{
	uint8_t notify_result;
	TickType_t delay_type = portMAX_DELAY;

	while (true) {
    	DEBUG_PRINT("LOG - [Enter] [%s] (%s)\n", __func__, (char*)param);

		while (true) {
			notify_result = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(delay_type));

			if (function_mode) {
				delay_type = portMAX_DELAY;
			} else {
				if (notify_result == pdTRUE) {
					delay_type = FUNCTION_DELAY;
				} else {
					break;
				}
			}
			taskENTER_CRITICAL();
			yellow_ctr++;
			taskEXIT_CRITICAL();
		}

		switch (yellow_ctr) {
			case 1:
				DEBUG_PRINT("Sent!\n");
				xTaskNotifyGive(alarmTaskHandle);
				vTaskDelay(TICK_DELAY_50_MS);
				delay_type = portMAX_DELAY;
				break;
			case 2:
				DEBUG_PRINT("Sent!\n");
				xTaskNotifyGive(stopwatchHandle);
				vTaskDelay(TICK_DELAY_50_MS);
				delay_type = portMAX_DELAY;
				break;
			case 3:
				DEBUG_PRINT("Sent!\n");
				xTaskNotifyGive(adjustTimeHandle);
				vTaskDelay(TICK_DELAY_50_MS);
				delay_type = portMAX_DELAY;
				break;
			default:
				DEBUG_PRINT("No assigned function!\n");
				yellow_ctr = 0;
				delay_type = portMAX_DELAY;
				break;
		}

    	DEBUG_PRINT("LOG - [Exit ] [%s] (%s)\n\n", __func__, (char*)param);
    }

    vTaskDelete(NULL);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
