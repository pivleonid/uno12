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
/*Функция принимает 22 байта и записывает в свободное место*/
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
/* Считывание общего количества записей*/
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

/*
	Последовательное считывание ключей с сектора
	key = 7 * 186 = 1302 записи. 1303 записей т.к. 0 байт решил оставить для количества записей в секторе
	if key[0] > 186 => записей больше нет
*/
int8_t Getdatanames_sector(uint8_t key[1303], uint16_t sector ) {
	uint8_t sector_data[SectorDataSize];
	memset(sector_data, 0, sizeof(sector_data));
	Read_sector_bytes(sector_data, sector);
	if (sector_data[0] > 186)
		return error_data;
	for (int i = 1, j = 1; i < 1303; i += 7, j += 22)
		memcpy(&key[i], &sector_data[j], 7);
	key[0] = sector_data[0];
}
/*
1) Считываю ключи с 0 сектора, хранимые в key_data[1303] с 1 элемента и проверяю их на совпадение
1.1) поиск совпадения сопровлождается инкрементом key_count -> который потребуется для поиска значения
2) ежели совпадение не найдено попадаем в цикл while. ежели 0 байт > 186 return error_data-> совпадения не найдены
3) В цикле инкрементируется сектор. При совпадении значения перепрыгиваем на метку pass
4) Зная номер сектора- вычитываею данные с него
5) зная номер ключа key_count-> копирую данные в data[15]
*/
int8_t GetData(uint8_t key[7], uint8_t data[15]) {
	uint8_t sector_data[SectorDataSize];
	static uint16_t sector = 0;
	uint8_t key_count_index = 8; // первый элемент с данными
	uint8_t key_data[1303];
	uint8_t key_count = 0; // счетчик положения данных
	Getdatanames_sector(key_data, sector);
	for (int i = 1; i < 1303; i += 7) {
		key_count++;
		if (memcmp(key, &key_data[i], 7) == 0)
			goto pass; //выход из цикла с сохранением key_count
	}
	/*Если больше 186 и значения не найдено в 0 секторе*/
	while (key_data[0] > 186) {
		key_count = 0;
		Getdatanames_sector(key_data, ++sector);
		for (int i = 1; i < 1303; i += 7) {
			key_count++;
			if (memcmp(key, &key_data[i], 7) == 0)
				goto pass;
		}
	}
	return error_data;
	//работа поиска значения  с использованием key_count'a и sector
pass:
	Read_sector_bytes(sector_data, sector);
	memcpy(data, &sector_data[key_count_index*key_count], 15);
}

/*
Считывание количества записей с определенными координатами
*/
uint32_t Getdatasizemask(uint8_t key[7], uint8_t mask_key[7]) {
	for (uint8_t i = 0; i < 7; i++)
		key[i] = key[i] & mask_key[i];
	uint8_t key_data[1303];
	static uint16_t sector = 0;
	uint8_t sector_data[SectorDataSize];
	uint32_t data_key_size = 0; 
	Getdatanames_sector(key_data, sector);
	/* видоизменяю массив ключей под "маску"*/
	for (int i = 1; i < (key_data[0]*7) + 1; ) {	/*1303 -max. */
		for (uint8_t j = 0; j < 7; j++, i++)
			key_data[i] = key_data[i] & mask_key[j];
		if (memcmp(key, &key_data[i - 7],7) == 0)
			data_key_size++;
	}


	while (key_data[0] >= 186) {
		Getdatanames_sector(key_data, ++sector);
		/* видоизменяю массив ключей под "маску"*/
		for (int i = 1; i < (key_data[0] * 7) + 1; ) {  /*1303*/
			for (uint8_t j = 0; j < 7; j++, i++)
				key_data[i] = key_data[i] & mask_key[j];
			if (memcmp(key, &key_data[i - 7], 7) == 0)
				data_key_size++;
		}
	}
	return data_key_size;
}

/*
Считывание количества записей с определенными координатами
*/  //Идея -выводить результат как массив структур
void getdatanamesmask_sector(uint8_t data_mask[2790], uint8_t key[7], uint8_t mask_key[7], uint16_t sector) {
	for (uint8_t i = 0; i < 7; i++)
		key[i] = key[i] & mask_key[i];
	uint8_t key_data[1303];
	uint8_t sector_data[SectorDataSize];
	uint16_t key_count = 0, quantity_mask = 0;
	uint16_t data_count = 1;
	Getdatanames_sector(key_data, sector);
	Read_sector_bytes(sector_data, sector);
	/* видоизменяю массив ключей под "маску"*/
	for (int i = 1; i < (key_data[0] * 7) + 1; ) {	/*1303 -max. */
		for (uint8_t j = 0; j < 7; j++, i++)
			key_data[i] = key_data[i] & mask_key[j]; 
		/*надо сравнить ключ заполненного массива*/ 
		if (memcmp(key, &key_data[i - 7], 7) == 0){
			/*Ежели ключ совпал, надо записакть данные, считанные из сектора*/
			memcpy(&data_mask[data_count], &sector_data[22*key_count+8], 15);
			data_count += 15;
			quantity_mask++;
		}
		/*Номер ключа в секторе*/
		key_count++;
	}
	data_mask[0] = quantity_mask;
}














