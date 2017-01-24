#ifndef INITIALIZATIONUNO_V1_H
#define INITIALIZATIONUNO_V1_H

#ifdef __cplusplus
extern "C" {
#endif

/*!  \file 	InitializationUNO_v1.h
 *   \brief  ���� ������������ ���� InitializationUNO_v1.h
 * �������� � ���� ��������� ������� ��������� � ������������� ����������� ����� Advantex,
 * ������� ����� ��� �������� ����� ���������� ���������!
 *
 * int uno_open(uint8_t uno_index) -> ��� ������� ��������� �����������. � ���� ������� ���������� ���������
 * ���������������� ������ SPI5 ��� SPI6, � ����� ��������� �����������, ����������� �� 1 ��� �� 13 ���
 * �������������� ���� �� ������������- ��� ���� �������� �����������.
 * �������� ���������: ERR_UNO_Pow = -1 ( ������ �� ��� ); UNO_OK = 0 ( ������� ������ );
 *
 * int uno_close( uint8_t uno_index) -> ������� ��������� ��������� ���������� � ������ SPI5 ��� SPI6
 * �������� ���������: outputState = 0 ( �������� ���������� ); outputStete = -2 ( ������ �� ���������� )
 *
 * int uno_write( uint8_t uno_index, float freq, uint8_t gain ) -> ������� ������������� ������� 
 * � �������� � ��������� ����������.
 * �������� ���������:	UNO_OK == 0 �������� ������������
 *
 *	uno_gain -> ������� ��������
 *	uno_set_profile -> ������� ��������� �������
 *	uno_read_profile -> ������� ������ �������
 *	uno_read_profile_fast -> ������� �������� ������ �������. ������������ ��� ������������ �� ������� �  
 *	����� ��������� ���������. ����������- ������������ ����. K � n_pow!
 *
 * calculate_uno -> ��������������� ���������� ���������� ��� ����������� � ������� � ������� UnoData_0 ... UnoData_3
 * transmit_uno  -> ��������� ����������� �� ���������� ��������������� ��������� UnoData_0 ... UnoData_3
 */

/*includes==========================================================================================================*/		
#include "math.h"
#include "stdint.h"
/*defines===========================================================================================================*/


/*types=============================================================================================================*/


/*prototypes========================================================================================================*/
int uno_open(
			uint8_t uno_index /*!< [in] ������ ����������� 0 ��� 1 */
			);

int uno_close(
			  uint8_t uno_index /*!< [in] ������ ����������� 0 ��� 1 */
			 );

int uno_write(
			 uint8_t uno_index, /*!< [in] ������ ����������� 0 ��� 1 */
			 float freq,        /*!< [in] ������� �� 1 ��� �� 13 ���. ��������������� � ��� */
			 uint8_t gain       /*!< [in] �������� ������� �� 0 �� 0x3F */
			  );

int uno_gain(
			uint8_t uno_index, /*!< [in] ������ ����������� 0 ��� 1 */
			uint8_t gain       /*!< [in] �������� ������� �� 0 �� 0x3F */
			);
int uno_set_profile(
					uint8_t uno_index,	    /*!< [in] ������ ����������� 0 ��� 1 */
					float freq,				/*!< [in] ������� � ��� */
					uint8_t dds_profile		/*!< [in] ����� ������� 0 - 7 */
					);
int uno_read_profile(
					uint8_t uno_index,	    /*!< [in] ������ ����������� 0 ��� 1 */
					uint8_t dds_profile,	/*!< [in] ����� ������� */
					float freq				/*!< [in] ������� ������- ��� ���������� ��������� ����. */
					);
int uno_read_profile_fast(
						uint8_t uno_index,	    /*!< [in] ������ ����������� 0 ��� 1 */
						uint8_t dds_profile /*!< [in] ����� ������� */
						 );

void transmit_uno(uint8_t uno_index, uint8_t* UnoData_0, uint8_t* UnoData_1, uint8_t* UnoData_2, uint8_t* UnoData_3);
void calculate_uno(float freq, uint8_t gain);

void usual_freq(float freq, uint8_t gain);
#ifdef __cplusplus
}
#endif

#endif /* !InitializationUNO_v1.h */


