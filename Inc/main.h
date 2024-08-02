/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <cmsis_os.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void _Error_Handler(char const* file, uint32_t line) __attribute__((noreturn));
void idleInterruptCallback(UART_HandleTypeDef* huart);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define AUDIO_IN_Pin GPIO_PIN_3
#define AUDIO_IN_GPIO_Port GPIOA
#define AUDIO_OUT_DAC_Pin GPIO_PIN_4
#define AUDIO_OUT_DAC_GPIO_Port GPIOA
#define AUDIO_IN_VREF_Pin GPIO_PIN_5
#define AUDIO_IN_VREF_GPIO_Port GPIOA
#define BUTTON_AUDIO_IN_ADJUST_Pin GPIO_PIN_1
#define BUTTON_AUDIO_IN_ADJUST_GPIO_Port GPIOB
#define BUTTON_AUDIO_IN_ADJUST_EXTI_IRQn EXTI1_IRQn
#define LED_RED_Pin GPIO_PIN_8
#define LED_RED_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_9
#define LED_GREEN_GPIO_Port GPIOA
#define LED_YELLOW_Pin GPIO_PIN_10
#define LED_YELLOW_GPIO_Port GPIOA
#define PTT_M_Pin GPIO_PIN_11
#define PTT_M_GPIO_Port GPIOA
#define PTT_S_Pin GPIO_PIN_12
#define PTT_S_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define LD3_Pin GPIO_PIN_3
#define LD3_GPIO_Port GPIOB
#define AUDIO_OUT_ATTEN_Pin GPIO_PIN_5
#define AUDIO_OUT_ATTEN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

// #define TNC_HAS_LSCO -- Not available on NucleoTNC
// #define TNC_HAS_SWO -- Not available on NucleoTNC
#define TNC_HAS_LSE
// #define TNC_HAS_HSE -- Not available on NucleoTNC
// #define TNC_HAS_MCO -- Not available on NucleoTNC
// #define TNC_HAS_BT -- Not available on NucleoTNC
// #define TNC_HAS_BAT -- Not available on NucleoTNC
// #define TNC_HAS_USB -- Not available on NucleoTNC

// Compatibility defines
// #define BATTERY_ADC_HANDLE hadc2
// #define BATTERY_ADC_CHANNEL ADC_CHANNEL_16
#define LED_PWM_TIMER_HANDLE htim1
#define EEPROM_I2C hi2c3
#define SERIAL_UART huart2
#define AUDIO_ADC_HANDLE hadc1
#define AUDIO_ADC_CHANNEL CHANNEL_8

//#define USB_CE_Pin GPIO_PIN_4
//#define USB_CE_GPIO_Port GPIOB

#define EEPROM_ADDRESS 0xA0
#define EEPROM_CAPACITY 4096
#define EEPROM_PAGE_SIZE 32
#define EEPROM_WRITE_TIME 5

#define CMD_USER_BUTTON_DOWN 1
#define CMD_USER_BUTTON_UP 2
#define CMD_SET_PTT_SIMPLEX 3
#define CMD_SET_PTT_MULTIPLEX 4

extern char error_message[80];
extern char serial_number_64[13];
extern osMutexId hardwareInitMutexHandle;

#define CxxErrorHandler() _Error_Handler(__FILE__, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif

// These are no-ops on NucleoTNC.
void SysClock80(void);
void SysClock72(void);
void SysClock48(void);
void SysClock4(void);

#ifdef __cplusplus
}
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
