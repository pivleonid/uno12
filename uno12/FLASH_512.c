
/*!  \file 	filename.c
 *   \brief  
 *   \details  
 *             
 */

/*includes==========================================================================================================*/
#include "FLASH_512.h"
#include "stm32f4xx_hal.h"
//#include "stm32f427xx.h"

/*defines===========================================================================================================*/
#define FLASH_OK (0)
#define ERR_FLASH (-1)

#define Chip_Select_Down HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, RESET);
#define Chip_Select_Up  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, SET);

/*types=============================================================================================================*/

/*prototypes========================================================================================================*/

/*variables=========================================================================================================*/
SPI_HandleTypeDef hspi5; 
GPIO_InitTypeDef GPIO_InitStruct; 


/*code==============================================================================================================*/
void FLASH_SPI_open(void)
{
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_SPI5_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, RESET);
	//--------------------------//
	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; //2
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
	//--------------------------//
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	Chip_Select_Up 
}	
		


void FLASH_SPI_close (void)
{

    //----------------------------------//
		__HAL_RCC_SPI5_CLK_DISABLE();
	    /**SPI5 GPIO Configuration    
	    PF7     ------> SPI5_SCK
	    PF8     ------> SPI5_MISO
	    PF9     ------> SPI5_MOSI 
	    */
		HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}

//Запись разрешена
static void Write_Enable_WREN(void)
{
	Chip_Select_Down
	uint8_t adder = 0x06;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	Chip_Select_Up 
}
//Запись запрещена
static void Write_Disable_WRDI(void)
{
	Chip_Select_Down
	uint8_t adder = 0x04;
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	Chip_Select_Up 
}
/*
Read Identification 
outputs JEDEC ID: 1-byte Manufacturer 
ID & 2-byte Device ID
*/
void FLASH_RDID(void)
{
	Chip_Select_Down
	uint8_t adder = 0x9F;
	uint8_t data_reseive[3];
	HAL_SPI_Transmit(&hspi5, &adder, sizeof(adder), 1);
	HAL_SPI_Receive(&hspi5, data_reseive, 3, 1);
	Chip_Select_Up 
	HAL_Delay(1);
}
    
/*
to read out 
1-byte Device 
ID
*/
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
/*
output the 
Manufacturer 
ID & Device ID
*/
void FLASH_REMS(void)
{
	Chip_Select_Down
	uint8_t adder = 0x90;
	uint8_t DummyByte = 0;
	uint8_t data_reseive[2];
	HAL_SPI_Transmit(&hspi5, &adder, 1, 1);
	HAL_SPI_Transmit(&hspi5, &DummyByte, 3, 1);
	HAL_SPI_Receive(&hspi5, data_reseive, 2, 1);
	Chip_Select_Up 
	HAL_Delay(1);
}

// Read Configuration Register 

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
// Read Status Register (RDSR)
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



//9-9 не проверял работу
// 40 мс - max
void FLASH_Write_Status_Configuration_Register_WRSR(uint8_t status_reg, uint8_t config_reg)
{
	Chip_Select_Down
	uint8_t data[3] = { 0x01, status_reg, config_reg };
	HAL_SPI_Transmit(&hspi5, data, 3, 1);
	Chip_Select_Up 
}

// какой адрес писать? не понятно.. сколько принимает? не написано
void Read_DAta_Bytes_READ(uint32_t address)
{
	uint8_t adder = 0x03;
	uint8_t data[4];
	data[0] = adder;
	data[1] = address >> 8;
	data[2] = address >> 16;
	data[3] = address >> 24;
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 4, 1);
	HAL_SPI_Receive(&hspi5, Flash_data_out, 1024, 10);
	Chip_Select_Up 
	HAL_Delay(1); // проверить хватит ли 1 мс
}
 // Режимы fast read не реализовывал- т.к. 20 МГц хватает за глаза- и так сигнал искажается
 // Fast Boot - после включения- автоматическая реализация чтения
 
 //стирание начнется, после того, как CS = 1
 //WIP = 0, когда операция завершится
 // 43 - 200 мс
void Sector_Erase_SE(uint8_t address_MSB, uint8_t address, uint8_t address_LSB)
{
	Write_Enable_WREN();
	Chip_Select_Down
	uint8_t adder = 0x20;
	HAL_SPI_Transmit(&hspi5, &adder, 1, 1);
	HAL_SPI_Transmit(&hspi5, &address_MSB, 1, 1);
	HAL_SPI_Transmit(&hspi5, &address, 1, 1);
	HAL_SPI_Transmit(&hspi5, &address_LSB, 1, 1);
	Chip_Select_Up 
	HAL_Delay(200);
}
// проверить статусные регистры
// 0.19 - 1 секунды
void Block_Erase_BE32K(uint32_t address)
{
	uint8_t adder = 0x52;
	uint8_t data[4];
	data[0] = adder;
	data[1] = address >> 8;
	data[2] = address >> 16;
	data[3] = address >> 24;
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 4, 1);
	Chip_Select_Up 
}
// 0.34 - 2 секунды
// проверить статусные регистры
void Block_Erase(uint32_t address)
{
	uint8_t adder = 0xD8;
	uint8_t data[4];
	data[0] = adder;
	data[1] = address >> 8;
	data[2] = address >> 16;
	data[3] = address >> 24;
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data, 4, 1);
	Chip_Select_Up 
}
// от 240 - 600 секунд
void Chip_Erase_CE(void)
{
	Chip_Select_Down
	uint8_t adder = 0x60;
	HAL_SPI_Transmit(&hspi5, &adder, 1, 1);
	Chip_Select_Up 
}
// проверить регистры + учесть время на запись, про которое ни слово не сказано в документации
void Page_Programm_PP(uint32_t address)
{
	Write_Enable_WREN();
	uint8_t adder = 0x02;
	uint8_t data_adder[4];
	data_adder[0] = adder;
	data_adder[1] = address >> 8;
	data_adder[2] = address >> 16;
	data_adder[3] = address >> 24;
	Chip_Select_Down
	HAL_SPI_Transmit(&hspi5, data_adder, 4, 1);
	HAL_SPI_Transmit(&hspi5, Flash_data_in, 256, 1);
	Chip_Select_Up 
}
/*
Функцию записи странички
	функцию стирания сектора
	функцию чтения
	фнукцию запроса текущего статуса*/
//инициализация
int FLASH_Init(void)
{
}
//чтение сектора
int FLASH_Read_Sector(void)
{
}
//запись сектора
int FLASH_Write_Sector(void)
{
}


/*=============================================================================================================*/
/*!  \brief
     \return 
     \retval 
     \sa 
*/
/*=============================================================================================================*/

