
#include <string.h>
#include "dev4chCtrl.h"
#include "mycrc.h"



//�ɹ����� 1 ʧ�ܷ���0
//pStorData ָ��ͨ��״̬�洢������ʽch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1 0-�� 1-��
uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData)
{
	struct devData
	{
		uint8_t dorg;					//0x10-�ֶ�����; 0x20-���߿��ش���; 0x30-����������;0x40-���ش���;0x50-��ʱ�ϴ�
		uint8_t setDevId[DEV_ID_LEN];	//�������������豸ID
		uint8_t nodeChStatus;			//��4bit��Ӧ��������ַ 0����ַ ��4λ��Ӧͨ��״̬��ͨ��4-3-2-1�� 1����0��
		uint8_t devId[DEV_ID_LEN];		//�ƹ������ID
	}*pDevData;

	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	pDevData = (struct devData *)(&pData->commData.privateDate);

	if (0 == (0xf0 & pDevData->nodeChStatus))
	{
		uint8_t chStatus = (0x0f & pDevData->nodeChStatus);

		//ͨ��״̬ת���㷨
		for (uint8_t i = 0; i < 4; i++)
		{
			pStorData[i] = (chStatus & 0x01);
			chStatus >> 1;
		}
		return 1;
	}

	return 0;
}

//�ɹ����� 1 ʧ�ܷ���0
//pStorData ָ��洢��
//pDevId	�豸ID
//ch		���Ƶ�ͨ��	����λ��Ӧͨ��4~1��1��Ч
//status	��Ӧ��ͨ��״̬ ����λ��Ӧͨ��4~1״̬��0-�أ�1-��
uint8_t set4chCtrlData_chStatus(uint8_t *pStorData, uint8_t *pDevId, uint8_t ch, uint8_t status)
{
	//���ݳ�ʼ��
	struct tDevSetStatus
	{
		//head data
		uint8_t head				;//= AGREEMENT_CMD_HEAD;
		uint8_t dataLenH			;//= 0;
		uint8_t dataLenL			;//= 0;
		uint8_t optionalLen			;//= AGREEMENT_CMD_OPTIONAL_LEN;
		uint8_t packType			;//= AGREEMENT_CMD_PACK_TYPE_PADIO;
		uint8_t CRC8H				;//= 0;           //

		//data
		uint8_t rorg				;//= AGREEMENT_CMD_RORG_CTRL;
		uint8_t manufacturerId1		;//= AGREEMENT_CMD_MANUFACTURER_ID;
		uint8_t manuId2_MasterCmdId ;//= AGREEMENT_CMD_MID_MASTER_4L;      //
		uint8_t salveCmd			;//= AGREEMENT_CMD_SALVE_4L_SET_STATUS;

		uint8_t controlId[TCM_ID_LEN]	;//= { 0 };	//
		uint8_t nodeNumChNum			;//= 0x0f;		//4·ͨ��ͬʱ���� ���ֽ� ��Ӧ��Ӧͨ�� 1��Ч 0��Ч
		uint8_t chStatus				;//= 0;		//���ֽڶ�Ӧ��Ӧ��ͨ�� 1�� 0��
		uint8_t gatewayId[TCM_ID_LEN]	;//= { 0x88,0x88,0x88,0x88 };  //������
		uint8_t status					;//= 0;		//������ ͨ������0x00

		//constant data
		uint8_t bufConsData[AGREEMENT_CMD_OPTIONAL_LEN];// = { AGREEMENT_CMD_OPTIONA_DATA_TIMES_OF_SUB_MES,0XFF,0XFF,0XFF,0XFF,0XFF };// {0x03,0XFF,0XFF,0XFF,0XFF,0XFF}0x03�̶� Ϊ�ӱ��Ĵ���
		uint8_t CRC8D;// = 0;

	}*ptDevSetStatus;	//tDevSetStatus , *ptDevSetStatus;

	//��ʼ������
	memset(pStorData, 0x00, sizeof(struct tDevSetStatus));

	ptDevSetStatus = (struct tDevSetStatus *)pStorData;

	//head data
	ptDevSetStatus->head = AGREEMENT_CMD_HEAD;
	ptDevSetStatus->optionalLen = AGREEMENT_CMD_OPTIONAL_LEN;
	ptDevSetStatus->packType = AGREEMENT_CMD_PACK_TYPE_PADIO;

	//data
	ptDevSetStatus->rorg = AGREEMENT_CMD_RORG_CTRL;					
	ptDevSetStatus->manufacturerId1 = AGREEMENT_CMD_MANUFACTURER_ID;		//
	ptDevSetStatus->manuId2_MasterCmdId = AGREEMENT_CMD_MID_MASTER_4L;      //����ַ
	ptDevSetStatus->salveCmd = AGREEMENT_CMD_SALVE_4L_SET_STATUS;			//�ӵ�ַ

	//˽������
	//��ȡtcmID
	memcpy(ptDevSetStatus->controlId, pDevId, TCM_ID_LEN);

	//��ȡ����ͨ��
	ptDevSetStatus->nodeNumChNum &= (0x0f & ch);

	//��ȡͨ��״̬
	ptDevSetStatus->chStatus = (0x0f & status);

	
	//
	memset(ptDevSetStatus->gatewayId, 0x88, TCM_ID_LEN);//������
	memset(ptDevSetStatus->bufConsData, 0xFF, AGREEMENT_CMD_OPTIONAL_LEN);
	ptDevSetStatus->bufConsData[0] = AGREEMENT_CMD_OPTIONA_DATA_TIMES_OF_SUB_MES;

	//�������ݳ���
	ptDevSetStatus->dataLenL = sizeof(struct tDevSetStatus) - AGREEMENT_CMD_OPTIONAL_LEN - AGREEMENT_CMD_HEAD_LEN -  2;

	//����CRC8H
	SetCRC8Sub(&ptDevSetStatus->dataLenH, AGREEMENT_CMD_CRC8H_CHECK_BYTE);

	//����CRC8D
	SetCRC8Sub(&ptDevSetStatus->rorg, ptDevSetStatus->dataLenL + ptDevSetStatus->optionalLen);

	//�������ݵ�����
	memcpy(pStorData, (uint8_t *)ptDevSetStatus, sizeof(struct tDevSetStatus));

	return 1;
}
