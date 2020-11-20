
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "readConfigCsv.h"



#define DEBUG_EN


tDevTypeNodeTotalTypdef tDevTypeNodeTotal;

//获取配置文件中配置设备数
//profile 文件路径名
//s 设备名字符串
int getNameCount(char *profile, char *s)
{
	FILE *fp = NULL;
	char *line,*record;
	char buffer[1024];
	if ((fp = fopen(profile, "at+")) != NULL)
	{
		fseek(fp, 0, SEEK_SET);  //定位到起始行
		char delims[] = ",";
		char *result = NULL;
		int getDevCount = 0;
		while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)//当没有读取到文件末尾时循环继续
		{
			
			record = strtok(line, ",");

			if (strcmp(record, s) == 0)//当第一列读取到目标字符串s时计数
			{
				getDevCount++;
				continue;
			}
 
		}
		fclose(fp);
		fp = NULL;

		#ifdef DEBUG_EN
			printf("dev-%s=%d\n", s,getDevCount);
		#endif

		return getDevCount;
	}
}

//数值转字符串
char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum
 
    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位
 
    }while(unum);//直至unum为0退出循环
 
    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整
 
    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }
 
    return str;//返回转换后的字符串
}

//字符串转大写 str必须是字符串
char *strupr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);	//#include <ctype.h>
    return orign;
}

//字符串转小写 str必须是字符串
char *strlowr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = tolower(*str);	//#include <ctype.h>
    return orign;
}

//字符串转HEX
/*************************************************
Function: 		string2hex
Description: 	字符串转换成hex,要求str只能是大写字母ABCDEF和数字
Input: 			str:要转换的字符串
Output: 		hex:转换后的hex字符数组
Return: 		0 成功
                1 不符合规则，失败
*************************************************/
int string2hex(char* str,char* hex)
{
    int i = 0;
    int j = 0;
    unsigned char temp = 0;
    int str_len = 0;
    char str_cpy[100] = {'0'};
    strcpy(str_cpy,str);
    str_len = strlen(str_cpy);
    if(str_len==0)
    {
        return 1;
    }
    while(i < str_len)
    {
        if(str_cpy[i]>='0' && str_cpy[i]<='F') 
        {
            if((str_cpy[i]>='0' && str_cpy[i]<='9'))
            {
                temp = (str_cpy[i] & 0x0f)<<4;
            }
            else if(str_cpy[i]>='A' && str_cpy[i]<='F')
            {
                temp = ((str_cpy[i] + 0x09) & 0x0f)<<4;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }   
        i++;
        if(str_cpy[i]>='0' && str_cpy[i]<='F') 
        {
            if(str_cpy[i]>='0' && str_cpy[i]<='9')
            {
                temp |= (str_cpy[i] & 0x0f);
            }
            else if(str_cpy[i]>='A' && str_cpy[i]<='F')
            {
                temp |= ((str_cpy[i] + 0x09) & 0x0f);
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        } 
        i++;
        hex[j] = temp;
        //printf("%02x",temp);
        j++;
    }
    //printf("\n");
    return 0 ;
}

//返回1-成功，返回0-失败，返回2-配置文件设备超出设备buff节点数
uint8_t get4lCtrlConfig(char *profile, tDev_4channelCtl_Typedef *ptDevBuf, int getDevNum)
{
	int getDevCount = 0;

	FILE *fp = NULL;
	char *line,*record;
	char buffer[1024];
	
	if ((fp = fopen(profile, "at+")) != NULL)
	{
		fseek(fp, 0, SEEK_SET);  //定位到起始位置
		char delims[] = ",";
		char *result = NULL;
		uint8_t column = 0;
		while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)//当没有读取到文件末尾时循环继续
		{
			record = strtok(line, delims);//获取行 第一个子字符串，注意：此时行line会变成第一个子字符串
			column = 1;

			//行处理完或无有效处理结果时column = 0, 
			while ((record != NULL) && (column != 0))//继续读取行 里剩下的列字符串
			{
				switch(column)
				{
					case 1:	//第一列内容 设备类型
					{
						if (strcmp(record, DEV_TYPE_NAME_4CH_CTRL) == 0)//当读取第一个子字符串 不等与目标字符串 跳过本行剩余字符串读取
						{
							//获取 设备配置节点计数
							getDevCount ++;

							//比较获取是否超过设备buff节点总数
							if(getDevCount > getDevNum)
							{
								#ifdef DEBUG_EN
									printf("devNum>devBuff\n");
								#endif

								return 2;//超出设备buff节点数
							}

							//设备节点号 1~n
							(ptDevBuf + getDevCount - 1)->devNum = getDevCount;

							//处理第二列内容
							column = 2;
							break;
						}

						//跳过此行处理内容
						column = 0;
					}
					break;

					case 2:	//第二列内容 设备名
					{
						char *ps=NULL;
						//获取 dev_name
						record = strtok(NULL, delims);

						#ifdef DEBUG_EN
							printf("设备读取 dev_name?=%s\n", record);
						#endif

						ps = record;
						if(*(ps-1) == ',')	//此列为空内容判断
						{
							//为空 默认设备名 设备类型_设备节点号

							int num=getDevCount;
							char nameNumStr[8];
							if(sprintf(nameNumStr,"%d",num))	//整型转字符串 #include <stdio.h>
							{
								strcpy((ptDevBuf+getDevCount - 1)->devName,DEV_TYPE_NAME_4CH_CTRL);
								strcat((ptDevBuf+getDevCount - 1)->devName,nameNumStr);
							}
							else
							{
								strcpy((ptDevBuf+getDevCount - 1)->devName,DEV_TYPE_NAME_4CH_CTRL);
								strcat((ptDevBuf+getDevCount - 1)->devName,"err");
							}
							
							//获取 dev_id
							// record = strtok(NULL, ",");
							#ifdef DEBUG_EN
								printf("设备读取 dev_id=%s\n", record);
							#endif
							int len=strlen(record);
							if(DEV_ID_STR_LEN != len)
							{
								#ifdef DEBUG_EN
									printf("设备%d_ID错误 rDevIdlen=%d\n",getDevCount, len);
								#endif

								//跳过此行处理内容
								column = 0;
								break;
							}

							//大写转换
							strupr(record);
							if(string2hex(record,(ptDevBuf+getDevCount - 1)->devId))
							{
								#ifdef DEBUG_EN
									printf("设备ID不符合规则 sDevId=%s\n", record);
								#endif

								//跳过此行处理内容
								column = 0;
								break;
							}

							//跳到指定列解析
							column = 4;
						}
						else
						{
							//获取设备名
							strcpy((ptDevBuf+getDevCount - 1)->devName,record);
							
							//跳到指定列解析
							column = 3;
						}
					}
					break;

					case 3:	//第三列内容 设备ID
					{
						//获取 dev_id
						record = strtok(NULL, delims);
						#ifdef DEBUG_EN
							printf("设备读取 dev_id=%s\n", record);
						#endif
						int len=strlen(record);
						if(DEV_ID_STR_LEN != len)
						{
							#ifdef DEBUG_EN
								printf("设备%d_ID错误 rDevIdlen=%d\n",getDevCount, len);
							#endif

							//跳过此行处理内容
							column = 0;
							break;
						}

						//大写转换
						strupr(record);
						if(string2hex(record,(ptDevBuf+getDevCount - 1)->devId))
						{
							#ifdef DEBUG_EN
								printf("设备ID不符合规则 sDevId=%s\n", record);
							#endif

							//跳过此行处理内容
							column = 0;
							break;
						}

						//跳到指定列解析
						column = 4;
					}
					break;

					case 4:	//第四列内容 设备读使能
					{
						//获取 dev_cmd_read
						record = strtok(NULL, delims);
						#ifdef DEBUG_EN
							printf("设备读取 dev_cmd_read=%s\n", record);
						#endif
						
						//转换大写
						strupr(record);
						if(strcmp(CMD_EN,record) == 0)
						{
							strcpy((ptDevBuf+getDevCount - 1)->readCmd,CMD_EN);
						}
						else
						{
							strcpy((ptDevBuf+getDevCount - 1)->readCmd,CMD_DISEN);
						}

						//跳到指定列解析
						column = 5;
					}
					break;

					case 5:	//第四列内容 设备写使能
					{
						//获取 dev_cmd_write
						record = strtok(NULL, delims);
						#ifdef DEBUG_EN
							printf("设备读取 dev_cmd_write=%s\n", record);
						#endif
						
						strupr(record);
						if(strcmp(CMD_EN,record) == 0)
						{
							strcpy((ptDevBuf+getDevCount - 1)->writeCmd,CMD_EN);
						}
						else
						{

							strcpy((ptDevBuf+getDevCount - 1)->writeCmd,CMD_DISEN);
						}

						//跳到指定列解析 0为结束本行解析，开始下一行
						column = 0;
					}
					break;
					
				}
			}
 
		}
		fclose(fp);
		fp = NULL;
	}

	if(getDevCount)
	{
		return 1;//成功
	}
	else
	{
		return 0;//失败
	}
}


//设备配置初始化
//返回1成功 返回0失败 返回2-配置文件设备超出设备buff节点数
uint8_t initDevConfig(char *pDevTypeName)
{

	if(strcmp(DEV_TYPE_NAME_4CH_CTRL, pDevTypeName) == 0)
	{
		//获取数量
		tDevTypeNodeTotal.dev4chCtrlTotal = getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_4CH_CTRL);
		
		if(tDevTypeNodeTotal.dev4chCtrlTotal > 0)
		{
			//分配空间
			tDevTypeNodeTotal.ptDev4ChCtl = (tDev_4channelCtl_Typedef*)calloc(tDevTypeNodeTotal.dev4chCtrlTotal, sizeof(tDev_4channelCtl_Typedef));
			//初始化设备配置内容
			return get4lCtrlConfig(DEV_CONFIG_CSV_FILE_NAME, tDevTypeNodeTotal.ptDev4ChCtl, tDevTypeNodeTotal.dev4chCtrlTotal);
		
		}
	}

	// if(strcmp(DEV_TYPE_NAME_LED_CTRL, pDevTypeName) == 0)
	// {
	// 	//获取数量
	// 	tDevTypeNodeTotal.dev3ledCtrlTotal = getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_LED_CTRL);
	// 	//分配空间
	// 	tDevTypeNodeTotal.ptDev4ChCtl = (tDev_4channelCtl_Typedef*)calloc(tDevTypeNodeTotal.dev4chCtrlTotal, sizeof(tDev_4channelCtl_Typedef));
	// }


	return 0;
}
