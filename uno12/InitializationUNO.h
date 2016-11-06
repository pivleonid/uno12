#ifndef INITIALIZATIONUNO_H
#define INITIALIZATIONUNO_H		
#include "math.h"
#include "stdint.h"
void SPI5_Init_UNO(void);
void SPI6_Init_UNO(void);
void uno_open(uint8_t);
void uno_close(uint8_t);
void uno_write(uint8_t , float , uint16_t );
uint8_t Funk_n_pow(float );
float Funk_fr_vco2(uint8_t);
int Search_K(float );
#endif // !InitializationUNO.h