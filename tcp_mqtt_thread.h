#ifndef TCP_MQTT_THREAD_H
#define TCP_MQTT_THREAD_H


#include "readConfigCsv.h"

//macro definition 


#define NUM_THREADS	    		4


//The macro structure

typedef struct 
{
	int tcp_send_flag ;

	char *pData;
	int dataLen;

}tTcpMessageDealTypedef;

typedef struct 
{
	int mqtt_push_flag;
	int mqtt_read_flag;
	int mqtt_writ_flag;

	char *pMqttTopic;
	int mqttTopicLen;
	char *pMqttMessgae;
	int mqttMessageLen;

}tMqttMessageDealTypedef;



/*
typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_CurtainCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_LedCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_TempCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_PWSwitchCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_DoorMagneticCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_BodySensor_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_HumTempSensor_Typedef;


*/




















#endif