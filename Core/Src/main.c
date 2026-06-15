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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gc9a01.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "gui_guider.h"
#include "custom.h"
#include "flash_storage.h"
#include "at24c02.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUF_SIZE 128
#define REFRESH_INTERVAL_MS 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
static uint8_t uart_rx_byte;
static uint8_t rx_buffer[RX_BUF_SIZE];
static uint8_t rx_wr_idx = 0;
static volatile uint8_t rx_ready = 0;

lv_ui guider_ui;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_DMA_Init(void);
/* USER CODE BEGIN PFP */
static void ParseRxData(void);
static uint8_t KeyProcess(void);
static void boot_self_test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#include "at24c02.h"
#include "gui_guider.h"

static void draw_term_line(uint16_t y, const char *txt, uint16_t color)
{
    uint16_t len = 0;
    while (txt[len]) len++;

    uint16_t x = 120 - (len * 4);     /* center horizontally for size=1 */
    GC9A01_DrawStringRotated(x, y, txt, color, GC9A01_BLACK, 1);
}

static void boot_self_test(void)
{
    uint16_t y = 52;   /* vertical start, centered block */
    const uint16_t line_h = 10;

    GC9A01_FillScreen(GC9A01_BLACK);

    /* header */
    draw_term_line(y, "=== SYSTEM BOOT ===", GC9A01_CYAN);
    y += line_h;
    HAL_Delay(200);

    /* 1. MCU */
    draw_term_line(y, "MCU   STM32F401  OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 2. Clock */
    draw_term_line(y, "CLK   168MHz     OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 3. I2C1 */
    draw_term_line(y, "I2C1             OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 4. AT24C02 */
    if (at24c02_init() == 0) {
        draw_term_line(y, "AT24C02          OK", GC9A01_GREEN);
    } else {
        draw_term_line(y, "AT24C02          NG", GC9A01_RED);
    }
    y += line_h;
    HAL_Delay(120);

    /* 5. SPI1 */
    draw_term_line(y, "SPI1             OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 6. Display */
    draw_term_line(y, "GC9A01           OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 7. USART1 */
    draw_term_line(y, "USART1 460800    OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 8. LVGL */
    draw_term_line(y, "LVGL  v9.3       OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 9. Flash */
    draw_term_line(y, "FLASH            OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* 10. Gauge */
    draw_term_line(y, "GAUGE            OK", GC9A01_GREEN);
    y += line_h;
    HAL_Delay(120);

    /* footer */
    y += line_h;
    draw_term_line(y, "=== READY ===", GC9A01_YELLOW);
    HAL_Delay(500);
}

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* Pull ESP EN high to enable Wi-Fi module */
  HAL_GPIO_WritePin(GPIOA, esp_en_Pin, GPIO_PIN_SET);

  /* Initialize display */
  GC9A01_Init();

  /* Set memory access control after init (MADCTL) */
  GC9A01_WriteCommand(0x36);
  GC9A01_WriteData(GC9A01_ROTATION);

  /* Self-test boot screen */
  boot_self_test();

  /* Initialize flash storage for persistent gauge index */
  flash_storage_init();

  /* Initialize LVGL and UI */
  lv_init();
  lv_port_disp_init();
  setup_ui(&guider_ui);
  custom_init(&guider_ui);

  /* Enable USART1 RX interrupt (byte-by-byte IT mode) */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (rx_ready) {
      ParseRxData();
    }

    uint8_t key_action = KeyProcess();
    if (key_action == 2) {
      power_toggle();
    } else if (key_action == 1 && is_power_on()) {
      gauge_next_page();
      update_gauge_display(&guider_ui);
    }

    gauge_process_pending_save();

    if (is_power_on()) {
      uint32_t time_till_next = lv_timer_handler();
      if (time_till_next > 5) time_till_next = 5; /* Cap at 5ms for more responsive UI */
      if (time_till_next < 1) time_till_next = 1;
      HAL_Delay(time_till_next);
    } else {
      /* In power-off state, slow down the loop to save power */
      HAL_Delay(50);
    }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 460800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);    /* PA2 = HIGH: key active-high pull-up */
  HAL_GPIO_WritePin(GPIOA, esp_en_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DC_Pin|CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, screen_RST_Pin|AT24C02_VCC_Pin|AT24C02_GND_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : key_Pin */
  GPIO_InitStruct.Pin = key_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(key_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 esp_en_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2|esp_en_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DC_Pin CS_Pin screen_RST_Pin */
  GPIO_InitStruct.Pin = DC_Pin|CS_Pin|screen_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : AT24C02_VCC_Pin */
  GPIO_InitStruct.Pin = AT24C02_VCC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AT24C02_VCC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AT24C02_GND_Pin */
  GPIO_InitStruct.Pin = AT24C02_GND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AT24C02_GND_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* Power on AT24C02 module (PB4 = VCC, PB5 = GND) */
  HAL_GPIO_WritePin(AT24C02_VCC_GPIO_Port, AT24C02_VCC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(AT24C02_GND_GPIO_Port, AT24C02_GND_Pin, GPIO_PIN_RESET);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  UART RX complete callback (byte-by-byte IT mode).
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance != USART1) {
    return;
  }

  uint8_t ch = uart_rx_byte;
  if (ch == '[') {
    rx_wr_idx = 0;
    rx_buffer[rx_wr_idx++] = ch;
  } else if (rx_wr_idx > 0 && rx_wr_idx < RX_BUF_SIZE) {
    rx_buffer[rx_wr_idx++] = ch;
    if (ch == ']') {
      rx_buffer[rx_wr_idx] = '\0';
      rx_ready = 1;
      rx_wr_idx = 0;
    }
  }

  HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
}

/**
  * @brief  Parse received data frame.
  * @retval None
  */
static void ParseRxData(void)
{
  rx_ready = 0;

  int load = 0, tmp = 0, rpm = 0, spd = 0, maf = 0, thr = 0;
  int n = sscanf((char *)rx_buffer,
                 "[LOAD=%d TMP=%d RPM=%d SPD=%d MAF=%d THR=%d]",
                 &load, &tmp, &rpm, &spd, &maf, &thr);

  if (n == 6) {
    gauge_set_value(GAUGE_LOAD, (int16_t)load);
    gauge_set_value(GAUGE_TMP,  (int16_t)tmp);
    gauge_set_value(GAUGE_RPM,  (int16_t)rpm);
    gauge_set_value(GAUGE_SPD,  (int16_t)spd);
    gauge_set_value(GAUGE_MAF,  (int16_t)maf);
    gauge_set_value(GAUGE_THR,  (int16_t)thr);

    /* Throttle UI updates to max ~30 fps to avoid overwhelming LVGL renderer */
    static uint32_t last_update = 0;
    uint32_t now = HAL_GetTick();
    if (now - last_update >= 33) {
      update_gauge_display(&guider_ui);
      last_update = now;
    }
  }
}

/**
  * @brief  Process key press, distinguish short and long press.
  * @retval 0 = none, 1 = short press, 2 = long press.
  */
static uint8_t KeyProcess(void)
{
  static uint32_t press_start = 0;
  static uint8_t last_pressed = 0;
  static uint8_t long_press_done = 0;
  uint8_t result = 0;

  uint32_t now = HAL_GetTick();
  uint8_t pressed = (HAL_GPIO_ReadPin(key_GPIO_Port, key_Pin) == GPIO_PIN_SET);

  if (pressed && !last_pressed) {
    press_start = now;
    long_press_done = 0;
    last_pressed = 1;
  } else if (pressed && last_pressed) {
    if (!long_press_done && (now - press_start >= 1000)) {
      result = 2; /* long press */
      long_press_done = 1;
    }
  } else if (!pressed && last_pressed) {
    if (!long_press_done && (now - press_start >= 30)) {
      result = 1; /* short press */
    }
    last_pressed = 0;
  }

  return result;
}

/**
  * @brief DMA Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();

  hdma_spi1_tx.Instance = DMA2_Stream3;
  hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
  hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi1_tx.Init.Mode = DMA_NORMAL;
  hdma_spi1_tx.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_spi1_tx);

  __HAL_LINKDMA(&hspi1, hdmatx, hdma_spi1_tx);

  /* USART1 RX DMA (DMA2 Stream2 Channel4) */
  hdma_usart1_rx.Instance = DMA2_Stream2;
  hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
  hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart1_rx.Init.Mode = DMA_NORMAL;
  hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_usart1_rx);

  __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
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
