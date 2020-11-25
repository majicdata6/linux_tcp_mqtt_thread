#ifndef READ_CONFIG_CSV_H
#define READ_CONFIG_CSV_H

#include "tcmProData.h"	//

// #include "tcp_mqtt_thread.h"

#define DEV_CONFIG_CSV_FILE_NAME 		"./devconfig.CSV"

#define DEV_TYPE_NAME_4CH_CTRL			"4channelCtrl"
#define DEV_TYPE_NAME_LED_CTRL			"3ledCtrl"
#define DEV_TYPE_NAME_CURTAIN_CTRL 		"curtainCtrl"
#define DEV_TYPE_NAME_TEMP_CTRL			"tempCtrl"

#define DEV_TYPE_NAME_BODY_SENSOR		"bodySensor"
#define DEV_TYPE_NAME_HUMTEMP_SENSOR	"humTempSensor"
#define DEV_TYPE_NAME_CURTAIN_SENSOR	"curtainSensor"

#define DEV_TYPE_NAME_KEY_SWITCH		"keySwitch"

#define CMD_EN							"YES"
#define CMD_DISEN						"NO"

#define DEV_ID_LEN 						TCM_ID_LEN
#define DEV_ID_STR_LEN					(2*DEV_ID_LEN)

#define DEV_NAME_LEN 					32
#define MQTT_TOPIC_NAME_LEN				64



//结构体宏

typedef struct tDev_4channelCtl
{
	int 	devNum;
	char 	devName[DEV_NAME_LEN];
	uint8_t devId[DEV_ID_LEN];
	char	readCmd[4];
	char 	writeCmd[4];
	
	uint8_t statusCh1;
	uint8_t statusCh2;
	uint8_t statusCh3;
	uint8_t statusCh4;
	
	char 	mqttTopicName[MQTT_TOPIC_NAME_LEN];
	char	mqttData[60];
	int 	mqttDataLen;

}tDev_4channelCtl_Typedef;


typedef struct tDevTypeNodeTotal
{
	int dev4chCtrlTotal;
	tDev_4channelCtl_Typedef *ptDev4ChCtl;

	int dev3ledCtrlTotal;
	int devCurtainCtrlTotal;
	int devTempCtrlTotal;
	int devBodySensorTotal;
	int devHumTempSensorTotal;
	int devCurtainSensorTotal;
	int devKeySwitchTotal;
}tDevTypeNodeTotalTypdef;

//外部变量
extern tDevTypeNodeTotalTypdef tDevTypeNodeTotal;



//函数
//获取配置文件中配置设备数 
//profile 文件路径名
//s 设备名字符串
int getNameCount(char *profile, char *s);

//字符串转大写 str必须是字符串
char *strupr(char *str);

//字符串转小写 str必须是字符串
char *strlowr(char *str);

//字符串转HEX
/*************************************************
Function: 		string2hex
Description: 	字符串转换成hex,要求str只能是大写字母ABCDEF和数字
Input: 			str:要转换的字符串
Output: 		hex:转换后的hex字符数组
Return: 		0 成功
                1 不符合规则，失败
*************************************************/
int string2hex(char* str,char* hex);

//返回1-成功，返回0-失败，返回2-配置文件设备超出设备buff节点数
uint8_t get4lCtrlConfig(char *profile, tDev_4channelCtl_Typedef *ptDevBuf, int getDevNum);

//设备配置初始化
//返回1成功 返回0失败 返回2-配置文件设备超出设备buff节点数
uint8_t initDevConfig(char *pDevTypeName);

//根据设备ID	获取设备4路控制器节点号（1~n）0没找到
uint8_t getDev4ChCtlNode(uint8_t *pDevId);

#endif