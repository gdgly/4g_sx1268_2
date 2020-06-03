#include <math.h>
#include <string.h>
#include "main.h"
#include "sx126x.h"
#include "sx126x_hal.h"
#include "radio.h"


#define TX_OUTPUT_POWER                             22        // dBm

#define LORA_BANDWIDTH                              2         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       5         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        12       // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         100       // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  False
#define LORA_IQ_INVERSION_ON                        False
#define LORA_FIX_LENGTH_PAYLOAD_LEN                 19


#define LORA_FIX_LENGTH_PAYLOAD_OFF                  True













/*!
 * Radio events function pointer
 */
static RadioEvents_t Radio_1_Events;
static RadioEvents_t Radio_2_Events;





/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

/*!
 * \brief Function executed on Radio CAD Done event
 */
void OnCadDone( bool channelActivityDetected);












extern SX126x_t *p_sx126x;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_PIN_3 == GPIO_Pin)
	{
		
	}
	else if(GPIO_PIN_9 == GPIO_Pin)
	{
		
	}
}
uint32_t random_num;
int8_t rssi;
extern RadioStatus_t RadioStatus;
RadioError_t RadioError;
void rf_init()
{
	HAL_GPIO_WritePin(L506_PWR_EN_GPIO_Port,L506_PWR_EN_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(RF1_LED_CRC_ERROR_GPIO_Port,RF1_LED_CRC_ERROR_Pin,GPIO_PIN_RESET);
	USE_RF_1
	
	
	
	USE_RF_2	
		// Radio initialization
	Radio_2_Events.TxDone = OnTxDone;
	Radio_2_Events.RxDone = OnRxDone;
	Radio_2_Events.TxTimeout = OnTxTimeout;
	Radio_2_Events.RxTimeout = OnRxTimeout;
	Radio_2_Events.RxError = OnRxError;
	Radio_2_Events.CadDone = OnCadDone;
	
	Radio.Init(&Radio_2_Events);
	rssi = SX126xReadRegister(0x0740);
	rssi = SX126xReadRegister(0x0741);
	SX126xWriteRegister(0x0740,0x10);
	rssi = SX126xReadRegister(0x0740);
//	Radio.SetTxConfig(MODEM_LORA,TX_OUTPUT_POWER,0,LORA_BANDWIDTH,LORA_SPREADING_FACTOR,LORA_CODINGRATE,LORA_PREAMBLE_LENGTH,
//	False,True,0, 0, False, 3000 );

//	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
//                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
//                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
//                                   0, True, 0, 0, LORA_IQ_INVERSION_ON, True );


random_num = SX126xGetRandom();

	SX126xSetStandby(0);
	RadioStatus = SX126xGetStatus();
	//SX126xReset( );
//	SX126xSetStandby(1);
//	RadioStatus = SX126xGetStatus();
//	RadioStatus = SX126xGetStatus();
//	rssi = Radio.Rssi(0);
	SX126xSetFs();
	RadioStatus = SX126xGetStatus();
	Radio.SetTxContinuousWave(490000000,22,0);
	RadioStatus = SX126xGetStatus();
	Radio.Standby();
	RadioStatus = SX126xGetStatus();
	RadioError =  SX126xGetDeviceErrors(  );
}
																																		
																					

void OnTxDone( void )
{
    Radio.Standby( );
    
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Standby( );

}

void OnTxTimeout( void )
{
    Radio.Standby( );

}

void OnRxTimeout( void )
{
    Radio.Standby( );

}

void OnRxError( void )
{
    Radio.Standby( );

}

void OnCadDone( bool channelActivityDetected)
{
    Radio.Standby( );


}
