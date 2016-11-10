#ifndef INITIALIZATIONUNO_H
#define INITIALIZATIONUNO_H		
#include "math.h"
#include "stdint.h"
//#ifdef __cplusplus
//extern "C" {
//#endif

int uno_open(uint8_t uno_index);
/*
ERR_UNO_Pow == -1 ������ �� ���.;
UNO_OK == 0 ������� ������;
*/
int uno_close(uint8_t uno_index);
/* outputState == 0 �������� ����������
   outputStete == -2 ������ �� ���������� */
int uno_write(uint8_t uno_index, float freq, uint8_t gain);
//	UNO_OK == 0 �������� ������������

//����������� ������� ���������� � InitializationUNO.c
//��� �� ������ ���� ��� �����
//#ifdef __cplusplus
//}
//#endif
#endif // !InitializationUNO.h
//

