/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define key_Pin GPIO_PIN_1
#define key_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_0
#define DC_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOB
#define screen_RST_Pin GPIO_PIN_2
#define screen_RST_GPIO_Port GPIOB
#define esp_en_Pin GPIO_PIN_11
#define esp_en_GPIO_Port GPIOA
#define AT24C02_VCC_Pin GPIO_PIN_4
#define AT24C02_VCC_GPIO_Port GPIOB
#define AT24C02_GND_Pin GPIO_PIN_5
#define AT24C02_GND_GPIO_Port GPIOB
#define AT24C02_SCL_Pin GPIO_PIN_6
#define AT24C02_SCL_GPIO_Port GPIOB
#define AT24C02_SDA_Pin GPIO_PIN_7
#define AT24C02_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern UART_HandleTypeDef huart1;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
