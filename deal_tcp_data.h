
#ifndef DEAL_TCP_DATA_H
#define DEAL_TCP_DATA_H

#define AGREEMENT_CMD_HEAD              	0X55    //协议头
#define AGREEMENT_CMD_HEAD_LEN      		0X05    //协议头数据长度
#define AGREEMENT_CMD_OPTIONAL_LEN      	0X06    //选项数据长度
#define AGREEMENT_CMD_PACK_TYPE_PADIO   	0X01    //无线电报
#define AGREEMENT_CMD_PACK_TYPE_RESPONSE 	0X02    //对任何数据包的响应 发送自应答

#define AGREEMENT_CMD_RORG_KEY            	0XF6    //无线开关(暂时不需要)
#define AGREEMENT_CMD_RORG_SENSOR           0XA5    //传感器(人体传感器，温湿度传感器，二氧化碳传感器，门磁传感器)
#define AGREEMENT_CMD_RORG_CTRL             0XD1    //控制器
#define AGREEMENT_CMD_MANUFACTURER_ID   	0X68    //厂家ID1  (厂家ID=厂家ID1 和 厂家ID2)
	
//设备类型	
#define AGREEMENT_CMD_MID_MASTER_4L     	0X80    //厂家ID2_主命令_4路控制器
#define AGREEMENT_CMD_MID_MASTER_TEMP   	0X84    //厂家ID2_主命令_风机控制器
#define AGREEMENT_CMD_MID_MASTER_LED    	0X82    //厂家ID2_主命令_LED调光控制器
#define AGREEMENT_CMD_MID_MASTER_CURTAIN 	0X83    //厂家ID2_主命令_窗帘控制器



#define AGREEMENT_CMD_CRC8H_CHECK_BYTE  0X04    //CRC8头计算长度     4= 数据长度2+选项数据长度1+包类型1

//数据公共部分结构体
typedef struct
{
    //public data
    uint8_t rorg;                //= AGREEMENT_CMD_RORG;
    uint8_t manufacturerId1;     //= AGREEMENT_CMD_MANUFACTURER_ID;
    uint8_t manuId2_MasterCmdId; //= AGREEMENT_CMD_MID_MASTER_4L;      //
    uint8_t salveCmd;            //= 0;

    uint8_t privateDate;
}tDevPulicData_Typedef;

//末尾选项数据
typedef struct 
{
	uint8_t bufOptData[AGREEMENT_CMD_OPTIONAL_LEN];
	uint8_t crc8D;
}tDevOptData_Typdef;

//数据头结构体
typedef struct
{
	//head data
    uint8_t head;                //= AGREEMENT_CMD_HEAD;
    uint8_t dataLenH;            //= 0;
    uint8_t dataLenL;            //= 0;
    uint8_t optionalLen;         //= AGREEMENT_CMD_OPTIONAL_LEN;
    uint8_t packType;            //= AGREEMENT_CMD_PACK_TYPE_PADIO;
    uint8_t CRC8H;               //= 0;           //

    tDevPulicData_Typedef commData;
    // uint8_t dataStart;
}tDevData_Typedef;


//函数声明
//获取设备ID 成功返回1 失败返回0
uint8_t getDevId(uint8_t *pTcpData, uint8_t tcpDataLen, uint8_t *pStorDevId);

//获取设备类型 正常返回 !0 异常返回0
uint8_t getDevType(uint8_t *pTcpData);

//获取设备指令 返回设备操作指令
uint8_t getDevCmd(uint8_t *pData, uint8_t devType);

//设备ID比较 相等返回1 不等返回0
uint8_t compDevId(uint8_t *pDevId1, uint8_t *pDevId2);.

//获取通道状态 成功返回 1 失败返回0
//pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
uint8_t get_4chCtrl_chStatus(uint8_t *pTcpData, uint8_t *pStorData);





#endif