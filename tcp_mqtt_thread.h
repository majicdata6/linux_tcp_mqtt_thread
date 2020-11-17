#ifndef TCP_MQTT_THREAD_H
#define TCP_MQTT_THREAD_H




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


typedef struct 
{
	int 	devNum;
	char 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];
	
	uint8_t statusCh1;
	uint8_t statusCh2;
	uint8_t statusCh3;
	uint8_t statusCh4;
	

	char 	mqttTopicName[MQTT_TOPIC_NAME_LEN];
	char	mqttData[60];
	int 	mqttDataLen;

}tDev_4channelCtl_Typedef;



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























#endif