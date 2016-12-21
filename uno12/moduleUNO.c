#include "stm32f4xx_hal.h"
#include "stdint.h"

#include "InitializationUNO_v1.h"

#include "FLASH_512.h"

#ifdef __cplusplus
extern "C" {
#endif



			

	void SystemClock_Config(void);
	void Error_Handler(void);



	int main(void)
	{
	

		HAL_Init();
		SystemClock_Config();

		
		uno_open(0);
		uno_set_profile(0, 5000, 0);
		uno_set_profile(0, 6000, 1);
		for (;;)
		{

			uno_read_profile(0);
			HAL_Delay(1000);
			uno_read_profile(1);
			HAL_Delay(1000);
		}
	}



	//void SystemClock_Config(void)
	//{

	//	RCC_OscInitTypeDef RCC_OscInitStruct;
	//	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	//	    /**Configure the main internal regulator output voltage 
	//	    */
	//	__HAL_RCC_PWR_CLK_ENABLE();

	//	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	//	    /**Initializes the CPU, AHB and APB busses clocks 
	//	    */
	//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	//	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	//	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	//	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	//	RCC_OscInitStruct.PLL.PLLM = 15; //15
	//	RCC_OscInitStruct.PLL.PLLN = 144; //144
	//	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV6; //6
	//	RCC_OscInitStruct.PLL.PLLQ = 5;  //5
	//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	//	{
	//		Error_Handler();
	//	}

	//	    /**Initializes the CPU, AHB and APB busses clocks 
	//	    */
	//	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	//	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	//	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	//	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	//	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	//	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	//	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	//	{
	//		Error_Handler();
	//	}

	//	    /**Configure the Systick interrupt time 
	//	    */
	//	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	//	    /**Configure the Systick 
	//	    */
	//	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	//	  /* SysTick_IRQn interrupt configuration */
	//	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	//}

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
