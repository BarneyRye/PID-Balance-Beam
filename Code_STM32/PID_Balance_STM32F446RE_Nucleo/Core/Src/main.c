/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
  float refAngle;
  float currentAngle;
  float Kp;
  float Ki;
  float Kd;
  float lastError;
  float error;
  float lastTime;
  float currentTime;
  float dt;
  float errorDerivative;
  float errorSum;
  float upperLimit;
  float lowerLimit;
  float output;
} PIDController;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void mapVtoPWM(float voltage, uint16_t *pwmValue);
void doPIDControl(PIDController *pid);
float adcToAngle(uint32_t adcValue);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */ 

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  PIDController pid {
    .refAngle = 0.0f,
    .currentAngle = 0.0f,
    .Kp = 50.0f,
    .Ki = 0.1f,
    .Kd = 10.0f,
    .lastError = 0.0f,
    .error = 0.0f,
    .lastTime = 0.0f,
    .currentTime = 0.0f,
    .dt = 0.0f,
    .errorDerivative = 0.0f,
    .errorSum = 0.0f,
    .upperLimit = 3.0f,
    .lowerLimit = -3.0f,
    .output = 0.0f
  };
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    HAL_ADC_START(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
      uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
      pid.currentAngle = adcToAngle(adcValue);
    }

    doPIDControl(&pid);
    uint16_t pwmValue;
    mapVtoPWM(pid.output, &pwmValue);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwmValue);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void mapVtoPWM(float voltage, uint16_t *pwmValue) {
  const float maxVoltage = 6.0f;
  const float minVoltage = 0.0f;
  const float tim1period = 999.0f;

  voltage += 3.0f; 
  
  if (voltage > maxVoltage) {voltage = maxVoltage;} 
  else if (voltage < minVoltage) {voltage = minVoltage;}


  dutyCycle = (voltage - minVoltage) / (maxVoltage - minVoltage);
  *pwmValue = (uint16_t)(dutyCycle * 1000);
}

void doPIDControl(PIDController *pid) {
  pid->currentTime = HAL_GetTick() / 1000.0f;
  pid->dt = pid->currentTime - pid->lastTime;
  pid->lastTime = pid->currentTime;

  pid-> error = pid->refAngle - pid->currentAngle;
  pid->errorSum += pid->error * pid->dt;
  pid->errorDerivative = (pid->error - pid->lastError) / pid->dt;
  pid->lastError = pid->error;

  float pidSum = pid->Kp * pid->error + pid->Ki * pid->errorSum + pid->Kd * pid->errorDerivative;
  if (pidSum > pid->upperLimit) {pidSum = pid->upperLimit;} 
  else if (pidSum < pid->lowerLimit) {pidSum = pid->lowerLimit;}
  pid->output = pidSum;
}

float adcToAngle(uint32_t adcValue) {
  const float maxADCValue = 4095.0f; //12-bit ADC
  const float maxAngle = 270.0f;
  const float minAngle = 0.0f;
  return (float(adcValue)/maxADCValue) * (maxAngle - minAngle) + minAngle;
}
/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
