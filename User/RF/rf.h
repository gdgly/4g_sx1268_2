#ifndef RF_H_
#define RF_H_






#define RF1 0
#define RF2 1


#define BIT_RF1 (1)
#define BIT_RF2 (1<<1)


// rf状态记录表
typedef struct _rf_status_manage
{
	int8_t rf_work_status;         //rf 所处的tx rx sb 等操作模式
	uint32_t rf_send_count;        //发送的包数
	uint32_t rf_send_ok_count;	   //发送ok的包数
	volatile uint32_t rf_last_rx_tick;  //最后一条收到数据的tick点
}struct_rf_status_manage;










#endif

