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

#pragma pack(push, 1)
	 struct  _PRES_KEY
	 {
		 uint8_t     freq[5];
		 int8_t      temp;
		 uint8_t		LNA;
	 };
#pragma pack(pop)
	 
#pragma pack(push ,1)
	 struct  _PRES_BODY
	 {
		 uint8_t     inter_step[2];
		 uint8_t     dac01[3];
		 uint8_t     dac23[3];
		 uint8_t     dac45[3];
		 uint8_t     dac67[3];
		 uint8_t     digital_att;
	 };
#pragma pack(pop)
#pragma pack(push ,1)
	 typedef struct  _PRES_PACK
	 {
		 struct  _PRES_KEY	key;
		 struct  _PRES_BODY	body;
	 }preselector_pack_t;

	 preselector_pack_t pact_t[11];

#define presel_container_not_full	 -1
#define presel_container_full	     -2

	 uint8_t data_in[22] = {0, 1, 2, 3, 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21}; //тестовые входные данные

   //заполнение контейнера и его  отправка
int save_page_data(uint8_t *data_in)
{
	/*статическая переменная сохраняет свое значение между вызовами,
	а инициализация происходит только один раз */
	static uint8_t i = 0;
	if (i > 10) 
	{
		static uint32_t adder_locate;//max = 0x3FFFF00
		static uint16_t sector;		  //max = 0x3FFF
		i = 0;
		if (adder_locate % 4096 == 0)
			sector++;
		if (sector > 16383)
		{
			sector = 0;
			return (presel_container_full);
		}
		uint8_t a[256];
		for (i = 242; i < 256; i++)
//			pact_t[i] = 0;
		memcpy(a, pact_t,256);
		FLASH_Page_Programm_PP(adder_locate, a); //запись во флэш
		adder_locate += 256;
		return (sector);
	}
	else
	{
		memcpy(&pact_t[i], data_in, 22 );
		//pres_pack. = (preselector_pack_t*)data_in; //запись значений в массив структур
		//как заполнить недостающие данные нулями???
		i++;
		return (presel_container_not_full);
	}
}

	int main(void)
	{
		HAL_Init();
		SystemClock_Config();
		Initial_Led();


		FLASH_SPI_close();
		FLASH_SPI_open();
		
		/*while (Flash_ID_Check()) {
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
			
		};
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);*/
		LED_GREEN_ON

		for (;;)
		{
			//save_page_data_1(data_in);
			save_page_data(data_in);
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




