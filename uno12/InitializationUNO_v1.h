#ifndef INITIALIZATIONUNO_V1_H
#define INITIALIZATIONUNO_V1_H

#ifdef __cplusplus
extern "C" {
#endif

/*!  \file 	InitializationUNO_v1.h
 *   \brief  Этот заголовочный файл InitializationUNO_v1.h
 * содержит в себе прототипы функций настройки и инициализации синтезатора фирмы Advantex,
 * которая косит под пиндосов своим буржуйским даташитом!
 *
 * int uno_open(uint8_t uno_index) -> Это функция настройки синтезатора. В этой функции происходит включение
 * соответствующего модуля SPI5 или SPI6, а также включение синтезатора, работающего от 1 Ггц до 13 ГГц
 * низкочастотный вход не задействован- его надо отдельно настраивать.
 * Выходные параметры: ERR_UNO_Pow = -1 ( Модуль не вкл ); UNO_OK = 0 ( Питание подано );
 *
 * int uno_close( uint8_t uno_index) -> Функция выключает выбранный синтезатор и модуль SPI5 или SPI6
 * Выходные параметры: outputState = 0 ( успешное выключение ); outputStete = -2 ( модуль не выключился )
 *
 * int uno_write( uint8_t uno_index, float freq, uint8_t gain ) -> функция устанавливает частоту 
 * и усиление в выбранный синтезатор.
 * Выходные параметры:	UNO_OK == 0 успешное переключение
 *
 *	uno_gain -> функция усиления
 *	uno_set_profile -> функция установки профиля
 *	uno_read_profile -> функция чтения профиля
 *	uno_read_profile_fast -> функция быстрого чтения профиля. Используется для свипирования по частоте в  
 *	малом частотном диапазоне. Требования- неизменность коэф. K и n_pow!
 *
 * calculate_uno -> Предварительное вычисления параметров для синтезатора с записью в массивы UnoData_0 ... UnoData_3
 * transmit_uno  -> Настройка синтезатора по параметрам предварительной настройки UnoData_0 ... UnoData_3
 */

/*includes==========================================================================================================*/		
#include "math.h"
#include "stdint.h"
/*defines===========================================================================================================*/


/*types=============================================================================================================*/


/*prototypes========================================================================================================*/
int uno_open(
			uint8_t uno_index /*!< [in] индекс синтезатора 0 или 1 */
			);

int uno_close(
			  uint8_t uno_index /*!< [in] индекс синтезатора 0 или 1 */
			 );

int uno_write(
			 uint8_t uno_index, /*!< [in] индекс синтезатора 0 или 1 */
			 float freq,        /*!< [in] частота от 1 ГГц до 13 ГГц. Устанавливается в МГц */
			 uint8_t gain       /*!< [in] усиление сигнала от 0 до 0x3F */
			  );

int uno_gain(
			uint8_t uno_index, /*!< [in] индекс синтезатора 0 или 1 */
			uint8_t gain       /*!< [in] усиление сигнала от 0 до 0x3F */
			);
int uno_set_profile(
					uint8_t uno_index,	    /*!< [in] индекс синтезатора 0 или 1 */
					float freq,				/*!< [in] Частота в МГц */
					uint8_t dds_profile		/*!< [in] номер профиля 0 - 7 */
					);
int uno_read_profile(
					uint8_t uno_index,	    /*!< [in] индекс синтезатора 0 или 1 */
					uint8_t dds_profile,	/*!< [in] номер профиля */
					float freq				/*!< [in] частота работы- для корректных установок коэф. */
					);
int uno_read_profile_fast(
						uint8_t uno_index,	    /*!< [in] индекс синтезатора 0 или 1 */
						uint8_t dds_profile /*!< [in] номер профиля */
						 );

void transmit_uno(uint8_t uno_index, uint8_t* UnoData_0, uint8_t* UnoData_1, uint8_t* UnoData_2, uint8_t* UnoData_3);
void calculate_uno(float freq, uint8_t gain);

void usual_freq(float freq, uint8_t gain);
#ifdef __cplusplus
}
#endif

#endif /* !InitializationUNO_v1.h */


