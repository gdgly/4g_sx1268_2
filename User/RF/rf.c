#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <math.h>
#include <string.h>
#include "sx126x.h"
#include "sx126x_hal.h"
#include "radio.h"



#define RF1 0
#define RF2 1


#define BIT_RF1 (1)
#define BIT_RF2 (1<<1)

typedef struct _rf_status_manage
{
	int8_t rf_work_status;
	uint32_t rf_send_count;
	uint32_t rf_send_ok_count;	
}struct_rf_status_manage;



struct_rf_status_manage rf_status_manage[2];

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



int32_t rf_set_ch(uint8_t rf_index,uint32_t freq);






extern osThreadId_t rf_callbackHandle;
extern SX126x_t *p_sx126x;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;;
	
	if(GPIO_PIN_3 == GPIO_Pin)
	{
		if(rf_callbackHandle != NULL)
		{
			xTaskNotifyFromISR(rf_callbackHandle,BIT_RF2,eSetBits,&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
	}
	else if(GPIO_PIN_9 == GPIO_Pin)
	{
		if(rf_callbackHandle != NULL)
		{		
			xTaskNotifyFromISR(rf_callbackHandle,BIT_RF1,eSetBits,&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );		
		}
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

	rf_set_ch(RF1,470000000);
	rf_set_ch(RF2,470000000);
	
	USE_RF_1
	RadioStatus = SX126xGetStatus();
	rf_status_manage[0].rf_work_status = RadioStatus.Fields.ChipMode;		
	
	USE_RF_2
	RadioStatus = SX126xGetStatus();
	rf_status_manage[1].rf_work_status = RadioStatus.Fields.ChipMode;		
	
//	USE_RF_2
//	Radio.SetTxContinuousWave(490000000,13,0);
//	
//	RadioStatus = SX126xGetStatus();
//	RadioError =  SX126xGetDeviceErrors(  );
//	
//	USE_RF_1
//	Radio.SetTxContinuousWave(490000000,13,0);
//	
//	RadioStatus = SX126xGetStatus();
//	RadioError =  SX126xGetDeviceErrors(  );	
	
}



int32_t rf_set_ch(uint8_t rf_index,uint32_t freq)
{
	if(rf_index == RF1)
	{
		USE_RF_1
		Radio.SetChannel(freq);
		Radio.RxBoosted(5000);
	}
	else if(rf_index == RF2)
	{
		USE_RF_2
		Radio.SetChannel(freq);		
		Radio.RxBoosted(5000);		
	}
	else
		return -1;
	
	return 0;
}


int32_t rf_send(uint8_t rf_index,void *pdata,uint8_t size)
{
	if(rf_index == RF1)
	{
		USE_RF_1
		HAL_GPIO_WritePin(RF1_LED_1_GPIO_Port,RF1_LED_1_Pin,GPIO_PIN_RESET);
		rf_status_manage[RF1].rf_send_count++;
		Radio.Send(pdata,size);
		RadioStatus = SX126xGetStatus();
		rf_status_manage[RF1].rf_work_status = RadioStatus.Fields.ChipMode;
	}
	else if(rf_index == RF2)
	{
		USE_RF_2
		HAL_GPIO_WritePin(RF2_LED_2_GPIO_Port,RF2_LED_2_Pin,GPIO_PIN_RESET);		
		rf_status_manage[RF2].rf_send_count++;
		Radio.Send(pdata,size);
		RadioStatus = SX126xGetStatus();
		rf_status_manage[RF2].rf_work_status = RadioStatus.Fields.ChipMode;		
	}	
	else
		return -1;
	
	return 0;	
}












																					

void OnTxDone( void )
{
	HAL_GPIO_WritePin(RF1_LED_1_GPIO_Port,RF1_LED_1_Pin,GPIO_PIN_SET);
	rf_status_manage[RF1].rf_send_ok_count++;
  Radio.RxBoosted( 5000);
	RadioStatus = SX126xGetStatus();
	rf_status_manage[RF1].rf_work_status = RadioStatus.Fields.ChipMode;	
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.RxBoosted( 5000);

}

void OnTxTimeout( void )
{
    Radio.RxBoosted( 5000);

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
	HAL_GPIO_WritePin(RF2_LED_2_GPIO_Port,RF2_LED_2_Pin,GPIO_PIN_SET);
	rf_status_manage[RF2].rf_send_ok_count++;	
  Radio.RxBoosted( 5000);
	RadioStatus = SX126xGetStatus();
	rf_status_manage[RF2].rf_work_status = RadioStatus.Fields.ChipMode;    
}

void OnRxDone2( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.RxBoosted( 5000);

}

void OnTxTimeout2( void )
{
    Radio.RxBoosted(5000);

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
	static uint32_t times = 0;
	
	for(;;)
	{
		xTaskNotifyWait(0xffffffff,0xffffffff,&notify_value,pdMS_TO_TICKS(1000));

		if(notify_value & BIT_RF1)
		{
			USE_RF_1
			Radio.IrqProcess(&Radio_1_Events);
			continue;
		}
		if(notify_value & BIT_RF2)
		{
			USE_RF_2
			Radio.IrqProcess(&Radio_2_Events);			
			continue;
		}
		//超时 处理
		times++;
		if(times%2)
			rf_send(RF1,"1234567890",10);
		else
			rf_send(RF2,"1234567890",10);
	}
}
