#include <math.h>
#include <string.h>
#include "main.h"
#include "sx126x.h"
#include "sx126x_hal.h"
#include "radio.h"
#include "task.h"









uint32_t random_num;
int8_t rssi;
extern RadioStatus_t RadioStatus;
RadioError_t RadioError;



/*!
 * Radio events function pointer
 */
RadioEvents_t Radio_1_Events;
RadioEvents_t Radio_2_Events;





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





/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone2( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone2( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout2( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout2( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError2( void );

/*!
 * \brief Function executed on Radio CAD Done event
 */
void OnCadDone2( bool channelActivityDetected);











extern SX126x_t *p_sx126x;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;;
	
	if(GPIO_PIN_3 == GPIO_Pin)
	{
		xTaskNotifyFromISR();
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	else if(GPIO_PIN_9 == GPIO_Pin)
	{
		
	}
}




void rf_init()
{
	HAL_GPIO_WritePin(L506_PWR_EN_GPIO_Port,L506_PWR_EN_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(RF1_LED_CRC_ERROR_GPIO_Port,RF1_LED_CRC_ERROR_Pin,GPIO_PIN_RESET);
	

	
	Radio_1_Events.TxDone = OnTxDone;
	Radio_1_Events.RxDone = OnRxDone;
	Radio_1_Events.TxTimeout = OnTxTimeout;
	Radio_1_Events.RxTimeout = OnRxTimeout;
	Radio_1_Events.RxError = OnRxError;
	Radio_1_Events.CadDone = OnCadDone;	
	
	Radio_2_Events.TxDone = OnTxDone2;
	Radio_2_Events.RxDone = OnRxDone2;
	Radio_2_Events.TxTimeout = OnTxTimeout2;
	Radio_2_Events.RxTimeout = OnRxTimeout2;
	Radio_2_Events.RxError = OnRxError2;
	Radio_2_Events.CadDone = OnCadDone2;
	
	Radio.Init(&Radio_1_Events);

	Radio.Init(&Radio_2_Events);


	//random_num = SX126xGetRandom();


	USE_RF_2
	Radio.SetTxContinuousWave(490000000,13,0);
	
	RadioStatus = SX126xGetStatus();
	RadioError =  SX126xGetDeviceErrors(  );
	
	USE_RF_1
	Radio.SetTxContinuousWave(490000000,13,0);
	
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


void OnTxDone2( void )
{
    Radio.Standby( );
    
}

void OnRxDone2( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Standby( );

}

void OnTxTimeout2( void )
{
    Radio.Standby( );

}

void OnRxTimeout2( void )
{
    Radio.Standby( );

}

void OnRxError2( void )
{
    Radio.Standby( );

}

void OnCadDone2( bool channelActivityDetected)
{
    Radio.Standby( );

}



void task_rf_callback(void *argument)
{
	uint32_t notify_value;
	#define BIT_RF1 (1)
	#define BIT_RF2 (1<<1)

	for(;;)
	{
		xTaskNotifyWait(0xffffffff,0xffffffff,&notify_value,pdMS_TO_TICKS(1000));
		if(notify_value & BIT_RF1)
		{
			RadioIrqProcess(&Radio_1_Events);
			continue;
		}
		if(notify_value & BIT_RF2)
		{
			RadioIrqProcess(&Radio_2_Events);			
			continue;
		}
		//超时 处理

	}
}
