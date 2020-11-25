
#ifndef DEAL_TCP_DATA_H
#define DEAL_TCP_DATA_H

#include <stdint.h>
#include "readConfigCsv.h"

#define TCP_READ_BUF_LEN					256
#define TCP_WRITE_BUF_LEN					256







//数据头结构体
typedef struct tTcpDataBuf
{
	uint8_t		readFlag;
	int			readLen;
	uint8_t		readData[TCP_READ_BUF_LEN];
	
	uint8_t		writeFlag;
	int			writeLen;
	uint8_t		writeData[TCP_WRITE_BUF_LEN];

}tTcpDataBuf_Typedef;


//外部变量使用声明
extern tTcpDataBuf_Typedef tTcpDataBuf;




////获取通道状态 成功返回 1 失败返回0
////pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
//uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData);

//解析网络数据包
//返回1正常，返回0失败
uint8_t decodeTcpData(uint8_t *pTcpData, uint8_t dataLen);





#endif