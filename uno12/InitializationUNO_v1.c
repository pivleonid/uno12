/*!  \file 	InitializationUNO_v1.c
 *   \brief   Файл для работы с синтезатором UNO-10M
 *   \details  
 *             
 */


/*includes==========================================================================================================*/
#include "InitializationUNO_v1.h"
#include "stm32f4xx_hal.h"
#include "stm32f427xx.h"

/*defines===========================================================================================================*/

#define Chip_Select_Up HAL_GPIO_WritePin(GPIO_X, GPIO_PIN, GPIO_PIN_SET);
#define Chip_Select_Down HAL_GPIO_WritePin(GPIO_X, GPIO_PIN, GPIO_PIN_RESET);
/* Дефайны ошибок */
#define UNO_OK (0)
#define ERR_UNO_Pow (-1)
#define ERR_UNO_PWR_Down (-2)
//
#define ERR_DATA_NULL  (-3)
#define ERR_SPI_BUSY   (-4)
#define ERR_SPI_TIMEOUT (-5)
#define OK (0)

/*types=============================================================================================================*/
SPI_HandleTypeDef hspi5; /* первый синтезатор   */
SPI_HandleTypeDef hspi6; /* второй синтезатор   */
SPI_HandleTypeDef hspix;
GPIO_InitTypeDef GPIO_InitStruct; 
GPIO_TypeDef* GPIO_X;
uint16_t GPIO_PIN;

/*prototypes========================================================================================================*/
static void UNOindex(uint8_t uno_index);
static int SPI_UNO_Transmit(uint8_t* data, uint16_t size);
static void SPI_5_open(void);
static void SPI_6_open(void);
static void Initial_Chip_Select_SPI_5(void);
static void Initial_Chip_Select_SPI_6(void);

static uint8_t func_n_pow(float fr_out);
static uint8_t Search_K(float fr_vco2);

/*Заполнение массивов для передачи настройки синтезаторов*/
static void FillingUnoData_0(uint32_t ftw);
static void FillingUnoData_1(uint8_t n_pow);
static void FillingUnoData_2(uint8_t gain);
static void FillingUnoData_3(uint8_t K); 

/*variables=========================================================================================================*/
/*----переменная выходного состояния--*/
int outputState; 
/*----Обработка ошибок--*/
uint8_t uno_answer;
static const uint8_t Read_reg_POW = 0x81;
/*константные массивы настройки синтезатора - для записей значений в постоянную память*/
static const uint8_t initial_mass_0[2] = {
	/*1*/ 0x01, 0x01
	};
//2 // HAL_Delay( 100 );
static	const uint8_t initial_mass_1[26] = {
	/*3*/ 0x02, 0x07,/*4*/ 0x03, 0x00 ,/*5*/ 0x04, 0x00, /*6*/ 0x60, 0x03, 0x80, 0x13,
	 /*7*/ 0x60, 0x03, 0x80, 0x12,/*8*/ 0x60, 0x00, 0x00,0x00, /*9*/0x60, 0x00, 0x00,0x01,
	/*10*/ 0x05, 0x01, /*11*/ 0x15, 0x00
	};

//12 //	HAL_Delay( 100 );

static const uint8_t initial_mass_2[4] = {
	/*13*/ 0x05, 0x03, 	/*14*/ 0x15, 0x00
	};
//15 //	HAL_Delay( 10 );

static const uint8_t initial_mass_3[4] = {
	/*16*/ 0x05, 0x01, 	/*17*/ 0x15, 0x00
};
//18 //	HAL_Delay( 10 );

static const uint8_t initial_mass_4[14] = {
	/*19*/ 0x05, 0x05, /*20-24; 25*/ 0x10, 0x00, 0x00, 0x01, 0x01, 0x02,/*26*/ 0x11, 0x00,
	/*27*/0x05, 0x00,/*28*/ 0x15, 0x00	};
//29 //	HAL_Delay( 10 );

static const uint8_t initial_mass_5[4] = {
	/*30*/ 0x05, 0x01, 	/*31*/ 0x15, 0x00
	};
//32 //	HAL_Delay( 100 );

static const uint8_t initial_mass_6[24] = {
	/*33*/ 0x10, 0x01, 0x00, 0x80, 0xB0, 0x00,/*34*/ 0x11, 0x00,/*35*/ 0x10, 0x02, 0x00, 
	0x00,0x00,0x00, /*36*/0x11, 0x00,/*37*/ 0x10, 0x03, 0x01, 0x05, 0x21, 0x20,
	/*38*/ 0x11, 0x00
	};
//39 //	HAL_Delay( 100 );

static const uint8_t initial_mass_7[83] = {
	/*40*/ 0x10, 0x03, 0x00,0x05,0x21,0x20,/*41*/ 0x11, 0x00,/*42 profile 0*/ 0x10, 0x0C,0x0F,0xFF,	0x00,  0x00,
															/*43 profile 1*/ 0x10, 0x0E, 0x0F, 0xFF, 0x00, 0x00,
															/*44 profile 2*/ 0x10, 0x10, 0x0F, 0xFF, 0x00, 0x00,
															/*45 profile 3*/ 0x10, 0x12, 0x0F, 0xFF, 0x00, 0x00, 
															/*46 profile 4*/ 0x10, 0x14, 0x0F, 0xFF, 0x00, 0x00,
															/*47 profile 5*/ 0x10, 0x16, 0x0F, 0xFF, 0x00, 0x00, 
															/*48 profile 6*/ 0x10, 0x18, 0x0F, 0xFF, 0x00, 0x00,
															/*49 profile 7*/ 0x10, 0x1A, 0x0F, 0xFF, 0x00, 0x00,
	/*50*/0x11, 0x00,/*51*/ 0x10, 0x04, 0x0B, 0xA2, 0xE8,0xBA,	/*52*/0x10,0x06, 0xC6,0x6F, 0x5E,0x22,
	/*53*/0x10, 0x05, 0x43,0x64,0xC5,0xBB,/*54*/ 0x11, 0x00,/*55*/ 0x62, 0x00,0x00,0x00,0x0B
	};	
/* выключение синтезатора*/
const uint8_t uno_off[2] = { 0x01, 0x00 };

uint8_t UnoData_0[6];
uint8_t UnoData_1[2];
uint8_t UnoData_2[2];
uint8_t UnoData_3[5];

/*code==============================================================================================================*/

/*=============================================================================================================*/
/*!  \brief
Функция настройки нужного модуля SPI
\return void
\retval
\sa
*/
/*=============================================================================================================*/
static void UNOindex(uint8_t uno_index)
{
	if (uno_index == 0)
	{
		hspix = hspi5;
		GPIO_X = GPIOF;
		GPIO_PIN = GPIO_PIN_6;
	}
	if (uno_index == 1)
	{
		hspix = hspi6;
		GPIO_X = GPIOG;
		GPIO_PIN = GPIO_PIN_15;
	}
	return UNO_OK;
}
/*=============================================================================================================*/
/*!  \brief 

     \return void
     \retval 
     \sa 
*/
/*=============================================================================================================*/
static void SPI_5_open(void)
{

	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_SPI5_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_6, RESET);
	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; //2
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi5.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi5);
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}
/*=============================================================================================================*/
/*!  \brief 

     \return void
     \retval 
     \sa 
*/
/*=============================================================================================================*/
static void SPI_6_open(void)
{
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_SPI6_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, RESET);
	hspi6.Instance = SPI6;
	hspi6.Init.Mode = SPI_MODE_MASTER;
	hspi6.Init.Direction = SPI_DIRECTION_2LINES;
	hspi6.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi6.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi6.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi6.Init.NSS = SPI_NSS_SOFT;
	hspi6.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;//2
	hspi6.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi6.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi6.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi6.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi6);  
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
/*=============================================================================================================*/
/*!  \brief 

     \return void
     \retval 
     \sa 
*/
/*=============================================================================================================*/
static void Initial_Chip_Select_SPI_5(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	Chip_Select_Up
}
/*=============================================================================================================*/
/*!  \brief 

     \return void
     \retval 
     \sa 
*/
/*=============================================================================================================*/
static void Initial_Chip_Select_SPI_6(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	Chip_Select_Up 
}
/*=============================================================================================================*/
/*!  \brief
Переписанная функция передачи по SPI. 
Входные параметры: Данные, размер.
Возвращает следующие команды:
OK = 0
ERR_DATA_NULL = -3
ERR_SPI_BUSY = -4
\return int
\retval  OK, ERR_SPI_BUSY, ERR_DATA_NULL 
\sa
*/
/*=============================================================================================================*/
static int SPI_UNO_Transmit(  uint8_t* data, uint16_t size) {
	Chip_Select_Down
	if (data == NULL) {
		return ERR_DATA_NULL;
	}
	if (hspix.State != HAL_SPI_STATE_READY) {
		return ERR_SPI_BUSY;
	}
	/* Process Locked */
	__HAL_LOCK(&hspix);
	/* Configure communication */
	hspix.State      = HAL_SPI_STATE_BUSY_TX;   
	hspix.ErrorCode  = HAL_SPI_ERROR_NONE;  
	  /*Init field not used in handle to zero */
	hspix.pRxBuffPtr  = (uint8_t *)NULL;
	hspix.RxXferSize  = 0U;
	hspix.RxXferCount = 0U;
	hspix.TxISR       = NULL;
	hspix.RxISR       = NULL;    
	/* передаем байты данных */
	hspix.pTxBuffPtr = data;
	hspix.TxXferSize = 8;
	hspix.TxXferCount = size;							/* SPI Tx Transfer Counter */
	SPI_1LINE_TX(&hspix);					/* Output enable in bidirectional mode */
	__HAL_SPI_ENABLE(&hspix);
	while (hspix.TxXferCount > 0)
	{
		if (__HAL_SPI_GET_FLAG(&hspix, SPI_FLAG_TXE))
		{
			hspix.Instance->DR = (*hspix.pTxBuffPtr++);
			hspix.TxXferCount--;
		}
	}
	 /* Wait until Busy flag is reset before disabling SPI */
	while ((hspix.Instance->SR & SPI_FLAG_BSY) != 0) {
		continue;
	}
	hspix.State = HAL_SPI_STATE_READY;
	/* Process Unlocked */
	__HAL_UNLOCK(&hspix);
	/* поднимаем соответсвующий CS */
	Chip_Select_Up
	return OK;
}


/*=============================================================================================================*/
/*!  \brief 

     \return int
     \retval  ERR_UNO_Pow = -1 синтезатор не включен; UNO_OK = 0;
	 \sa 
*/
/*=============================================================================================================*/
int uno_open(uint8_t uno_index)
{
	/*выбор синтезатора*/
	if (uno_index == 0)
	{
		SPI_5_open();
		hspix = hspi5;
		GPIO_X = GPIOF;
		GPIO_PIN = GPIO_PIN_6;
		Initial_Chip_Select_SPI_5();
	}
	if (uno_index == 1)
	{
		SPI_6_open();
		hspix = hspi6;
		GPIO_X = GPIOG;
		GPIO_PIN = GPIO_PIN_15;
		Initial_Chip_Select_SPI_6();
	}

	SPI_UNO_Transmit(initial_mass_0, 2);
    HAL_Delay(100);
	SPI_UNO_Transmit( initial_mass_1,     2 );
	SPI_UNO_Transmit( &initial_mass_1[2],  2 );
	SPI_UNO_Transmit( &initial_mass_1[4],  2 );
	SPI_UNO_Transmit( &initial_mass_1[6],  4 );
	SPI_UNO_Transmit( &initial_mass_1[10], 4 );
	SPI_UNO_Transmit( &initial_mass_1[14], 4 );
	SPI_UNO_Transmit( &initial_mass_1[18], 4 );
	SPI_UNO_Transmit( &initial_mass_1[22], 2 );
	SPI_UNO_Transmit( &initial_mass_1[24], 2 );
	HAL_Delay(100);
	SPI_UNO_Transmit( initial_mass_2,    2);
	SPI_UNO_Transmit( &initial_mass_2[2], 2);
	HAL_Delay(10);
	SPI_UNO_Transmit( initial_mass_3,    2);
	SPI_UNO_Transmit( &initial_mass_3[2], 2);
	HAL_Delay(10);
	SPI_UNO_Transmit( initial_mass_4,     2);
	
	
	/*20-24 пункт в настройках*/
	static uint8_t adder_0[2] = { 0x15, 0 };
	SPI_UNO_Transmit(adder_0, 2);
	HAL_Delay(10);
	static uint8_t adder_1[2] = { 0x05, 0x01 };
	SPI_UNO_Transmit(adder_1, 2);
	static uint8_t adder_2[2] = { 0x15, 0x00 };
	SPI_UNO_Transmit(adder_2, 2);
	HAL_Delay(10);
	
	SPI_UNO_Transmit( &initial_mass_4[2],  6);
	SPI_UNO_Transmit( &initial_mass_4[8],  2);
	SPI_UNO_Transmit( &initial_mass_4[10], 2);
	SPI_UNO_Transmit( &initial_mass_4[12], 2);
	HAL_Delay(10);
	SPI_UNO_Transmit( initial_mass_5, 2);
	SPI_UNO_Transmit( &initial_mass_5[2], 2);
	HAL_Delay(100);
	SPI_UNO_Transmit( initial_mass_6, 6);
	SPI_UNO_Transmit( &initial_mass_6[6], 2);
	SPI_UNO_Transmit( &initial_mass_6[8], 6);
	SPI_UNO_Transmit( &initial_mass_6[14], 2);
	SPI_UNO_Transmit( &initial_mass_6[16], 6);
	SPI_UNO_Transmit( &initial_mass_6[22], 2);
	HAL_Delay(100);
	SPI_UNO_Transmit( initial_mass_7,      6);
	SPI_UNO_Transmit( &initial_mass_7[6],  2);
	SPI_UNO_Transmit( &initial_mass_7[8],  6);
	SPI_UNO_Transmit( &initial_mass_7[14], 6);
	SPI_UNO_Transmit( &initial_mass_7[20], 6);
	SPI_UNO_Transmit( &initial_mass_7[26], 6);
	SPI_UNO_Transmit( &initial_mass_7[32], 6);
	SPI_UNO_Transmit( &initial_mass_7[38], 6);
	SPI_UNO_Transmit( &initial_mass_7[44], 6);
	SPI_UNO_Transmit( &initial_mass_7[50], 6);
	/*SPI_UNO_Transmit( &initial_mass_7[56], 2);
	SPI_UNO_Transmit( &initial_mass_7[58], 6);
	SPI_UNO_Transmit( &initial_mass_7[64], 6);
	SPI_UNO_Transmit( &initial_mass_7[70], 6);*/
	uint8_t mass[6] = { 0x10, 0x0B, 0x0B, 0xA2, 0xE8, 0xBA };
	SPI_UNO_Transmit(mass, 6);
	SPI_UNO_Transmit( &initial_mass_7[76], 2);
	SPI_UNO_Transmit( &initial_mass_7[78], 5);
		HAL_Delay(10);
		Chip_Select_Down
		HAL_SPI_Transmit(&hspix, &Read_reg_POW, 1, 1); 
		HAL_SPI_Receive(&hspix, &uno_answer, 1, 1);
		Chip_Select_Up 
		uno_answer *= 0x01; 
		if(uno_answer != 0x01)
		return ERR_UNO_Pow;
		return UNO_OK;
}
//--Выключение синтезатора--//
/*--uno_index = 0 или 1--*/
/*=============================================================================================================*/
/*!  \brief 
 * Функция выключения синтезатора и соответствующего модуля SPI	 	
     \return int
     \retval ERR_UNO_PWR_Down = -2 (синтезатор не выключился),  UNO_OK = 0
     \sa 
*/
/*=============================================================================================================*/
int uno_close (uint8_t uno_index)
{
	UNOindex(uno_index);
	SPI_UNO_Transmit(uno_off, 2);
	/*Обработчик ошибок*/
	Chip_Select_Down 
	HAL_SPI_Transmit(&hspix, &Read_reg_POW, 1,1);
	HAL_SPI_Receive(&hspix, &uno_answer, 1, 1);
	Chip_Select_Up 
	if(uno_answer != 0) 
	outputState = ERR_UNO_PWR_Down;
	outputState = UNO_OK;
	/*-Выключение SPI модуля-*/
	if (uno_index == 0){
		GPIO_InitStruct.Pin = GPIO_PIN_6; 
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		__HAL_RCC_SPI5_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
	}
	
	if(uno_index == 1){
		GPIO_InitStruct.Pin = GPIO_PIN_15;  
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
		__HAL_RCC_SPI6_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
	}
	return (outputState);
}
/*=============================================================================================================*/
/*!  \brief 
 * статическая внутренняя функция
     \return int
     \retval n_pow
     \sa 
*/
/*=============================================================================================================*/
static uint8_t func_n_pow(float fr_out)
{ 
	uint8_t n_pow;
	/*поиск n_pow показатель степени делител§; fr_out частота на выходе uno*/
	if (fr_out > 6070) n_pow = 0;
	else if (fr_out > 3035) n_pow = 1;
	else if (fr_out > 1517.5) n_pow = 2;
	else if (fr_out > 758.75) n_pow = 3;
	else if (fr_out > 379.375) n_pow = 4;
	else if (fr_out > 189.6875) n_pow = 5;
	else  n_pow = 6;
	return n_pow;
}
/*=============================================================================================================*/
/*!  \brief 
  * статическая внутренняя функция, возвращает коэф. усиления
     \return int
     \retval K
     \sa 
*/
/*=============================================================================================================*/
static uint8_t Search_K(float fr_vco2)
{
	uint8_t K;
	if (fr_vco2 < 7020) K = 6; 
	else if (fr_vco2 < 8190) {
		K = 7;
		if ((fr_vco2 >= 7068) && (fr_vco2 <= 7080)) K = 6;
		if ((fr_vco2 >= 8030) && (fr_vco2 <= 8062)) K = 8;    
	}
	else if (fr_vco2 < 9360) {
		K = 8;
		if ((fr_vco2 >= 9172) && (fr_vco2 <= 9175)) K = 9;
		if ((fr_vco2 >= 9181) && (fr_vco2 <= 9184)) K = 9;
		if ((fr_vco2 >= 9190) && (fr_vco2 <= 9220)) K = 9;
	}
	else if (fr_vco2 < 10530) {
		K = 9;
		if ((fr_vco2 >= 9390) && (fr_vco2 <= 9393)) K = 8;
		if ((fr_vco2 >= 9435) && (fr_vco2 <= 9464)) K = 8;
		if ((fr_vco2 >= 10315) && (fr_vco2 <= 10360)) K = 10;
		if ((fr_vco2 >= 10366) && (fr_vco2 <= 10370)) K = 10;
	}
	else if (fr_vco2 < 11700) {
		K = 10;
		if ((fr_vco2 >= 11019) && (fr_vco2 <= 11022)) K = 11;
		if ((fr_vco2 >= 11038) && (fr_vco2 <= 11042)) K = 11;
		if ((fr_vco2 >= 11057) && (fr_vco2 <= 11060)) K = 11;
		if ((fr_vco2 >= 11465) && (fr_vco2 <= 11469)) K = 11;
		if ((fr_vco2 >= 11476) && (fr_vco2 <= 11480)) K = 11;
		if ((fr_vco2 >= 11485) && (fr_vco2 <= 11512)) K = 11;
		if ((fr_vco2 >= 11519) && (fr_vco2 <= 11521)) K = 11;
		if ((fr_vco2 >= 11528) && (fr_vco2 <= 11531)) K = 11;
		if ((fr_vco2 >= 11545) && (fr_vco2 <= 11549)) K = 11;
		if ((fr_vco2 >= 11650) && (fr_vco2 <= 11687)) K = 11;
	}
	else if (fr_vco2 < 12870) {
		K = 11;
		if ((fr_vco2 >= 12050) && (fr_vco2 <= 12096)) K = 12;
		if ((fr_vco2 >= 12098) && (fr_vco2 <= 12124)) K = 12;
		if ((fr_vco2 >= 12142) && (fr_vco2 <= 12187)) K = 12;
		if ((fr_vco2 >= 12372) && (fr_vco2 <= 12378)) K = 12;
		if ((fr_vco2 >= 12584) && (fr_vco2 <= 12588)) K = 12;
		if ((fr_vco2 >= 12612) && (fr_vco2 <= 12615)) K = 12;
		if ((fr_vco2 >= 12635) && (fr_vco2 <= 12665)) K = 12;
		if ((fr_vco2 >= 12670) && (fr_vco2 <= 12684)) K = 12;
		if ((fr_vco2 >= 12700) && (fr_vco2 <= 12703)) K = 12;
		if ((fr_vco2 >= 12812) && (fr_vco2 <= 12856)) K = 12;
	}
	else {
		K = 12;
		if ((fr_vco2 >= 12958)&&(fr_vco2 <= 12856)) K = 11;
	}
        
	return K;
}
//Заполнение массива данными
static void FillingUnoData_0(uint32_t ftw)
{
	UnoData_0[0] = 0x10;
	UnoData_0[1] = 0x0B;
	UnoData_0[2] = (uint8_t)(ftw >> 24);
	UnoData_0[3] = (uint8_t)(ftw >> 16);
	UnoData_0[4] = (uint8_t)(ftw >> 8);
	UnoData_0[5] = (uint8_t)(ftw);
}
static void FillingUnoData_1(uint8_t n_pow)
{
	UnoData_1[0] = 0x03;
	UnoData_1[1] = (0x00 | n_pow);
}
static void FillingUnoData_2(uint8_t gain)
{
	UnoData_2[0] = 0x04;
	UnoData_2[1] = gain;
}
static void FillingUnoData_3(uint8_t K)
{
	UnoData_3[0] = 0x62;
	UnoData_3[1] = 0;
	UnoData_3[2] = 0;
	UnoData_3[3] = 0;
	UnoData_3[4] = K;
}
/*=============================================================================================================*/
/*!  \brief
freq, - желаемая частота на выходе синтезатора
gain - желаемое усиление или амплитуда на выходе синтезатора

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
void calculate_uno(float freq, uint8_t gain)
{
	uint8_t n_pow = func_n_pow(freq);
	float fr_vco2 = freq * powf(2, n_pow);
	uint8_t K = Search_K(fr_vco2);
	float fr_dds = 1200 - fr_vco2 / K;
	uint32_t ftw = (uint32_t)(roundf(fr_dds*powf(2, 32) / 2400));
	FillingUnoData_0(ftw);
	FillingUnoData_1(n_pow);
	FillingUnoData_2(gain);
	FillingUnoData_3(K);
}
/*=============================================================================================================*/
/*!  \brief
uno_index, - переменная идентификатор конкретного синтезатора 1 или 2

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
void transmit_uno(uint8_t uno_index, uint8_t* UnoData_0, uint8_t* UnoData_1, uint8_t* UnoData_2, uint8_t* UnoData_3)
{
	UNOindex(uno_index);
	SPI_UNO_Transmit(UnoData_0, 6);
	SPI_UNO_Transmit(UnoData_1, 2);
	SPI_UNO_Transmit(UnoData_2, 2);
	SPI_UNO_Transmit(UnoData_3, 5);
}
/*=============================================================================================================*/
/*!  \brief 
	uno_index, - переменная идентификатор конкретного синтезатора 1 или 2
	freq, - желаемая частота на выходе синтезатора
	gain - желаемое усиление или амплитуда на выходе синтезатора

     \return int
     \retval UNO_OK
     \sa 
*/
/*=============================================================================================================*/
int uno_write (uint8_t uno_index, float freq, uint8_t gain)
{
	float fr_out = freq;
	uint8_t dB = gain;
	uint8_t n_pow;
	UNOindex(uno_index);
	n_pow =	 func_n_pow(fr_out);
	float fr_vco2 = fr_out * powf(2, n_pow);  
	uint8_t K = Search_K(fr_vco2);
	float fr_dds = 1200 - fr_vco2 / K;
	float ftw = roundf(fr_dds*powf(2, 32) / 2400);
	uint32_t ftw_1 = (uint32_t)ftw;
	uint8_t data[6]   = { 0x10, 0x0B, (uint8_t)(ftw_1 >> 24), (uint8_t)(ftw_1 >> 16),
											(uint8_t)(ftw_1 >> 8), (uint8_t)ftw_1 };
	uint8_t data_1[2] = { 0x03, (0x00 | n_pow) };
	uint8_t data_2[2] = { 0x04, dB };
	uint8_t data_3[5] = { 0x62,0,0,0, K };
	SPI_UNO_Transmit(data, 6);
	SPI_UNO_Transmit(data_1, 2);
	/*Усиление*/
	SPI_UNO_Transmit(data_2, 2);
	SPI_UNO_Transmit(data_3, 5);
	return UNO_OK;
}

/*=============================================================================================================*/
/*!  \brief
		Усиление синтезатора. 

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
int uno_gain(uint8_t uno_index, uint8_t gain)
{
		UNOindex(uno_index);
		uint8_t data_0[2] = { 0x04, gain }; 
		uint8_t data_1[2] = { 0x14, 0 };
		SPI_UNO_Transmit(data_0, 2);
		SPI_UNO_Transmit(data_1, 2);
	
	return UNO_OK;

}



/*=============================================================================================================*/
/*!  \brief
uno_index, - переменная идентификатор конкретного синтезатора 1 или 2
freq, - желаемая частота на выходе синтезатора
gain - желаемое усиление или амплитуда на выходе синтезатора

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
int uno_set_profile(uint8_t uno_index, float freq,  uint8_t dds_profile)
{
	uint8_t dds_adr = 0x0B + 2 * dds_profile;
	float fr_out = freq;
	uint8_t n_pow;

	UNOindex(uno_index);
	n_pow = func_n_pow(fr_out);
	float fr_vco2 = fr_out * powf(2, n_pow);
	uint8_t K = Search_K(fr_vco2);
	float fr_dds = 1200 - fr_vco2 / K;
	float ftw = roundf(fr_dds*powf(2, 32) / 2400);
	uint32_t ftw_1 = (uint32_t)ftw;
	uint8_t data[6] = { 0x10, dds_adr, (uint8_t)(ftw_1 >> 24), (uint8_t)(ftw_1 >> 16),
											(uint8_t)(ftw_1 >> 8), (uint8_t)ftw_1 };
	uint8_t data_1[2] = { 0x03, (0x00 | n_pow) };
	uint8_t data_2[2] = { 0x05, (0x01 | (dds_profile << 3)) };
	uint8_t data_3[5] = { 0x62,0,0,0, K };
	SPI_UNO_Transmit(data, 6);
	SPI_UNO_Transmit(data_1, 2);
	SPI_UNO_Transmit(data_2, 2);
	SPI_UNO_Transmit(data_3, 5);
	return UNO_OK;
}


/*=============================================================================================================*/
/*!  \brief
Выбор установленного профиля

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
int uno_read_profile(uint8_t uno_index, uint8_t dds_profile,  float freq )
{
	UNOindex(uno_index);
	float fr_out = freq;
	uint8_t n_pow;
	n_pow = func_n_pow(fr_out);
	float fr_vco2 = fr_out * powf(2, n_pow);
	uint8_t K = Search_K(fr_vco2);
	
	uint8_t data_1[2] = { 0x03, (0x00 | n_pow) };
	uint8_t data_2[2] = { 0x05, (0x01 | (dds_profile << 3)) };
	uint8_t data_3[5] = { 0x62,0,0,0, K };
	
	SPI_UNO_Transmit(data_1, 2);
	SPI_UNO_Transmit(data_2, 2);
	SPI_UNO_Transmit(data_3, 5);
	return UNO_OK;
}



/*=============================================================================================================*/
/*!  \brief
Выбор установленного профиля. При неизменных K и n_pow

\return int
\retval UNO_OK
\sa
*/
/*=============================================================================================================*/
int uno_read_profile_fast(uint8_t uno_index, uint8_t dds_profile)
{
	UNOindex(uno_index);
	uint8_t data[2] = { 0x05, (0x01 | (dds_profile << 3)) };
	uint8_t data_1[2] = { 0x15, 0 };

	SPI_UNO_Transmit(data, 2);
	SPI_UNO_Transmit(data_1, 2);
	return UNO_OK;
}
