#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DEV_ID_LEN 4

#define DEBUG_EN

//获取设备ID 成功返回1 失败返回0
uint8_t getDevId(uint8_t *pTcpData, uint8_t tcpDataLen, uint8_t *pStorDevId)
{
	uint8_t *p = pTcpData;

	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;
	uint8_t dataLen = pData->dataLenL + pData->optionalLen + AGREEMENT_CMD_HEAD_LEN + 1;
	if(tcpDataLen != dataLen)
	{
		#ifdef DEBUG_EN
			printf("数据长度错误-接收长度=%d -计算数据长度=%d\n",tcpDataLen,dataLen);
		#endif
		return 0;
	}

	//
	for(uint8_t i=0; i<DEV_ID_LEN; i++)
	{
		pStorDevId[3-i] = *(p-pData.optionalLen -1 -i);
	}

	return 1;
}

//获取设备类型 正常返回 !0 异常返回0
uint8_t getDevType(uint8_t *pTcpData)
{
	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	//控制器
	if(AGREEMENT_CMD_RORG_CTRL == pData->commData.rorg)
	{
		switch(pData->commData.manuId2_MasterCmdId)
		{
			case AGREEMENT_CMD_MID_MASTER_4L:
			{
				return AGREEMENT_CMD_MID_MASTER_4L;
			}
			break;

			case AGREEMENT_CMD_MID_MASTER_LED:
			{
				return AGREEMENT_CMD_MID_MASTER_LED;
			}
			break;

			case AGREEMENT_CMD_MID_MASTER_CURTAIN:
			{
				return AGREEMENT_CMD_MID_MASTER_CURTAIN;
			}
			break;

			case AGREEMENT_CMD_MID_MASTER_TEMP:
			{
				return AGREEMENT_CMD_MID_MASTER_TEMP;
			}
			break;


		}

		//传感器 人体感应数据（传感器电源，照度值，保留0x00，人体值）
		//		温湿度数据（温度正负，湿度值，温度值，保留0x00）
		//		二氧化碳数据（温度值，二氧化碳浓度，保留0x8F）
		//		窗磁数据（保留0x00,电压值，门窗状态，保留0X08）
		if(AGREEMENT_CMD_RORG_SENSOR == pData->commData.rorg)
		{

		}

		// //按键-(暂时不需要)
		// if(AGREEMENT_CMD_RORG_KEY == pData->commData.rorg)
		// {

		// }

	}
}

//获取设备指令 返回设备操作指令
uint8_t getDevCmd(uint8_t *pData, uint8_t devType)
{
	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	switch(devType)
	{
		case AGREEMENT_CMD_MID_MASTER_4L:
		{
			return(pData->commData.salveCmd)
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_LED:
		{
			return(pData->commData.salveCmd)
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_CURTAIN:
		{
			return(pData->commData.salveCmd)
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_TEMP:
		{
			return(pData->commData.salveCmd)
		}
		break;

	}

}

//设备ID比较 相等返回1 不等返回0
uint8_t compDevId(uint8_t *pDevId1, uint8_t *pDevId2)
{
	for(uint8_t i=0; i<DEV_ID_LEN; i++)
	{
		if(*(pDevId1 + i) != *(pDevId2 + i))
		{
			return 0;
		}
	}
	return 1;
}

//成功返回 1 失败返回0
//pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
uint8_t get_4chCtrl_chStatus(uint8_t *pTcpData, uint8_t *pStorData)
{
	struct devData
	{
		uint8_t dorg;					//0x10-手动触发; 0x20-无线开关触发; 0x30-传感器触发;0x40-网关触发;0x50-定时上传
		uint8_t setDevId[DEV_ID_LEN];	//触发控制器的设备ID
		uint8_t nodeChStatus;			//高4bit对应控制器地址 0主地址 低4位对应通道状态（通道4-3-2-1） 1开，0关
		uint8_t devId[DEV_ID_LEN];		//灯光控制器ID
	}*pDevData;

	pDevData = (struct devData *)&pTcpData->commData.privateDate;

	if((0xf0 & pDevData->nodeChStatus) == 0)
	{
		uint8_t chStatus = (0x0f & pDevData->nodeChStatus);
		
		//
		for(uint8_t i=0; i<4; i++)
		{
			pStorData[i] = (chStatus & 0x01);
			chStatus >>1;
		}
		return 1;
	}

	return 0;
}