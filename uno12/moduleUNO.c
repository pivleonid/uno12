#define LED_GREEN_ON HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
#define LED_GREEN_OFF HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
#include "stm32f4xx_hal.h"
#include "stdint.h"

#include "InitializationUNO_v1.h"

#include "FLASH_512.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

			

	void SystemClock_Config(void);
	void Error_Handler(void);

	GPIO_InitTypeDef Led;
	 void Initial_Led(void)
	{
		 __GPIOE_CLK_ENABLE();
		Led.Pin = GPIO_PIN_1 | GPIO_PIN_9;
		Led.Mode = GPIO_MODE_OUTPUT_PP;
		Led.Pull = GPIO_NOPULL;
		Led.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &Led);
	}
	
//#define printf my_printf

	/*
	данные калибровки	
	*/
	 typedef struct Data_preselector
	 {
		 uint8_t data_1 : 2;
		 uint8_t data_2 : 2;
		 uint8_t data_3 : 5;
		 uint8_t data_4 : 1;
	 } Data_preselector_t;

	int main(void)
	{
		
		uint32_t k = sizeof(Data_preselector_t);
		uint32_t c, i = 0;
		uint8_t data_write[256];
		uint8_t  data_read[1000];
		for (i = 0; i < 256; i++)
		{
			data_write[i] = i;
		}
		for (i = 0; i < 1000; i++)
		{
			data_read[i] = i;
		}
		

		HAL_Init();
		SystemClock_Config();
		Initial_Led();


		FLASH_SPI_close();
		FLASH_SPI_open();
		
		while (Flash_ID_Check()) {
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
			
		};
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
		LED_GREEN_ON
	
	
		Sector_Erase_SE4B(0);
		
		Read_DAta_Bytes_READ4B(0xF12C, data_read);
		for (i = 0; i < 262128; i += 256)
		{
			FLASH_Page_Programm_PP(i, data_write);
			Read_DAta_Bytes_READ4B(i, data_read);
			if (memcmp(data_read, data_write, 256) != 0)
				 c = i;
		}

		for (;;)
		{
			
			while (Flash_ID_Check());
			Read_DAta_Bytes_READ4B(100, data_read);
			/*for (i = 0; i < 262128; i += 256)
			{
				Read_DAta_Bytes_READ4B(i, data_read);
			}*/
			
		}

	}



	void SystemClock_Config(void)
	{

		RCC_OscInitTypeDef RCC_OscInitStruct;
		RCC_ClkInitTypeDef RCC_ClkInitStruct;

		/**Configure the main internal regulator output voltage
		*/
		__HAL_RCC_PWR_CLK_ENABLE();

		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		/**Initializes the CPU, AHB and APB busses clocks
		*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState = RCC_HSE_ON;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLM = 25;
		RCC_OscInitStruct.PLL.PLLN = 288;
		RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
		RCC_OscInitStruct.PLL.PLLQ = 6;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			return;
		}

		/**Initializes the CPU, AHB and APB busses clocks
		*/
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
		{
			return;
		}

		/**Configure the Systick interrupt time
		*/
		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

		/**Configure the Systick
		*/
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		/* SysTick_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	}

	void SysTick_Handler(void)
	{
		HAL_IncTick();
		HAL_SYSTICK_IRQHandler();
	}



	void Error_Handler(void)
	{
	  /* USER CODE BEGIN Error_Handler */
	  /* User can add his own implementation to report the HAL error return state */
		while (1) 
		{
		}
		/* USER CODE END Error_Handler */ 
	}


#ifdef __cplusplus
}
#endif









