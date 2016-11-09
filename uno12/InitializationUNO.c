

#include "stm32f4xx_hal.h"
#include "InitializationUNO.h"
//#include "stm32f4xx_hal_spi.h"
#include "stm32f427xx.h"

#define StrobUno_ON HAL_GPIO_WritePin( GPIO_X, GPIO_PIN, GPIO_PIN_SET );
#define StrobUno_OFF HAL_GPIO_WritePin( GPIO_X, GPIO_PIN, GPIO_PIN_RESET );

#define UNO_OK (0)
#define ERR_UNO_Pow (-1)
#define ERR_UNO_PWR_Down (-2)



//Статичные функции\\
uint8_t func_power(float fr_out);
static float Funk_fr_vco2(uint8_t n_pow, float fr_out);
static uint8_t Search_K(float);


int outputState;

SPI_HandleTypeDef hspi5; // первый синтезатор--//
SPI_HandleTypeDef hspi6; // второй синтезатор--//
SPI_HandleTypeDef hspix;
GPIO_InitTypeDef GPIO_InitStruct; 
GPIO_TypeDef* GPIO_X;
uint16_t GPIO_PIN;


//Настройки SPi с тактированием и настройкой пинов\\



//uno_index = 0 или 1
int uno_open(uint8_t uno_index)
{
	//выбор синтезатора\\
	if (uno_index == 0)
	{
		
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_SPI5_CLK_ENABLE();
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_6, RESET);
		//--------------------------//
		hspi5.Instance = SPI5;
		hspi5.Init.Mode = SPI_MODE_MASTER;
		hspi5.Init.Direction = SPI_DIRECTION_2LINES;
		hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi5.Init.NSS = SPI_NSS_SOFT;
		hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
		hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi5.Init.CRCPolynomial = 10;
		if (HAL_SPI_Init(&hspi5) != HAL_OK)
		{
			//Error_Handler();
		}
		GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		//--------------------------//
		hspix = hspi5;
		GPIO_X = GPIOF;
		GPIO_PIN = GPIO_PIN_6;
		
		//--------------------------//
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		StrobUno_ON
	}
	if(uno_index == 1)
	{
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_SPI6_CLK_ENABLE();
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, RESET);
		//-------------------------//
		hspi6.Instance = SPI6;
		hspi6.Init.Mode = SPI_MODE_MASTER;
		hspi6.Init.Direction = SPI_DIRECTION_2LINES;
		hspi6.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi6.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi6.Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi6.Init.NSS = SPI_NSS_SOFT;
		hspi6.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		hspi6.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi6.Init.TIMode = SPI_TIMODE_DISABLE;
		hspi6.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi6.Init.CRCPolynomial = 10;
		HAL_SPI_Init(&hspi6);
//		if (HAL_SPI_Init(&hspi6) != HAL_OK) //посмотреть как будет вызываться эта функция. Будет ли она вообще вызываться
//		{
//			Error_Handler();
//		}
//	  
		GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
		//-------------------------//
		hspix = hspi6;
		GPIO_X = GPIOG;
		GPIO_PIN = GPIO_PIN_15;
		//-------------------------//
		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
		//-------------------------//
		StrobUno_ON

		
		
	}
	
		
		uint8_t n_pow = 0;
		uint8_t n_pow_1 = 0;
		uint8_t n_pow_2 = 0;
		//----------------------------------//
		uint8_t hb_gain = 0x00;// усиление- 0дБ
		uint8_t hb_gain_1 = 0x00;
		uint8_t hb_gain_2 = 0x00;
		//----------------------------------//
	   // Значение регистров петли ФАПЧ вычисление не имеет смысла, т.к. ref_clk_ext_en = 0; нет внешнего тактового генератора
		uint16_t refpll_r = 0;
		uint16_t refpll_n = 0;
		uint8_t address; 
		uint32_t address_32;
		//---------------------------------//
		//1//
		address = 0x01;
		StrobUno_OFF 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x01; //0x0D
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//2//
		HAL_Delay(100);
		//3//
		address = 0x02;
		StrobUno_OFF 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x07; //0x07-> 111    // 0x06 -> 110  Настройка усилителя
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
	
		//4//
		address = 0x03; //задание делителя на выходе
		StrobUno_OFF 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x10); 
		address = (0x00 | n_pow); //задание делителя на выходе 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x10);
		StrobUno_ON
		//5//
		address = (0x04 | hb_gain_1); //задать усиление сигнала
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = (0x00 | hb_gain_2); //3F усиление на  31.5 дБ
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//6//
		address = 0x60; //address = 0x60038013; //два раза по 16 бит
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x03;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x80;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x13;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		StrobUno_ON
		//7////
		address = 0x60; //0x60038012;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x03;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x80;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x12;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		StrobUno_ON
		//8//
		address_32 = (0x60000000 | refpll_r << 2); //загрузка фазового детектора ФАПЧ опорного сигнала    
		uint8_t Fapch1 = (uint8_t)address_32;  //Fapch1 младший разряд  
		uint8_t Fapch2 = (uint8_t)(address_32 >> 8);    
		uint8_t Fapch3 = (uint8_t)(address_32 >> 16);   
		uint8_t Fapch4 = (uint8_t)(address_32 >> 24);
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &Fapch4, sizeof(Fapch4), 0x1); 
		HAL_SPI_Transmit(&hspix, &Fapch3, sizeof(Fapch3), 0x1);
		HAL_SPI_Transmit(&hspix, &Fapch2, sizeof(Fapch2), 0x1);
		HAL_SPI_Transmit(&hspix, &Fapch1, sizeof(Fapch1), 0x1);
		StrobUno_ON
		//9//
		address_32 = (0x60000001 | refpll_n << 8); //загрузка фазового детектора ФАПЧ опорного сигнала
		Fapch1 = (uint8_t)address_32;  //Fapch1 младший разряд  
		Fapch2 = (uint8_t)(address_32 >> 8);    
		Fapch3 = (uint8_t)(address_32 >> 16);   
		Fapch4 = (uint8_t)(address_32 >> 24);
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &Fapch4, sizeof(Fapch4), 0x1); 
		HAL_SPI_Transmit(&hspix, &Fapch3, sizeof(Fapch3), 0x1);
		HAL_SPI_Transmit(&hspix, &Fapch2, sizeof(Fapch2), 0x1);
		HAL_SPI_Transmit(&hspix, &Fapch1, sizeof(Fapch1), 0x1);
		StrobUno_ON
		//10//включение  DDS
		address = 0x05; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//11//
		address = 0x15; //применение данных регистра DDSCTL
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//12//
		HAL_Delay(100);
		//13//сброс DDS
		address = 0x05; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x03;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//14//
		address = 0x15; //применение данных регистра DDSCTL
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//15//
		HAL_Delay(10);
		//16//
		address = 0x05; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//17//
		address = 0x15; //применение данных регистра DDSCTL
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//18//
		HAL_Delay(10);
		//19//
		address = 0x05; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x05;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//20//
		//21//
		//22//
		//23//
		//24//
		//25//          0x100000010102; // CFR1 разрешение глубокого выключения
		address = 10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 02;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
    
		//26//
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//27//
		address = 0x05; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//28//
		address = 0x15; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//29//
		HAL_Delay(10);
		//30//      
		Fapch2 = 0x05;
		Fapch1 = 0x01;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &Fapch2, sizeof(Fapch2), 0x1); 
		HAL_SPI_Transmit(&hspix, &Fapch1, sizeof(Fapch1), 0x1);
		StrobUno_ON
		//31//
		address = 0x15; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//32//
		HAL_Delay(100);
		//33//      Проверка на режим вычисления частоты
		//    if ( T == usualFreq )
		//        {
		        //0x100100890B00
		        //	address = 0x10;
		        //	StrobUno_OFF  
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 ); 
		        //	address = 0x01;
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		        //	address = 0x00;
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		        //	address = 0x89;
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		        //	address = 0x0B;
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		        //	address = 0x00;
		        //	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		        //	StrobUno_ON
		        	//   //     }
		        	//   // else if ( T == quickFreq )
		        	//    //    {
		        	       // 0x10010080B000
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x80;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xB0;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//34//
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//35//      0x100200000000
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x02;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//36//     
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON  
		//37//      0x100301052120
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x03;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x01;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x05;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x21;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x20;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//38//     
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//39//
		HAL_Delay(100);
		//40//      0x100300052120
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x03;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x05;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x21;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x20;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//41//     
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//42//      0x100C0FFF0000  (profile 0)
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x0C;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x0F;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xFF;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//43//      0x100E0FFF0000  (profile 1)
		//    address = 0x10;
		//    StrobUno_OFF  
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 ); 
		//    address = 0x0E;
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		//    address = 0x0F;
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		//    address = 0xFF;
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		//    address = 0x00;
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		//    address = 0x00;
		//    HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0x1 );
		//    StrobUno_ON
		                    //-------------------------//
		                    //----Profile (2-7)--------//
		                    //-------------------------//
    
		//50//
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//51//      0x10040BA2E8BA
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x04;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x0B;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xA2;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xE8;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xBA;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//52//      0x1006C66F5E22
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x06;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xC6;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x6F;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x5E;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x22;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//53//      0x10054364C5BB
		address = 0x10;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x05;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x43;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x64;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xC5;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0xBB;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//54//
		address = 0x11; 
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//55//      0x620000000B
		address = 0x62;
		StrobUno_OFF  
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x0C;//B
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
	StrobUno_ON
	
//return (OK);
//----------Обработка ошибок--------//
	uint8_t uno_answer;
	address = 0x81;
	StrobUno_OFF 
	HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
	HAL_SPI_Receive(&hspix, &uno_answer, sizeof(address), 0x1);
	StrobUno_ON
	if(uno_answer != 0x81)
	return ERR_UNO_Pow;
	return UNO_OK;
}
//--Выключение синтезатора--//
//--uno_index = 0 или 1--//
int uno_close (uint8_t uno_index)
{
		if (uno_index == 0)
	{
		hspix = hspi5;
		GPIO_X = GPIOF;
		GPIO_PIN = GPIO_PIN_6;
	}
	if(uno_index == 1)
	{
		hspix = hspi6;
		GPIO_X = GPIOG;
		GPIO_PIN = GPIO_PIN_15;
		
	}
	uint8_t address = 0x01;
	StrobUno_OFF 
	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0xFF ); 
	address = 0x00;
	HAL_SPI_Transmit( &hspix, &address, sizeof( address ), 0xFF );
	StrobUno_ON
	//----------Обработка ошибок--------//
	uint8_t uno_answer;
	address = 0x81;
	StrobUno_OFF 
	HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
	HAL_SPI_Receive(&hspix, &uno_answer, sizeof(address), 0x1);
	StrobUno_ON
	if(uno_answer != 0) // модуль выкл
	outputState = ERR_UNO_PWR_Down;
	outputState = UNO_OK;
	//деинициализация spi
	//HAL_SPI_DeInit(&hspix);
	//SS пины на вход
	if (uno_index == 0){
	GPIO_InitStruct.Pin = GPIO_PIN_6; 
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    //----------------------------------//
		__HAL_RCC_SPI5_CLK_DISABLE();
  
	    /**SPI5 GPIO Configuration    
	    PF7     ------> SPI5_SCK
	    PF8     ------> SPI5_MISO
	    PF9     ------> SPI5_MOSI 
	    */
		HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
	}
	
	if(uno_index == 1){
	GPIO_InitStruct.Pin = GPIO_PIN_15;  
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    //------------------------------------//
		__HAL_RCC_SPI6_CLK_DISABLE();
  
    /**SPI6 GPIO Configuration    
    PG12     ------> SPI6_MISO
    PG13     ------> SPI6_SCK
    PG14     ------> SPI6_MOSI 
    */
		HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
	}
	return (outputState);
}
/*uno_index, - переменная идентификатор конкретного синтезатора 1 или 2
freq, - желаемая частота на выходе синтезатора
gain - желаемое усиление или амплитуда на выходе синтезатора*/
int uno_write (uint8_t uno_index, float freq, uint8_t gain)
{
	float fr_out = freq;
	uint8_t dB = gain;
	uint8_t n_pow;

	if (uno_index == 0)
	{
		hspix = hspi5;
		GPIO_X = GPIOF;
		GPIO_PIN = GPIO_PIN_6;
	}
	if(uno_index == 1)
	{
		hspix = hspi6;
		GPIO_X = GPIOG;
		GPIO_PIN = GPIO_PIN_15;
	}

	if (fr_out > 6070) n_pow = 0;
	else if (fr_out > 3035) n_pow = 1;
	else if (fr_out > 1517.5) n_pow = 2;
	else if (fr_out > 758.75) n_pow = 3;
	else if (fr_out > 379.375) n_pow = 4;
	else if (fr_out > 189.6875) n_pow = 5;
	else  n_pow = 6;

	//n_pow =	 func_power(fr_out);
	float fr_vco2 =  Funk_fr_vco2(n_pow, fr_out);  
	uint8_t K = Search_K(fr_vco2);
	float fr_dds = 1200 - fr_vco2 / K;
	float ftw = roundf(fr_dds*powf(2, 32) / 2400);
	uint32_t ftw_1 = (uint32_t)ftw;
	uint8_t ftw1 = (uint8_t)ftw_1;          //ftw1 младший разряд  
	uint8_t ftw2 = (uint8_t)(ftw_1 >> 8);    
	uint8_t ftw3 = (uint8_t)(ftw_1 >> 16);   
	uint8_t ftw4 = (uint8_t)(ftw_1 >> 24);  //ftw4 старший разряд
	//придумать перевод ftw в 8 битный режим
	//--Устанавливаем значения в синтезатор--//
	//--1--//
	uint8_t address = 0x10;
	StrobUno_OFF
	{
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = 0x0B; 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		HAL_SPI_Transmit(&hspix, &ftw4, sizeof(address), 0x1);
		HAL_SPI_Transmit(&hspix, &ftw3, sizeof(address), 0x1);
		HAL_SPI_Transmit(&hspix, &ftw2, sizeof(address), 0x1);
		HAL_SPI_Transmit(&hspix, &ftw1, sizeof(address), 0x1);
		StrobUno_ON
    
		//HAL_Delay( 10 );
		//--2--//
		address = 0x03;
		StrobUno_OFF; 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = (0x00 | n_pow); // вроде все верно 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON
		//-------------------игра с усилением----------------------//
		address = 0x04;
		StrobUno_OFF
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1); 
		address = dB;         //3F max
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		StrobUno_ON

			//------------------конец усиления-------------------------//
			//HAL_Delay( 10 );
			//--3--//
		address = 0x62;
		StrobUno_OFF
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00; 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00;
		; 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = 0x00; 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
		address = K; //(uint8_t)K; // вроде все верно 
		HAL_SPI_Transmit(&hspix, &address, sizeof(address), 0x1);
	}
	StrobUno_ON
	return UNO_OK;

}

//uint8_t func_power(float fr_out)
// { 
// uint8_t n_pow;
// //поиск n_pow показатель степени делител§; fr_out частота на выходе uno
//	if (fr_out > 6070) n_pow = 0;
//	 else if (fr_out > 3035) n_pow = 1;
//	 else if (fr_out > 1517.5) n_pow = 2;
//	 else if (fr_out > 758.75) n_pow = 3;
//	 else if (fr_out > 379.375) n_pow = 4;
//	 else if (fr_out > 189.6875) n_pow = 5;
//	 else  n_pow = 6;
//	 return n_pow;
//}
 
	        //  float fr_vco2 //
static float Funk_fr_vco2(uint8_t n_pow, float fr_out)
	{
    
	 float fr_vco2 = fr_out * powf( 2, n_pow );
	 return fr_vco2;
	}
    
	            //int  k//

static uint8_t Search_K(float fr_vco2)
	{
   uint8_t K;
	if ( fr_vco2 < 7020 ) K = 6; 
	else if ( fr_vco2 < 8190 ) {
		K = 7;
		if ( (fr_vco2 >= 7068) && (fr_vco2 <= 7080) ) K = 6;
		if ( (fr_vco2 >= 8030) && (fr_vco2 <= 8062) ) K = 8;    
		}
	else if ( fr_vco2 < 9360 ) {
		K = 8;
		if ( (fr_vco2 >= 9172) && (fr_vco2 <= 9175) ) K = 9;
		if ( (fr_vco2 >= 9181) && (fr_vco2 <= 9184) ) K = 9;
		if ( (fr_vco2 >= 9190) && (fr_vco2 <= 9220) ) K = 9;
		}
	else if ( fr_vco2 < 10530 ) {
		K = 9;
		if ( (fr_vco2 >= 9390) && (fr_vco2 <= 9393) ) K = 8;
		if ( (fr_vco2 >= 9435) && (fr_vco2 <= 9464) ) K = 8;
		if ( (fr_vco2 >= 10315) && (fr_vco2 <= 10360) ) K = 10;
		if ( (fr_vco2 >= 10366) && (fr_vco2 <= 10370) ) K = 10;
		}
	else if ( fr_vco2 < 11700 ) {
		K = 10;
		if ( (fr_vco2 >= 11019) && (fr_vco2 <= 11022) ) K = 11;
		if ( (fr_vco2 >= 11038) && (fr_vco2 <= 11042) ) K = 11;
		if ( (fr_vco2 >= 11057) && (fr_vco2 <= 11060) ) K = 11;
		if ( (fr_vco2 >= 11465) && (fr_vco2 <= 11469) ) K = 11;
		if ( (fr_vco2 >= 11476) && (fr_vco2 <= 11480) ) K = 11;
		if ( (fr_vco2 >= 11485) && (fr_vco2 <= 11512) ) K = 11;
		if ( (fr_vco2 >= 11519) && (fr_vco2 <= 11521) ) K = 11;
		if ( (fr_vco2 >= 11528) && (fr_vco2 <= 11531) ) K = 11;
		if ( (fr_vco2 >= 11545) && (fr_vco2 <= 11549) ) K = 11;
		if ( (fr_vco2 >= 11650) && (fr_vco2 <= 11687) ) K = 11;
		}
	else if ( fr_vco2 < 12870 ) {
		K = 11;
		if ( (fr_vco2 >= 12050) && (fr_vco2 <= 12096) ) K = 12;
		if ( (fr_vco2 >= 12098) && (fr_vco2 <= 12124) ) K = 12;
		if ( (fr_vco2 >= 12142) && (fr_vco2 <= 12187) ) K = 12;
		if ( (fr_vco2 >= 12372) && (fr_vco2 <= 12378) ) K = 12;
		if ( (fr_vco2 >= 12584) && (fr_vco2 <= 12588) ) K = 12;
		if ( (fr_vco2 >= 12612) && (fr_vco2 <= 12615) ) K = 12;
		if ( (fr_vco2 >= 12635) && (fr_vco2 <= 12665) ) K = 12;
		if ( (fr_vco2 >= 12670) && (fr_vco2 <= 12684) ) K = 12;
		if ( (fr_vco2 >= 12700) && (fr_vco2 <= 12703) ) K = 12;
		if ( (fr_vco2 >= 12812) && (fr_vco2 <= 12856) ) K = 12;
		}
	else {
		K = 12;
		if ( (fr_vco2 >= 12958)&&(fr_vco2 <= 12856) ) K = 11;
		}
        
	return K;
	}










