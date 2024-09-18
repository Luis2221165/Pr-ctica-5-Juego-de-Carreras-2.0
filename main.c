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

/* Private variables ---------------------------------------------------------*/
int contador1 = 0;
int contador2 = 0;
int estadoAnteriorBotonContador1 = GPIO_PIN_SET;
int estadoAnteriorBotonContador2 = GPIO_PIN_SET;

/* Function prototypes -------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void iniciarContadores(void);
void mostrarContador(int valor, GPIO_TypeDef *port[], uint16_t leds[]);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Infinite loop */
  while (1)
  {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET)  // Verifica botón de semáforo
    {
      // Ciclo del semáforo
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);    // LED Rojo
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);    // LED Amarillo
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);    // LED Verde
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

      iniciarContadores();  // Inicia los contadores después del semáforo
    }
  }
}

/**
  * @brief  Function to manage counters
  */
void iniciarContadores(void)
{
  GPIO_TypeDef *puertosContador1[4] = {GPIOB, GPIOB, GPIOB, GPIOB};
  uint16_t pinesContador1[4] = {GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13};
  GPIO_TypeDef *puertosContador2[4] = {GPIOA, GPIOA, GPIOA, GPIOB};
  uint16_t pinesContador2[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_4, GPIO_PIN_0};

  while (1)
  {
    // Leer estado de los botones
    int estadoBotonContador1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
    int estadoBotonContador2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);

    // Control del contador 1
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET)  // Verifica si el contador 2 no está bloqueado
    {
      if (estadoBotonContador1 == GPIO_PIN_RESET && estadoAnteriorBotonContador1 == GPIO_PIN_SET)
      {
        contador1++;
        if (contador1 > 15) contador1 = 0;

        mostrarContador(contador1, puertosContador1, pinesContador1);

        if (contador1 == 15)
        {
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);   // Bloquear contador 2
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
        }
      }
      estadoAnteriorBotonContador1 = estadoBotonContador1;
    }

    // Control del contador 2
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET)  // Verifica si el contador 1 no está bloqueado
    {
      if (estadoBotonContador2 == GPIO_PIN_RESET && estadoAnteriorBotonContador2 == GPIO_PIN_SET)
      {
        contador2++;
        if (contador2 > 15) contador2 = 0;

        mostrarContador(contador2, puertosContador2, pinesContador2);

        if (contador2 == 15)
        {
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);   // Bloquear contador 1
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
        }
      }
      estadoAnteriorBotonContador2 = estadoBotonContador2;
    }

    HAL_Delay(50); // Pequeña pausa para evitar rebotes
  }
}

/**
  * @brief  Display counter value on LEDs
  */
void mostrarContador(int valor, GPIO_TypeDef *port[], uint16_t leds[])
{
  for (int i = 0; i < 4; i++)
  {
    HAL_GPIO_WritePin(port[i], leds[i], (valor >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

/* Configure GPIO pins */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configurar pines de salida para los LEDs
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configurar pines de salida para los LEDs del contador
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configurar botones como entrada con pull-up
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_8 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* USER CODE END 0 */
