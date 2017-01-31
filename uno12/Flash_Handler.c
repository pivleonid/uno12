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
/*types=============================================================================================================*/
/*prototypes========================================================================================================*/
int16_t save_page_data(uint8_t *data_in);

static void Read_sector_bytes(uint8_t* sector_data, uint16_t sector);
static uint8_t Sector_is_Full(uint8_t* sector_data);
static void Sector_write(uint8_t* sector_data, uint8_t* data_in, uint16_t sector);
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

int16_t save_page_data(uint8_t *data_in);

void setdata(uint8_t* data)
{
	static uint16_t sector = 0;
	uint8_t sector_data[SectorDataSize];
	memset(sector_data, 0, sizeof(sector_data));
	Read_sector_bytes(sector_data, sector);
	while (Sector_is_Full(sector_data) == sector_full)
			Read_sector_bytes(sector_data, ++sector);
	/*Очистка сектора*/
	Sector_Erase_SE4B((sector * SectorDataSize));
	Sector_write(sector_data, data, sector);
	/*Проверка*/
	Read_sector_bytes(sector_data, sector);
	
}
/*Считываем данные с сектора*/
static void Read_sector_bytes(uint8_t* sector_data, uint16_t sector) 
{
	int i;
	uint16_t sector_data_inc = 0;	/*Для заполнения массива sector data*/
	uint32_t sector_adder_locate = sector * SectorDataSize;
	Read_DAta_Bytes_READ4B(sector_adder_locate, sector_data, 4096); 
}
/*Ежели сектор пуст sector_data[0] = 0xFF заменяю на нуль;
  ежели значение больше, чем MaXSectorCommand return(sector_full) = 0xF0*/
static uint8_t Sector_is_Full(uint8_t* sector_data)
{
	/*По умолчанию sector = 0xFF*/
	if (sector_data[0] == 0xFF)
		sector_data[0] = 0;
	if (sector_data[0] > MaXSectorCommand)
		return sector_full;
	return sector_data[0];
}
/*===---Посекторная запись---===*/
static void Sector_write(uint8_t* sector_data, uint8_t* data_in, uint16_t sector)
{
	int i;
	uint16_t sector_data_inc = 0;	/*Для заполнения массива sector data*/
	uint32_t sector_adder_locate = sector * SectorDataSize;
	/*		количество записей хранится в нулевом байте массива, домножая на 22 получаю индекс конца записи, +1 чтобы не затереть 0 массив		*/
	uint8_t quantity_data_sector = (sector_data[0] * 22)+1;
	/*		Заполнение данных		*/
	memcpy(&sector_data[quantity_data_sector], data_in, 22);
	sector_data[0] = sector_data[0]+1;
	/*		Запись данных во флэш	*/
	sector_adder_locate = sector * SectorDataSize;
	for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
	{
		FLASH_Page_Programm_PP(sector_adder_locate, &sector_data[sector_data_inc]);
	}
}




///////////////////////////////////////////////////////////////////
/*Поиск индекса с подготовкой для новой записи 0х44 0х44 */
static uint8_t* New_Search_data(uint8_t* sector_data)
{
	// uint8_t* null_p = NULL;
	for (int i = 0; i < SectorDataSize; i++) {
		if (sector_data[i] == 0x44)
			if (sector_data[i + 1] == 0x44)
				if (sector_data[i + 2] == 0xFF) {
					sector_data[i + 22] = 0x44;
					sector_data[i + 23] = 0x44;
					return &sector_data[i];
				}
	}
	return NULL;
}
/*Поиск индекса с заполнением окончания записи 0х44 0х44 */
static uint8_t* Search_data(uint8_t* sector_data)
{
	// uint8_t* null_p = NULL;
	for (int i = 0; i < SectorDataSize; i++) {
		if (sector_data[i] == 0x44)
			if (sector_data[i + 1] == 0x44)
				return &sector_data[i];
	}
	return NULL;
}
//Возвращает номер сектора
static int8_t Sector_num(uint8_t* sector_data, int16_t sector)
{
	for (int i = 0; i < SectorDataSize; i++) {
		if (sector_data[i] == 0x44)
			if (sector_data[i + 1] == 0x44)
				/* Влезут ли новые данные? */
					if (((i + 23) * sizeof(&sector_data[0])) > sizeof(sector_data)){
						return ++sector;
					return sector;
				    }
	}
	return -5;
}
int16_t save_page_data(uint8_t *data_in)
{									//adder max = 0x3FFFF00
	static uint16_t sector = 0;	//max       = 0x3FFF
	int i;
	uint16_t sector_data_inc; /*Для заполнения массива sector data*/
	uint8_t sector_data[SectorDataSize];
	uint32_t sector_adder_locate = sector * SectorDataSize;
	/*Флэшка заполнена?*/
	if (sector > MaxSectorSize) {
		sector = 0;
		return (presel_container_full);
	}
	/*Считываем данные с сектора*/
	for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
			Read_DAta_Bytes_READ4B(sector_adder_locate, &sector_data[sector_data_inc]);
	/*Уместятся ли новые данные в секторе?*/
	if ( sector == Sector_num(sector_data, sector))
	/*Очистка сектора*/
	Sector_Erase_SE4B((sector * SectorDataSize));
	else{
		uint8_t sector_adder_locate = sector * SectorDataSize;
		/*сохранение нового значения сектора*/
		sector = Sector_num(sector_data, sector);
		/*Считываем данные с сектора*/
		for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
			Read_DAta_Bytes_READ4B(sector_adder_locate, &sector_data[sector_data_inc]);
		/*Очистка сектора*/
		Sector_Erase_SE4B((sector * SectorDataSize));
	}
	/*Проверка данных*/
	if (New_Search_data(sector_data) == NULL)
		return (error_data);
	/*Заполнение данных*/
	memcpy(New_Search_data(sector_data), data_in, 22);
	/*Запись данных во флэш*/
	sector_adder_locate = sector * SectorDataSize;
	sector_data_inc = 0;
	for (i = 0; i < 16; sector_adder_locate += 256, sector_data_inc += 256)
		FLASH_Page_Programm_PP(sector_adder_locate, &sector_data[sector_data_inc]); //запись во флэш
}

/*====Считывание общего количества данных====*/
uint8_t GetDataSize(void) 
{
	static uint16_t sector = 0;	//max       = 0x3FFF
	int i;						//счетчик заполнения сектора
	uint16_t sector_data_inc;	/*Для заполнения массива sector data*/
	uint8_t sector_data[SectorDataSize];
	uint8_t 	num_data; //Количество записей в секторе
	uint32_t sector_adder_locate = sector * SectorDataSize;
	/*Считываем данные с сектора*/
	for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
		Read_DAta_Bytes_READ4B(sector_adder_locate, &sector_data[sector_data_inc]);
	if (Search_data(sector_data) == NULL)
		return 0; //Записей нет
	 num_data = (sizeof(Search_data(sector_data))) / 22;
	//

	while (Search_data(sector_data) != NULL)
	{
		sector++;
		sector_adder_locate = sector * SectorDataSize;
		for (i = 0; i < 16; i++, sector_adder_locate += 256, sector_data_inc += 256)
			Read_DAta_Bytes_READ4B(sector_adder_locate, &sector_data[sector_data_inc]);
		num_data = num_data + (sizeof(Search_data(sector_data))) / 22;

	}
	return num_data;
}


//#pragma pack(push, 1)
//typedef struct  _PRES_KEY
//{
//	uint8_t     freq[5];
//	int8_t      temp;
//	uint8_t		LNA;
//}_PRES_KEY_t;
//#pragma pack(pop)
//
//#pragma pack(push ,1)
//typedef struct  _PRES_BODY
//{
//	uint8_t     inter_step[2];
//	uint8_t     dac01[3];
//	uint8_t     dac23[3];
//	uint8_t     dac45[3];
//	uint8_t     dac67[3];
//	uint8_t     digital_att;
//}_PRES_BODY_t;
//#pragma pack(pop)
//
//#pragma pack(push ,1)
//typedef struct  _PRES_PACK
//{
//	_PRES_BODY_t	key;
//	_PRES_BODY_t	body;
//}preselector_pack_t;
//
//preselector_pack_t pact_t[11]; //для последовательного заполнения
//
////Заполнение сначала 11 байт ключей, а потом 11 байт "тела" 
//#pragma pack(push ,1)
//typedef struct _Flash_256 {
//	_PRES_KEY_t		 key_t[11];		//7 байт
//	_PRES_BODY_t	 body_t[11];	//15 байт
//}_Flash_256_t;
//_Flash_256_t Flash_256;
//#pragma pack(pop)
///*=============================================================================================================*/
///*!  \brief
//*		Последовательное Заполнение контейнера данных и отправка.
//*		Одна посылка 22 байта.
//*		Страница 256 байт.
//*		22 байта * 11 команд = 242 байт
//*
//\return int16_t
//\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
//\sa
//*/
///*=============================================================================================================*/   
//
//int16_t save_page_data(uint8_t *data_in)
//{
//	/*статическая переменная !сохраняет! свое значение между вызовами,
//	а инициализация происходит только один раз */
//	static uint8_t i = 0;
//	if (i > 10) 
//	{
//		static uint32_t adder_locate;//max = 0x3FFFF00
//		static int16_t sector;		  //max = 0x3FFF
//		i = 0;
//		if (adder_locate % 4096 == 0)
//			sector++;
//		if (sector > 16383)
//		{
//			sector = 0;
//			return (presel_container_full);
//		}
//		FLASH_Page_Programm_PP(adder_locate, &Flash_256); //запись во флэш
//		adder_locate += 256;
//		return (sector);
//	}
//	else
//	{
//		memcpy(&pact_t[i], data_in, 22 );
//		i++;
//		return (presel_container_not_full);
//	}
//}
///*=============================================================================================================*/
///*!  \brief
//*		Заполнение контейнера данных и отправка.
//*		Первично заполняются ключи - 77 байт, затем тело - 165
//*		Страница 256 байт.
//*		22 байта * 11 команд = 242 байт
//*		
//*
//\return int16_t
//\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
//\sa
//*/
///*=============================================================================================================*/
//int16_t save_page_data_keyBody(uint8_t *data_in)
//{
//	static uint8_t i = 0;
//	if (i > 10)
//	{
//		static uint32_t adder_locate; //max = 0x3FFFF00
//		static int16_t sector;		  //max = 0x3FFF
//		i = 0;
//		if (adder_locate % 4096 == 0)
//			sector++;
//		if (sector > 16383)
//		{
//			sector = 0;
//			return (presel_container_full);
//		}
//		uint8_t a[256];
//		memcpy(a, &Flash_256, 256); //а - для проверки заполнения массива
//		FLASH_Page_Programm_PP(adder_locate, a); //запись во флэш
//		adder_locate += 256;
//		return (sector);
//	}
//
//	else
//	{		
//		memcpy(&Flash_256.key_t[i] , data_in, 7);
//		memcpy(&Flash_256.body_t[i], &data_in[7], 15);
//		i++;
//		return (presel_container_not_full);
//	}
//}
///*=============================================================================================================*/
///*!  \brief
//*		Заполнение контейнера данных и отправка.
//*		Первично заполняются ключи - 77 байт, затем тело - 165
//*		Страница 256 байт.
//*		22 байта * 11 команд = 242 байт
//*
//*
//\return int16_t
//\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
//\sa
//*/
///*=============================================================================================================*/

    










