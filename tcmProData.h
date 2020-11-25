#ifndef TCM_PRO_DATA_H
#define TCM_PRO_DATA_H

#include <stdint.h>



#define AGREEMENT_CMD_HEAD              	0X55    //Э��ͷ
#define AGREEMENT_CMD_HEAD_LEN      		0X05    //Э��ͷ���ݳ���
#define AGREEMENT_CMD_OPTIONAL_LEN      	0X06    //ѡ�����ݳ���
#define AGREEMENT_CMD_PACK_TYPE_PADIO   	0X01    //���ߵ籨
#define AGREEMENT_CMD_PACK_TYPE_RESPONSE 	0X02    //���κ����ݰ�����Ӧ ������Ӧ��

#define AGREEMENT_CMD_RORG_KEY            	0XF6    //���߿���(��ʱ����Ҫ)
#define AGREEMENT_CMD_RORG_SENSOR           0XA5    //������(���崫��������ʪ�ȴ�������������̼���������ŴŴ�����)
#define AGREEMENT_CMD_RORG_CTRL             0XD1    //������
#define AGREEMENT_CMD_MANUFACTURER_ID   	0X68    //����ID1  (����ID=����ID1 �� ����ID2)

//�豸����	
#define AGREEMENT_CMD_MID_MASTER_4L     	0X80    //����ID2_������_4·������
#define AGREEMENT_CMD_MID_MASTER_TEMP   	0X84    //����ID2_������_���������
#define AGREEMENT_CMD_MID_MASTER_LED    	0X82    //����ID2_������_LED���������
#define AGREEMENT_CMD_MID_MASTER_CURTAIN 	0X83    //����ID2_������_����������

#define AGREEMENT_CMD_OPTIONA_DATA_TIMES_OF_SUB_MES     0X03    //ѡ������ ��1�ֽ�     = �ӱ��Ĵ��� 1�ֽ� �̶� Ĭ��0x03
#define AGREEMENT_CMD_OPTIONA_DATA_TARGET_DEV_ID        0XFF    //ѡ������ ��2~5�ֽ�   = Ŀ���豸ID 4�ֽ� Ĭ�� Ĭ��0xff ��ʾ����Ϊ�㲥״̬
#define AGREEMENT_CMD_OPTIONA_DATA_RSSI                 0XFF    //ѡ������ ��6�ֽ�     = �ź�ǿ��   1�ֽ� Ĭ�� Ĭ��0xff

#define AGREEMENT_CMD_CRC8H_CHECK_BYTE		0X04    //CRC8ͷ���㳤��     4= ���ݳ���2+ѡ�����ݳ���1+������1

#define TCM_ID_LEN							4

//4·������-����
#define AGREEMENT_CMD_SALVE_4L_READ_STATUS  0X01    //���ز�ѯ�ƹ������״̬
#define AGREEMENT_CMD_SALVE_4L_UP_CH_STATUS 0XB0    //�ƹ�������ϴ�ͨ������״̬������

#define AGREEMENT_CMD_SALVE_4L_SET_STATUS   0X10    //���ؿ��Ƶƹ������ͨ���Ŀ���


//���ݹ������ֽṹ��
typedef struct tDevPulicData
{
	//public data
	uint8_t rorg;                //= AGREEMENT_CMD_RORG;
	uint8_t manufacturerId1;     //= AGREEMENT_CMD_MANUFACTURER_ID;
	uint8_t manuId2_MasterCmdId; //= AGREEMENT_CMD_MID_MASTER_4L;      //
	uint8_t salveCmd;            //= 0;

	uint8_t privateDate;
}tDevPulicData_Typedef;

//ĩβѡ������
typedef struct tDevOptData
{
	uint8_t bufOptData[AGREEMENT_CMD_OPTIONAL_LEN];
	uint8_t crc8D;
}tDevOptData_Typdef;

//����ͷ�ṹ��
typedef struct tDevData
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




//�ⲿ����ʹ������
//��ȡ�豸ID �ɹ�����1 ʧ�ܷ���0
uint8_t getDevId(uint8_t *pTcpData, uint8_t tcpDataLen, uint8_t *pStorDevId);

//��ȡ�豸���� �������� !0 �쳣����0
uint8_t getDevType(uint8_t *pTcpData);

//��ȡ�豸ָ�� �����豸����ָ��
uint8_t getDevCmd(uint8_t *pTcpData, uint8_t devType);

//�豸ID�Ƚ� ��ȷ���1 ���ȷ���0
uint8_t compDevId(uint8_t *pDevId1, uint8_t *pDevId2);


#endif // !TCM_PRO_DATA_H

