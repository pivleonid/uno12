
/*!  \file 	FLASH_512.c
 *   \brief  
 *   \details  
 * Режимы fast read не реализовывал- т.к. 20 МГц хватает за глаза- и так сигнал искажается
 * Fast Boot - после включения- автоматическая реализация чтения
 *             
 */

/*includes==========================================================================================================*/
#include "FLASH_512.h"
#include "stm32f4xx_hal.h"

/*defines===========================================================================================================*/
#define FLASH_OK  (0 )
#define ERR_FLASH (-1)
#define Err_ID    (-2)

#define Chip_Select_Down HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, RESET);
#define Chip_Select_Up  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, SET);


/*types=============================================================================================================*/
uint8_t data_reseive[6];
/*prototypes========================================================================================================*/
static void Write_Enable_WREN(void);
static void Write_Disable_WRDI(void);
static void Write_Erase_Complete(void);
static void Enable_4B(void);

/*variables=========================================================================================================*/
SPI_HandleTypeDef hspi5; 
GPIO_InitTypeDef GPIO_InitStruct; 

/*code==============================================================================================================*/
/*=============================================================================================================*/
/*!  \brief Функция инициализации переферии SPI и настройка на 4 байтный режим адресации
\return
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_SPI_open(void)
{  
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_SPI5_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, RESET);
	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; //2
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi5.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi5);
		
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	Chip_Select_Up 

	Enable_4B();
	
}	
 int Flash_ID_Check( void)
{
	FLASH_REMS();
	if (   (data_reseive[0] == 0xC2) && (data_reseive[1] == 0x19)
		|| (data_reseive[2] == 0xC2) && (data_reseive[3] == 0x19)
		|| (data_reseive[0] == 0x19) && (data_reseive[1] == 0xC2)
		|| (data_reseive[2] == 0x19) && (data_reseive[3] == 0xC2))
		return FLASH_OK;
	else
	{
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
		return Err_ID;
	}
		
}

/*=============================================================================================================*/
/*!  \brief Выключение тактирования SPI
\return
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_SPI_close(void)
{
		__HAL_RCC_SPI5_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}
/*=============================================================================================================*/
/*!  \brief Разрешение записи
\return
\retval
\sa
*/
/*=============================================================================================================*/
static void Write_Enable_WREN(void)
{
	uint8_t adder = 0x06;
	while ((FLASH_Read_Status_Register_RDSR() & 0x02) != 0x02); //WEL = 1 выход из while
	{
		Chip_Select_Down
		HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
		Chip_Select_Up
	}
}
/*=============================================================================================================*/
/*!  \brief Проверка бита завершения записи
Пока бит WIP не станет равен нулю- зависааем в этой функции
\return
\retval
\sa
*/
/*=============================================================================================================*/
static void Write_Erase_Complete(void) {
	while ((FLASH_Read_Status_Register_RDSR() & 0x01)); //WIP = 0 выход из while
}
/*=============================================================================================================*/
/*!  \brief Запись страницы
 *			Время записи приблизительно 0.6 - 3 мс
\return void
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_Page_Programm_PP(uint32_t address, uint8_t flash_data_in[256])
{
	Write_Enable_WREN();
	uint8_t data_adder[5];
	data_adder[0] = 0x12;
	data_adder[1] = address >> 24;
	data_adder[2] = address >> 16;
	data_adder[3] = address >> 8;
	data_adder[4] = address;
	Chip_Select_Down
		HAL_SPI_Transmit(&hspi5, data_adder, 5, 1);
	HAL_SPI_Transmit(&hspi5, flash_data_in, 256, 1);
	Chip_Select_Up
		Write_Erase_Complete();
}
/*=============================================================================================================*/
/*!  \brief Чтение size байт (max 65535), начиная с указанного адреса address
\return void
\retval
\sa
*/
/*=============================================================================================================*/
void Read_DAta_Bytes_READ4B(uint32_t address, uint8_t* Flash_data_out, uint16_t size)
{
	uint8_t data[5];
	data[0] = 0x13;
	data[1] = address >> 24;
	data[2] = address >> 16;
	data[3] = address >> 8;
	data[4] = address;
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 5, 1);
	HAL_SPI_Receive(&hspi5, Flash_data_out, size, 1000);
	Chip_Select_Up
}
/*=============================================================================================================*/
/*!  \brief Очистка сектора.
 *  стирание начнется, после того, как CS = 1
 *  WIP = 0, когда операция завершится
 *  43 - 200 мс
\return void
\retval
\sa
*/
/*=============================================================================================================*/
void Sector_Erase_SE4B(uint32_t SectorAddr)
{
	SectorAddr *= 0x1000;
	uint8_t data[5];
	data[0] = 0x21;
	data[1] = SectorAddr >> 24;
	data[2] = SectorAddr >> 16;
	data[3] = SectorAddr >> 8;
	data[4] = SectorAddr;
	Write_Enable_WREN();
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 5, 1);
	Chip_Select_Up
	Write_Erase_Complete();
}
/*=============================================================================================================*/
/*!  \brief Очистка блока
 *  проверить статусные регистры
 *  0.19 - 1 секунды
\return void
\retval
\sa
*/
/*=============================================================================================================*/
void Block_Erase_32(uint32_t address)
{
	uint8_t data[5];
	data[0] = 0x5C;
	data[1] = address >> 24;
	data[2] = address >> 16;
	data[3] = address >> 8;
	data[4] = address;
	Write_Enable_WREN();
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 4, 1);
	Chip_Select_Up
	Write_Erase_Complete();
}
/*=============================================================================================================*/
/*!  \brief Очистка блока
*  проверить статусные регистры
*  0.34 - 2 секунды
\return void
\retval
\sa
*/
/*=============================================================================================================*/
void Block_Erase_64(uint32_t address)
{
	uint8_t data[5];
	data[0] = 0xDC;
	data[1] = address >> 24;
	data[2] = address >> 16;
	data[3] = address >> 8;
	data[4] = address;
	Write_Enable_WREN();
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 4, 1);
	Chip_Select_Up
	Write_Erase_Complete();
}
/*=============================================================================================================*/
/*!  \brief Очистка Чипа
*  
*   от 240 - 600 секунд
\return void
\retval
\sa
*/
/*=============================================================================================================*/

void Chip_Erase_CE(void)
{
	Write_Enable_WREN();
	Chip_Select_Down
	uint8_t adder = 0x60;
	HAL_SPI_Transmit(&hspi5, &adder, 1, 1);
	Chip_Select_Up
	Write_Erase_Complete();
}


/*=============================================================================================================*/
/*!  \brief Разрешение 4 байтного режима записи
\return
\retval
\sa
*/
/*=============================================================================================================*/
static void Enable_4B(void)
{
	Chip_Select_Down
		uint8_t adder = 0xB7;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	Chip_Select_Up
}

/*=============================================================================================================*/
/*!  \brief
Read Configuration Register
\return
\retval
\sa
*/
/*=============================================================================================================*/
uint8_t FLASH_Read_Configuration_Register_RDCR(void)
{
	Chip_Select_Down
		uint8_t adder = 0x15;
	uint8_t data_reseive;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	HAL_SPI_Receive(&hspi5, &data_reseive, 1, 1);
	Chip_Select_Up
		return (data_reseive);

}

//максимальное время записи 40 мс 
void FLASH_Write_Status_Configuration_Register_WRSR(uint8_t status_reg, uint8_t config_reg)
{
	Chip_Select_Down
		uint8_t data[3] = { 0x01, status_reg, config_reg };
	HAL_SPI_Transmit(&hspi5, data, 3, 1);
	Chip_Select_Up
}


/*=============================================================================================================*/
/*!  \brief
Read Identification
outputs JEDEC ID: 1-byte Manufacturer
ID & 2-byte Device ID
\return
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_RDID(void)
{
	Chip_Select_Down
	uint8_t adder = 0x9F;
	uint8_t data_reseive[3];
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	HAL_SPI_Receive(&hspi5, data_reseive, 3, 1);
	Chip_Select_Up
}
/*=============================================================================================================*/
/*!  \brief
to read out
1-byte Device
ID
\return
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_RES(void)
{
	Chip_Select_Down
		uint8_t adder = 0xAB;
	uint8_t data_reseive[1];
	uint8_t DummyByte = 0;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	HAL_SPI_Transmit(&hspi5, &DummyByte, 3, 1);
	HAL_SPI_Receive(&hspi5, data_reseive, 1, 1);
	HAL_Delay(1);
	Chip_Select_Up
}
/*=============================================================================================================*/
/*!  \brief
output the
Manufacturer
ID & Device ID
\return
\retval
\sa
*/
/*=============================================================================================================*/
void FLASH_REMS(void)
{
	Chip_Select_Down
	uint8_t adder[3] = { 0x90,0, 0 };
	HAL_SPI_Transmit(&hspi5, &adder, 2, 1);
	HAL_SPI_Receive(&hspi5, data_reseive, 6, 1);
	Chip_Select_Up
}
/*=============================================================================================================*/
/*!  \brief
Read Status Register (RDSR)
\return
\retval
\sa
*/
/*=============================================================================================================*/
uint8_t FLASH_Read_Status_Register_RDSR(void)
{
	Chip_Select_Down
		uint8_t adder = 0x05;
	uint8_t data_reseive;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	HAL_SPI_Receive(&hspi5, &data_reseive, 1, 1);
	Chip_Select_Up
		return (data_reseive);
}
/*=============================================================================================================*/
/*!  \brief Запрет записи
\return
\retval
\sa
*/
/*=============================================================================================================*/
static void Write_Disable_WRDI(void)
{
	Chip_Select_Down
		uint8_t adder = 0x04;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	Chip_Select_Up
}