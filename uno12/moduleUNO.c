#define LED_GREEN_ON HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
#define LED_GREEN_OFF HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
#include "stm32f4xx_hal.h"
#include "stdint.h"

#include "InitializationUNO_v1.h"

#include "FLASH_512.h"
#include "Flash_Handler.h"


#include "string.h"
#include "Flash_Handler.h"



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
	


	int main(void)
	{
		HAL_Init();
		SystemClock_Config();
	
		Initial_Led();


		FLASH_SPI_close();
		FLASH_SPI_open();
		

		while (Flash_ID_Check()) //while (0)  ЛОЖь
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);	
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
		//LED_GREEN_ON
			uint8_t data_flash[22];
			uint8_t data_flash_1[256];
		uint8_t data[4096];
		for (int i = 0; i < 22; i++)
			data_flash[i] = i;
		Sector_Erase_SE4B(0);
		Sector_Erase_SE4B(1);
		Sector_Erase_SE4B(2);
		Sector_Erase_SE4B(3);
		Sector_Erase_SE4B(4);
	//	Chip_Erase_CE();
		uint8_t sector_data[256];
		for (uint16_t i = 0; i < 256; i++)
			sector_data[i] = i;
		for (uint16_t i = 0; i < 300	; i++)
	
			setdata(data_flash);
		uint8_t key_1[1303];
		Getdatanames_sector(key_1, 0);
		///*Проверка*/
		memset(data, 0, sizeof(data));
		Read_DAta_Bytes_READ4B(0, data, 4096);
		/*HAL_Delay(1);
		Read_DAta_Bytes_READ4B(4096, data, 4096);
		HAL_Delay(1);*/

 		for (;;)
		{
			 
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
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4; //RCC_HCLK_DIV4 = 36 MHz
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; //RCC_HCLK_DIV2 = 72 MHz

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


//через массивы
////--------------------------------------
//typedef struct _Flash_Presel {
//	uint8_t     freq[5];
//	uint8_t      temp;
//	uint8_t		LNA;
//	uint8_t     inter_step[2];
//	uint8_t     dac01[3];
//	uint8_t     dac23[3];
//	uint8_t     dac45[3];
//	uint8_t     dac67[3];
//	uint8_t     digital_att;
//}_Flash_Presel_t;

//_Flash_Presel_t _Flash_Presel_t_mas[11];



//int save_page_data_1(uint8_t *data_in)
//{
//	/*статическая переменная сохраняет свое значение между вызовами,
//	а инициализация происходит только один раз */

//	static uint8_t i = 0;

//	if (i > 12)
//	{
//		static uint32_t adder_locate;//max = 0x3FFFF00
//		static uint16_t sector;		  //max = 0x3FFF
//		i = 0;
//		if (adder_locate % 4096 == 0)
//			sector++;
//		if (sector > 16383)
//		{
//			sector = 0;
//			return (presel_container_full);
//		}
//		FLASH_Page_Programm_PP(adder_locate, _Flash_Presel_t_mas); //запись во флэш
//		adder_locate += 256;
//		return (sector);
//	}
//	else
//	{
//		memcpy(&_Flash_Presel_t_mas[i], data_in, 22);
//		//_Flash_Presel_t_mas[i].freq[0] = data_in;
//		//pres_pack. = (preselector_pack_t*)data_in; //запись значений в массив структур
//		//как заполнить недостающие данные нулями???
//		i++;
//		return (presel_container_not_full);
//	}
//}

////------------------------




