/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define MCU_HBEAT_Pin GPIO_PIN_5
#define MCU_HBEAT_GPIO_Port GPIOA
#define MCU_FAULT_Pin GPIO_PIN_6
#define MCU_FAULT_GPIO_Port GPIOA
#define MCU_GSENSE_Pin GPIO_PIN_7
#define MCU_GSENSE_GPIO_Port GPIOA
#define RFD_GPIO5_Pin GPIO_PIN_4
#define RFD_GPIO5_GPIO_Port GPIOC
#define RFD_GPIO4_Pin GPIO_PIN_5
#define RFD_GPIO4_GPIO_Port GPIOC
#define RFD_GPIO3_Pin GPIO_PIN_0
#define RFD_GPIO3_GPIO_Port GPIOB
#define RFD_GPIO2_Pin GPIO_PIN_1
#define RFD_GPIO2_GPIO_Port GPIOB
#define RFD_GPIO1_Pin GPIO_PIN_2
#define RFD_GPIO1_GPIO_Port GPIOB
#define RFD_GPIO0_Pin GPIO_PIN_10
#define RFD_GPIO0_GPIO_Port GPIOB
#define SDIO_CD_Pin GPIO_PIN_15
#define SDIO_CD_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
