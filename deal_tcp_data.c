#include <stdio.h>
#include <string.h>

#include "deal_tcp_data.h"



#define DEBUG_EN

tTcpDataBuf_Typedef tTcpDataBuf;

////成功返回 1 失败返回0
////pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
//uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData)
//{
//	struct devData
//	{
//		uint8_t dorg;					//0x10-手动触发; 0x20-无线开关触发; 0x30-传感器触发;0x40-网关触发;0x50-定时上传
//		uint8_t setDevId[DEV_ID_LEN];	//触发控制器的设备ID
//		uint8_t nodeChStatus;			//高4bit对应控制器地址 0主地址 低4位对应通道状态（通道4-3-2-1） 1开，0关
//		uint8_t devId[DEV_ID_LEN];		//灯光控制器ID
//	}*pDevData;
//
//	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;
//
//	pDevData = (struct devData *)(&pData->commData.privateDate);
//
//	if (0 == (0xf0 & pDevData->nodeChStatus))
//	{
//		uint8_t chStatus = (0x0f & pDevData->nodeChStatus);
//
//		//通道状态转换算法
//		for (uint8_t i = 0; i < 4; i++)
//		{
//			pStorData[i] = (chStatus & 0x01);
//			chStatus >> 1;
//		}
//		return 1;
//	}
//
//	return 0;
//}


//解析网络数据包
//pTcpData	解析数据起始地址
//dataLen	解析数据的长度
//返回1正常，返回0失败
uint8_t decodeTcpData(uint8_t *pTcpData, uint8_t dataLen)
{
	uint8_t dealType = 0;
	uint8_t devId[DEV_ID_LEN] = { 0 };
	uint8_t devNote = 0;
	tDevTypeNodeTotalTypdef *ptDevNodeInfo = &tDevTypeNodeTotal;

	//获取设备大类型（传感器/控制器）
	dealType = getDevType(pTcpData);

	//获取设备数据
	switch (dealType)
	{
	case AGREEMENT_CMD_MID_MASTER_4L:
	{
		//获取设备ID
		if (0 == getDevId(pTcpData, dataLen, devId))
		{
			return 0;
		}

		//查找设备ID数据地址
		devNote = getDev4ChCtlNode(devId);
		if (0 == devNote)
		{
			return 0;
		}

		//获取设备数据
		if (0 == get4chCtrlData_chStatus(pTcpData, ptDevNodeInfo->ptDev4ChCtl[devNote - 1].devId))
		{
			return 0;
		}
	}
	break;
	case AGREEMENT_CMD_MID_MASTER_TEMP:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_LED:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_CURTAIN:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_SENSOR:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_KEY:
	{

	}
	break;
	default:
		return 1;
		break;
	}
	return 1;
}

//编码网络数据包
//ptDevNode	要编码数据的节点地址
//cmd		编码指令
//返回1正常，返回0失败
uint8_t encodeTcpData(uint8_t devType, uint8_t *ptDevNode, uint8_t cmd)
{

	switch (devType)
	{
	case AGREEMENT_CMD_MID_MASTER_4L:
	{
		////获取设备id
		//if (0 == getdevid(ptcpdata, datalen, devid))
		//{
		//	return 0;
		//}

		////查找设备id数据地址
		//devnote = getdev4chctlnode(devid);
		//if (0 == devnote)
		//{
		//	return 0;
		//}

		////获取设备数据
		//if (0 == get4chctrldata_chstatus(ptcpdata, ptdevnodeinfo->ptdev4chctl[devnote - 1].devid))
		//{
		//	return 0;
		//}
	}
	break;
	case AGREEMENT_CMD_MID_MASTER_TEMP:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_LED:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_CURTAIN:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_SENSOR:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_KEY:
	{

	}
	break;
	default:
		return 1;
		break;
	}
}


