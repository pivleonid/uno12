/*!  \file 	Flash_Handler.c
*   \brief   Функции для работы с Flash памятью 
*   \details
*
*/
//В памяти должна ханиться переменная- адрессный счетчик-> секторный счетчик
/*includes==========================================================================================================*/
#include "stdint.h"
/*defines===========================================================================================================*/
#define presel_container_not_full	 -1
#define presel_container_full	     -2
#define error_data					 -3	
#define sector_full					 0xF0
#define MaxSectorSize				 16383
#define SectorDataSize				 4096
/* 22 * 186 = 4092; max = 4095 => 1 байт = quantity_data_sector; посылка окончания 0x44, 0x44 уместится*/
#define MaXSectorCommand 186
#ifndef NULL
#define NULL  0
#endif


#include "stm32f4xx_hal.h"
/*types=============================================================================================================*/
/*prototypes========================================================================================================*/

uint32_t Getdatasize(void);
void setdata(uint8_t* data);

static void Read_sector_bytes(uint8_t* sector_data, uint16_t sector);
static uint8_t Sector_is_Full(uint8_t* sector_data);
static void Sector_write(uint8_t* sector_data, uint8_t* data_in, uint16_t sector);
 int error = 0;
/*variables=========================================================================================================*/
#pragma pack(push, 1)
typedef struct  _PRES_KEY
{
	uint8_t     freq[5];
	int8_t      temp;
	uint8_t		LNA;
}_PRES_KEY_t;
#pragma pack(pop)

#pragma pack(push ,1)
typedef struct  _PRES_BODY
{
	uint8_t     inter_step[2];
	uint8_t     dac01[3];
	uint8_t     dac23[3];
	uint8_t     dac45[3];
	uint8_t     dac67[3];
	uint8_t     digital_att;
}_PRES_BODY_t;
#pragma pack(pop)

#pragma pack(push ,1)
typedef struct  _PRES_PACK
{
	_PRES_BODY_t	key;
	_PRES_BODY_t	body;
}preselector_pack_t;

void setdata(uint8_t* data)
{
	static uint16_t sector = 0;
	uint8_t sector_data[SectorDataSize];
	memset(sector_data, 0, sizeof(sector_data));
	Read_sector_bytes(sector_data, sector);
	while (Sector_is_Full(sector_data) == sector_full) {
		Read_sector_bytes(sector_data, ++sector);
	}
				
	/*Очистка сектора*/
	Sector_Erase_SE4B(sector);
	Sector_write(sector_data, data, sector);
	/////////////
}
/*Считываем данные с сектора*/
static void Read_sector_bytes(uint8_t* sector_data, uint16_t sector) 
{
	uint32_t sector_adder_locate = sector * SectorDataSize;
	memset(sector_data, 0, sizeof(sector_data));
	Read_DAta_Bytes_READ4B(sector_adder_locate, sector_data, 4096); 
}
/*Ежели сектор пуст sector_data[0] = 0xFF заменяю на нуль;
  ежели значение больше, чем MaXSectorCommand return(sector_full) = 0xF0*/
static uint8_t Sector_is_Full(uint8_t* sector_data)
{
	/*По умолчанию sector = 0xFF*/
	if (sector_data[0] == 0xFF)
		sector_data[0] = 0;
	if (sector_data[0] > MaXSectorCommand - 1)
		return sector_full;
		
	return sector_data[0];
}
/*===---Посекторная запись---===*/
static void Sector_write(uint8_t* sector_data, uint8_t* data_in, uint16_t sector)
{
	uint8_t inspection_sector_data[SectorDataSize];
	int i;
	uint16_t sector_data_inc = 0;	/*Для заполнения массива sector data*/
	uint32_t sector_adder_locate = sector * SectorDataSize;
	/*		количество записей хранится в нулевом байте массива, домножая на 22 получаю индекс конца записи, +1 чтобы не затереть 0 массив		*/
	uint32_t quantity_data_sector = (sector_data[0] * 22)+1;
	/*		Заполнение данных		*/
	memcpy(&sector_data[quantity_data_sector], data_in, 22);
	/*Счетчик данных*/
	sector_data[0] = sector_data[0]+1;
	/*		Копирование данных		*/
	memcpy(inspection_sector_data,sector_data, SectorDataSize);
	/*		Вычисление адреса сектора	*/
	sector_adder_locate = sector * SectorDataSize;
	/*		Запись данных во флэш		*/
	for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
	{
		FLASH_Page_Programm_PP(sector_adder_locate, &inspection_sector_data[sector_data_inc]);
	}
	uint8_t inspection_sector_data_test[SectorDataSize];
	Read_sector_bytes(inspection_sector_data_test, sector);
	uint16_t count = (sector_data[0] * 22) + 1;
			/*Обработчик ошибок*/
			while (memcmp(inspection_sector_data, inspection_sector_data_test, count) != 0) {
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
			sector_adder_locate = sector * SectorDataSize;
			sector_data_inc = 0;
			Sector_Erase_SE4B(sector);
			Read_sector_bytes(inspection_sector_data_test, sector);
			for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
				FLASH_Page_Programm_PP(sector_adder_locate, &inspection_sector_data[sector_data_inc]);
			Read_sector_bytes(inspection_sector_data_test, sector);
			error++;// отследим сколько раз 
			
			if (error > 50)
				error--;
		}

}

uint32_t Getdatasize(void) {
	static uint16_t sector = 0;
	uint8_t sector_data[SectorDataSize];
	uint32_t data_size;
	memset(sector_data, 0, sizeof(sector_data));
	Read_sector_bytes(sector_data, sector);
	data_size = sector_data[0];
	while (Sector_is_Full(sector_data) == sector_full) {
		Read_sector_bytes(sector_data, ++sector);
		data_size += sector_data[0];
	}
	data_size -= 255;
	return data_size;
}

















