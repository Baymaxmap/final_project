#include "main.h"
#include "fonts.h"
#include "ssd1306.h"
#include "sensor.h"
#include "gps.h"


ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC1_Init(void);

int main(void)
{

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();

  /*
  	-----STORE THE DATA TRANSMITTED FROM PMS5003 INTO dataPms-----
  */
    uint8_t dataPms[64];
    uint8_t copyDataPms[64];
    HAL_UART_Receive_DMA(&huart1, dataPms, sizeof(dataPms));
    HAL_Delay(500);

  /*
  	-----STORE DATA TRANSMITTED FROM L80-R INTO dataGps-----
  */
    uint8_t dataGps[500];
    uint8_t copyDataGps[500];
    HAL_UART_Receive_DMA(&huart2, dataGps, sizeof(dataGps));
    HAL_Delay(500);

  /*
  	-----INITIALIZE OLED-----
  */
    SSD1306_Init();
    HAL_Delay(100);
    oled_init();
  /*
  	-----DECLARE SENSOR PARA-----
  */
    int pm2_5;
    float temperature, humidity;
    double co;
    char strTemp[5],strHumi[5],strCO[5],strPms[5];

  /*
  	-----DECLARE GPS PARA-----
  */
    float latGps;
    float longGps;
    char strLat[9], strLong[10];
    while (1)
    {
  /*
  -----READ THE VALUE OF TEMP, HUMI AND PM2.5----
  */
  	  SHT31_ReadTempHumidity(&temperature, &humidity);
  	  pm2_5=getPM25(dataPms, copyDataPms, sizeof(dataPms));

  	  co=getCOValue();

  	  sprintf(strTemp,"%.1f",temperature);
  	  sprintf(strHumi,"%.1f",humidity);
  	  sprintf(strPms,"%d",pm2_5);
  	  sprintf(strCO,"%.1f",co);

  	  for(size_t i=strlen(strTemp);i<sizeof(strTemp);i++){
  		  strTemp[i]=' ';
  	  }
  	  for(size_t i=strlen(strHumi);i<sizeof(strHumi);i++){
  		  strHumi[i]=' ';
  	  }
  	  for(size_t i=strlen(strPms);i<sizeof(strPms);i++){
  		  strPms[i]=' ';
  	  }
  	  for(size_t i=strlen(strCO);i<sizeof(strCO);i++){
  		  strCO[i]=' ';
  	  }
/*
-----DELETE THE VALUE OF TEMP, HUMI, PM2.5 ON SCREEN-----
*/
/*
 	  SSD1306_GotoXY (45,0);
  	  HAL_Delay(50);
  	  SSD1306_Puts ("     ", &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,10);
  	  HAL_Delay(50);
  	  SSD1306_Puts ("     ", &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,20);
  	  HAL_Delay(50);
  	  SSD1306_Puts ("     ", &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,31);
  	  HAL_Delay(50);
  	  SSD1306_Puts ("     ", &Font_7x10, 1);
  	  HAL_Delay(50);
  	  SSD1306_UpdateScreen();
  	  HAL_Delay(100);
*/
  /*
  -----PUT THE VALUE OF TEMP, HUMI, PM2.5 ONTO SCREEN-----
  */
  	  SSD1306_GotoXY (45,0);
  	  HAL_Delay(50);
  	  SSD1306_Puts (strTemp, &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,10);
  	  HAL_Delay(50);
  	  SSD1306_Puts (strHumi, &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,20);
  	  HAL_Delay(50);
  	  SSD1306_Puts (strPms, &Font_7x10, 1);
  	  HAL_Delay(50);

  	  SSD1306_GotoXY (45,31);
  	  HAL_Delay(50);
  	  SSD1306_Puts (strCO, &Font_7x10, 1);
  	  HAL_Delay(50);
  	  SSD1306_UpdateScreen();


  	  char strSendLora[100]={0};
	  strcat(strSendLora,"temp:");
	  strcat(strSendLora,strTemp);
	  strcat(strSendLora,"\r\n");
	  strcat(strSendLora,"humi:");
	  strcat(strSendLora,strHumi);
	  strcat(strSendLora,"\r\n");
	  strcat(strSendLora,"CO gas:");
	  strcat(strSendLora,strCO);
	  strcat(strSendLora,"\r\n");
	  strcat(strSendLora,"dust:");
	  strcat(strSendLora,strPms);
	  strcat(strSendLora,"\r\n");
  /*
  	-----PROCESS THE GPS DATA AND SEND IT TO ESP BY LORA_E32-----
  */
  	  //take the times to check gps is 5 times maximum
  	  int timesCheckGps = 0;
  	  while(timesCheckGps<=5){
  		  memcpy(copyDataGps,dataGps,sizeof(copyDataGps));
  		  //get the latitude, if lat is got, then get the longtitude
  		  latGps=getLatGps(copyDataGps);
  		  if(latGps!=0){
  			  longGps=getLongGps(copyDataGps);
  			  //convert the float type of lat and long into string
  			  sprintf(strLat,"%.5f",latGps);
  			  sprintf(strLong,"%.5f",longGps);

  			  //put the string of lat and long onto screen
  			  SSD1306_GotoXY (45,42);
  			  HAL_Delay(50);
  			  SSD1306_Puts (strLat, &Font_7x10, 1);
  			  HAL_Delay(50);
  			  SSD1306_GotoXY (45,53);
  			  HAL_Delay(50);
  			  SSD1306_Puts (strLong, &Font_7x10, 1);
  			  HAL_Delay(50);
  			  SSD1306_UpdateScreen();
  			  timesCheckGps=5;

  			  //concatenate the string into strSendLora
  			  strcat(strSendLora,"latitude:");
  			  strcat(strSendLora,strLat);
  			  strcat(strSendLora,"\r\n");
  			  strcat(strSendLora,"longtitude:");
  			  strcat(strSendLora,strLong);
  			  strcat(strSendLora,"\r\n");

  			  //send gps data to lora-e32
  			  //HAL_UART_Transmit(&huart3, (uint8_t*)strSendLora, sizeof(strSendLora), HAL_MAX_DELAY);
  			  HAL_Delay(8000);
  		  }
  		  timesCheckGps+=1;
  	  }
  	  HAL_UART_Transmit(&huart3, (uint8_t*)strSendLora, sizeof(strSendLora), HAL_MAX_DELAY);
  	  HAL_Delay(6000); // delay 5s to see para on screen
    }

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hi2c1.Init.ClockSpeed = 400000;
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
  huart1.Init.BaudRate = 9600;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
