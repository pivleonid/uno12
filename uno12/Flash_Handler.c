/*!  \file 	Flash_Handler.c
*   \brief   Функции для работы с Flash памятью 
*   \details
*
*/
/*includes==========================================================================================================*/
#include "stdint.h"
/*defines===========================================================================================================*/
#define presel_container_not_full	 -1
#define presel_container_full	     -2
/*types=============================================================================================================*/
/*prototypes========================================================================================================*/
int16_t save_page_data(uint8_t *data_in);
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

preselector_pack_t pact_t[11]; //для последовательного заполнения

//Заполнение сначала 11 байт ключей, а потом 11 байт "тела" 
#pragma pack(push ,1)
typedef struct _Flash_256 {
	_PRES_KEY_t		 key_t[11];		//7 байт
	_PRES_BODY_t	 body_t[11];	//15 байт
}_Flash_256_t;
_Flash_256_t Flash_256;
#pragma pack(pop)
/*=============================================================================================================*/
/*!  \brief
*		Последовательное Заполнение контейнера данных и отправка.
*		Одна посылка 22 байта.
*		Страница 256 байт.
*		22 байта * 11 команд = 242 байт
*
\return int16_t
\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
\sa
*/
/*=============================================================================================================*/   

int16_t save_page_data(uint8_t *data_in)
{
	/*статическая переменная !сохраняет! свое значение между вызовами,
	а инициализация происходит только один раз */
	static uint8_t i = 0;
	if (i > 10) 
	{
		static uint32_t adder_locate;//max = 0x3FFFF00
		static int16_t sector;		  //max = 0x3FFF
		i = 0;
		if (adder_locate % 4096 == 0)
			sector++;
		if (sector > 16383)
		{
			sector = 0;
			return (presel_container_full);
		}
		FLASH_Page_Programm_PP(adder_locate, &Flash_256); //запись во флэш
		adder_locate += 256;
		return (sector);
	}
	else
	{
		memcpy(&pact_t[i], data_in, 22 );
		i++;
		return (presel_container_not_full);
	}
}
/*=============================================================================================================*/
/*!  \brief
*		Заполнение контейнера данных и отправка.
*		Первично заполняются ключи - 77 байт, затем тело - 165
*		Страница 256 байт.
*		22 байта * 11 команд = 242 байт
*		
*
\return int16_t
\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
\sa
*/
/*=============================================================================================================*/
int16_t save_page_data_keyBody(uint8_t *data_in)
{
	static uint8_t i = 0;
	if (i > 10)
	{
		static uint32_t adder_locate; //max = 0x3FFFF00
		static int16_t sector;		  //max = 0x3FFF
		i = 0;
		if (adder_locate % 4096 == 0)
			sector++;
		if (sector > 16383)
		{
			sector = 0;
			return (presel_container_full);
		}
		uint8_t a[256];
		memcpy(a, &Flash_256, 256); //а - для проверки заполнения массива
		FLASH_Page_Programm_PP(adder_locate, a); //запись во флэш
		adder_locate += 256;
		return (sector);
	}

	else
	{		
		memcpy(&Flash_256.key_t[i] , data_in, 7);
		memcpy(&Flash_256.body_t[i], &data_in[7], 15);
		i++;
		return (presel_container_not_full);
	}
}
/*=============================================================================================================*/
/*!  \brief
*		Заполнение контейнера данных и отправка.
*		Первично заполняются ключи - 77 байт, затем тело - 165
*		Страница 256 байт.
*		22 байта * 11 команд = 242 байт
*
*
\return int16_t
\retval presel_container_not_full = -1; presel_container_full = -2; int16_t sector - номер сектора
\sa
*/
/*=============================================================================================================*/

    










