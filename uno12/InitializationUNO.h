#ifndef INITIALIZATIONUNO_H
#define INITIALIZATIONUNO_H		
#include "math.h"
#include "stdint.h"
//#ifdef __cplusplus
//extern "C" {
//#endif

int uno_open(uint8_t uno_index);
/*
ERR_UNO_Pow == -1 Модуль не вкл.;
UNO_OK == 0 Питание подано;
*/
int uno_close(uint8_t uno_index);
/* outputState == 0 успешное выключение
   outputStete == -2 модуль не выключился */
int uno_write(uint8_t uno_index, float freq, uint8_t gain);
//	UNO_OK == 0 успешное переключение

//статические функции определены в InitializationUNO.c
//они не должны быть тут видны
//#ifdef __cplusplus
//}
//#endif
#endif // !InitializationUNO.h
//

