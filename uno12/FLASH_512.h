#ifndef FLASH_512_H
#define FLASH_512_H		

#ifdef __cplusplus
extern "C" {
#endif


/*!  \file 	FLASH_512.h
 *   \brief ��� ������������ ���� FLASH_512.h ���������� mx25l51245g_3v_512mb
 *  ���� �������� � ���� ��������� ������� �������������, ��������� ����������������
 *  ������ SPI, ������ ID ���������������, ��������� ���������, �������� ������/������
 *  � ������� ������.
 *
 * void FLASH_SPI_open(void);void FLASH_SPI_close(void); -> ��������� � ��������������� 
 * SPI ������ ��������������
 *
 * void FLASH_RDID(void); void FLASH_RES(void); void FLASH_REMS(void); -> ��� �������
 * ������������ ��� �������� ID ������.
 *
 * uint8_t FLASH_Read_Status_Register_RDSR(void);
 *
 * uint8_t FLASH_Read_Configuration_Register_RDCR(void);
 *
 * FLASH_Write_Status_Configuration_Register_WRSR
 */
/*includes==========================================================================================================*/
#include "math.h"
#include "stdint.h"


/*defines===========================================================================================================*/


/*types=============================================================================================================*/
/*
����������� ������ ������ � 4 ������� ������������ MX25L51245G
*/

uint8_t Flash_data_in[256];
uint8_t Flash_data_out[1024]; //������

/*prototypes========================================================================================================*/
void FLASH_SPI_open(void);
void FLASH_SPI_close(void);

//----�������� ID FLASH----------//

/* Manufactory ID -> �2; Memory type -> 20; Memory density -> 1A */
void FLASH_RDID(void);
/*  Electronic ID -> 19     */
void FLASH_RES(void);
/* Manufactory ID -> �2; Device ID -> 19     */
void FLASH_REMS(void);

//----------��������� ��������-----------//

//	������ ���������� ��������
/* ������������ ��������
bit7 -> Write Disable -> 1 = ������ ���������
bit6 -> 1 = Quad Enable; 0 = not Quad Enable
bit5 -> BP3 block protect
bit4 -> BP2 block protect
bit3 -> BP1 block protect
bit2 -> BP0 block protect
bit1 -> WEL(write enable latch)	   -> 1 = ������ ���������; 0 = ������ ���������
bit0 -> WIP(write in progress bit) -> 1 = �������� ������;  0 = ��� ������
*/
uint8_t FLASH_Read_Status_Register_RDSR(void);
//	������ ����������������� �������� - ��������� �� ����� -> ������� ������������� ���������
uint8_t FLASH_Read_Configuration_Register_RDCR(void);
//  ������ ���������� �������� � ����������������� �������� - ����� �� ������������
void FLASH_Write_Status_Configuration_Register_WRSR(
													uint8_t status_reg, /*!< [in] ��������� ������ */
													uint8_t config_reg  /*!< [in] ���������������� ������� */
												   );

//-------�������� ������/������/�������----------//
// ������ ������� 1024 ����
void FLASH_Read_DAta_Bytes_READ(uint32_t address);
// ������� ������� // ����� �������� 43 - 200 ��
void FLASH_Sector_Erase_SE(uint8_t address_MSB, uint8_t address, uint8_t address_LSB);
// ������ �������� -> �������� ������ 
void FLASH_Page_Programm_PP(uint32_t address);








#ifdef __cplusplus
}
#endif

#endif /* !FLASH_512.h  */


