#ifndef FLASH_HANDLER.h
#define FLASH_HANDLER.h

#ifdef __cplusplus
extern "C" {
#endif

/*!  \file 	Flash_Handler.h
 *   \brief  
 * 
 * ������: [ ���������� ������(186 ����) ][����][�������][����][������]..[������]
 * ���� = 7 ����; ������ = 15 ����
 */

/*includes==========================================================================================================*/
#include "FLASH_512.h"
/*defines===========================================================================================================*/


/*types=============================================================================================================*/


/*prototypes========================================================================================================*/
/*������ ������� 12 ���*/
	void setdata(uint8_t* data);
	uint32_t Getdatasize(void);
	int8_t Getdatanames_sector(uint8_t key[1303], uint16_t sector);
	int8_t GetData(uint8_t key[7], uint8_t data[15]);
	uint32_t Getdatasizemask(uint8_t key[7], uint8_t mask_key[7]);
	void getdatanamesmask_sector(uint8_t data_mask[2790], uint8_t key[7], uint8_t mask_key[7], uint16_t sector);

#ifdef __cplusplus
}
#endif

#endif /* !Flash_Handler.h */