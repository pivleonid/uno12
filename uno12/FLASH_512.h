#ifndef FLASH_512_H
#define FLASH_512_H		

#ifdef __cplusplus
extern "C" {
#endif


/*!  \file 	FLASH_512.h
 *   \brief Это заголовочный файл FLASH_512.h микросхемы mx25l51245g_3v_512mb
 *  Файл содержит в себе прототипы функций инициализации, настройки соответствующего
 *  модуля SPI, Чтение ID идентификаторов, статусных регистров, операции чтения/записи
 *  и очистки памяти.
 *
 *  void FLASH_SPI_open(void);void FLASH_SPI_close(void); -> настройка и деинициализация 
 *  SPI модуля соответственно
 *
 *  void FLASH_RDID(void); void FLASH_RES(void); void FLASH_REMS(void); -> эти функции
 *  используются для проверки ID флэшки.
 *
 *  uint8_t FLASH_Read_Status_Register_RDSR(void);
 *
 *  uint8_t FLASH_Read_Configuration_Register_RDCR(void);
 *
 *  FLASH_Write_Status_Configuration_Register_WRSR
 */
/*includes==========================================================================================================*/
#include "math.h"
#include "stdint.h"


/*defines===========================================================================================================*/


/*types=============================================================================================================*/
/*
Организацию памяти смотри в 4 таблице документации MX25L51245G
file:///D:/Leo_work/myExample/Moduls/uno/not_cube_sintezators/uno12/uno12/pdf/mx25l51245g_3v_512mb_v0.01.pdf
*/

/*prototypes========================================================================================================*/
void FLASH_SPI_open(void);
void FLASH_SPI_close(void);

/*----Проверка ID FLASH----------*/

/* Manufactory ID -> С2; Memory type -> 20; Memory density -> 1A */
void FLASH_RDID(void);
/*  Electronic ID -> 19     */
void FLASH_RES(void);
/* Manufactory ID -> С2; Device ID -> 19     */
void FLASH_REMS(void);

//----------Статусные регистры-----------//

//	Чтение статусного регистра
/* Возвращаемое значение
bit7 -> Write Disable -> 1 = Запись запрещена
bit6 -> 1 = Quad Enable; 0 = not Quad Enable
bit5 -> BP3 block protect
bit4 -> BP2 block protect
bit3 -> BP1 block protect
bit2 -> BP0 block protect
bit1 -> WEL(write enable latch)	   -> 1 = Запись разрешена; 0 = Запись запрещена
bit0 -> WIP(write in progress bit) -> 1 = Операция Записи;  0 = Нет Записи
*/
uint8_t FLASH_Read_Status_Register_RDSR(void);
//	Чтение конфигурационного регистра - настройка не нужна -> слишком специфические параметры
uint8_t FLASH_Read_Configuration_Register_RDCR(void);
//  Запись статусного регистра и конфигурационного регистра - лучше не использовать
void FLASH_Write_Status_Configuration_Register_WRSR(
													uint8_t status_reg, /*!< [in] статусный регист */
													uint8_t config_reg  /*!< [in] конфигурационный регистр */
												   );
 
/*-----------------------Чтение size байт---------------------*/
void Read_DAta_Bytes_READ4B(
	    					uint32_t address,         /*!< [in] адрес в памяти */
							uint8_t* Flash_data_out, /*!< [in] Flash_data_out[n] */
							uint16_t size
							);
/*--- очистка сектора = 16 страниц время операции 43 - 200 мс----------*/
void Sector_Erase_SE4B(
						uint32_t SectorAddr /*Номер сектора*/
						);
/*Запись в память FLASH*/
void FLASH_Page_Programm_PP(
							uint32_t address,			/*!< [in] Запись по сектору */
							uint8_t flash_data_in[256]  /*!< [in] 256 байт данных */
							);
/*--- очистка сектора = 16 страниц время операции 43 - 200 мс----------*/
void Chip_Erase_CE(void);






#ifdef __cplusplus
}
#endif

#endif /* !FLASH_512.h  */


