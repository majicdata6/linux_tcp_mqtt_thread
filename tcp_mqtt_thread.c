#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

//#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <string.h>
// #include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <stdint.h>	//uint8_t
#include "ccl.h"
#include "cJSON.h"

#include "tcp_mqtt_thread.h"



// #define ADDRESS         "172.16.254.211:1883"           //更改此处地址
// #define CLIENTID        "aaabbbccc_pub"                 //更改此处客户端ID
// #define SUB_CLIENTID    "aaabbbccc_sub"                 //更改此处客户端ID
// #define TOPIC           "mytest"                        //更改发送的话题
// // #define PAYLOAD         "Hello Man, Can you see me ?!"  //
// #define QOS             1
// #define TIMEOUT         10000L
// #define USERNAME        "test_user"
// #define PASSWORD	    "jim777"
#define DISCONNECT	    "out"

#define true    1
#define false   0

//消息发送同步
int CONNECT = 1;			//
int PUB_FLAG = false;
char buf[1024] = {0};

int TCP_SEND_FLAG = false;	//tcp下发标志位
char s_buf[1024]={0};		
int s_buf_send_len = 0;		

char *payload=buf;
int payloadlen=0;





//tcp 客户端连接服务器地址和端口
char ip_buf[16]={0};

char *server_ip_addr = ip_buf;
int server_ip_port = 10086;     //默认值

//mqtt 客户端连接配置
char mqtt_server_address[25]={0};			//mqtt 服务器地址
char mqtt_pub_client_id[25]={0};			//更改此处客户端ID
char mqtt_sub_client_id[25]={0};			//更改此处客户端ID
int mqtt_pub_qos=1;
int mqtt_sub_qos=1;
int mqtt_timeout=1000;
char mqtt_client_username[25]={0};
char mqtt_client_password[25]={0};

char mqtt_pub_client_topic[256]={0};


//全局变量
tDev_4channelCtl_Typedef tDev4chCtl;


volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i;
	char* payloadptr;

	//
	memset(s_buf,'\0',sizeof(s_buf));
	strcpy(s_buf,topicName);
	int str_len = 0;
	str_len = strlen(s_buf);
	s_buf_send_len = str_len;

	strcpy(&s_buf[str_len],message->payload);
	s_buf_send_len += message->payloadlen;

	// //json数据解析
	// cJSON *rjson=cJSON_Parse(s_buf);
	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
	// printf("%s\n", json_item->valuestring);


	TCP_SEND_FLAG = true;

	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");

	payloadptr = message->payload;
	if(strcmp(payloadptr, DISCONNECT) == 0)
	{
		printf(" \n out!!");
		CONNECT = 0;
	}


	
	for(i=0; i<message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	printf("\n");
	
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}

void *subClient(void *threadid)
{
	long tid;
	tid = (long)threadid;
	printf("Hello World! It's me, thread #%ld!\n", tid);
   
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;
	int ch;

	MQTTClient_create(&client, mqtt_server_address, mqtt_sub_client_id,MQTTCLIENT_PERSISTENCE_NONE, NULL);
	
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = mqtt_client_username;
	conn_opts.password = mqtt_client_password;
	
	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
		   "Press Q<Enter> to quit\n\n", tDev4chCtl.mqttTopicName, mqtt_sub_client_id, mqtt_sub_qos);
	
	MQTTClient_subscribe(client, tDev4chCtl.mqttTopicName, mqtt_sub_qos);

	do 
	{
		ch = getchar();
	} while(ch!='Q' && ch != 'q');

	MQTTClient_unsubscribe(client, tDev4chCtl.mqttTopicName);
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
   
   pthread_exit(NULL);
}

void *pubClient(void *threadid)
{
   long tid;
   tid = (long)threadid;
   int count = 0;
   printf("Hello World! It's me, thread #%ld!\n", tid);
	
	//声明一个MQTTClient
	MQTTClient client;
	//初始化MQTT Client选项
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	//#define MQTTClient_message_initializer { {'M', 'Q', 'T', 'M'}, 0, 0, NULL, 0, 0, 0, 0 }
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	//声明消息token
	MQTTClient_deliveryToken token;
	
	int rc;
	
	//使用参数创建一个client，并将其赋值给之前声明的client
	MQTTClient_create(&client, mqtt_server_address, mqtt_pub_client_id,MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = mqtt_client_username;
	conn_opts.password = mqtt_client_password;

	 //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	
	while(CONNECT)
	{
		if(PUB_FLAG)
		{
			// pubmsg.payload = payload;//;
			// pubmsg.payloadlen = payloadlen; //
			pubmsg.payload = tDev4chCtl.mqttData;//;
			pubmsg.payloadlen = tDev4chCtl.mqttDataLen; //
			pubmsg.qos = mqtt_pub_qos;
			pubmsg.retained = 0;
			
			PUB_FLAG = false;
			MQTTClient_publishMessage(client, mqtt_pub_client_topic, &pubmsg, &token);
			printf("Waiting for up to %d seconds for publication of %s\n"
					"on topic %s for client with ClientID: %s\n",
					(int)(mqtt_timeout/1000), payload, mqtt_pub_client_topic, mqtt_pub_client_id);
			rc = MQTTClient_waitForCompletion(client, token, mqtt_timeout);
			printf("Message with delivery token %d delivered\n", token);
		}
	
	// usleep(3000000L);
	}
	
	
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
}

void *tcpClient_r(void *threadid)
{
	long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);

	// char ip_buf[16]={0};

	// char *server_ip_addr = ip_buf;
	// int server_ip_port = 10086;     //默认值

	// //读取配置文件，获取ip/port
	// GetProfileString("./tcpclient.conf", "tcpclient_ip", ip_buf);
	// GetProfileInt("./tcpclient.conf", "tcpclient_port", &server_ip_port);

	// char *server_ip_addr = "172.16.18.125";
	// int server_ip_port = 10086;
	char *send_message = "hello";
	// char buf[1024] = {0};
	int rec_len = 0;

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) 
	{
		fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
	}

	struct sockaddr_in t_sockaddr;
	memset(&t_sockaddr, 0, sizeof(struct sockaddr_in));

	t_sockaddr.sin_family = AF_INET;
	t_sockaddr.sin_port = htons(server_ip_port);

	inet_pton(AF_INET, server_ip_addr, &t_sockaddr.sin_addr);

	if((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0 ) 
	{
		fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
		// return 0;
		exit(1);  
	}

	while(1)
	{

		if((rec_len = recv(socket_fd, buf, sizeof(buf), 0)) == -1) 
		{  
		   perror("recv error");  
		   exit(1);  
		}
		buf[rec_len]='\0';

		payload = buf;
		payloadlen = rec_len+1;
		PUB_FLAG = true;
		
		printf("%s\n",buf);
	}
	
	close(socket_fd);
	socket_fd = -1;
}

void *tcpClient_w(void *threadid)
{
	long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);


	char *send_message = "hello";
	
	int rec_len = 0;

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) 
	{
		fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
	}

	struct sockaddr_in t_sockaddr;
	memset(&t_sockaddr, 0, sizeof(struct sockaddr_in));

	t_sockaddr.sin_family = AF_INET;
	t_sockaddr.sin_port = htons(server_ip_port);

	inet_pton(AF_INET, server_ip_addr, &t_sockaddr.sin_addr);

	if((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0 ) 
	{
		fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
		// return 0;
		exit(1);  
	}


	while(1)
	{

		// rec_len=send(socket_fd,buf,strlen(buf),0);

		if(TCP_SEND_FLAG)
		{
			TCP_SEND_FLAG = false;
			// rec_len=send(socket_fd, s_buf, s_buf_send_len, 0);
			rec_len=send(socket_fd, &tDev4chCtl.statusCh1, 4, 0);
			printf("send_tcp_data_len=%d\n", rec_len);
			rec_len = 0;
		}

	}
	
	close(socket_fd);
	socket_fd = -1;
}

int main(int argc, char* argv[])
{
	pthread_t threads[NUM_THREADS];
	// long t;

	

	//tcp客户端 读取配置文件，获取ip/port
	GetProfileString("./tcpclient.conf", "tcpclient_ip", ip_buf);
	GetProfileInt("./tcpclient.conf", "tcpclient_port", &server_ip_port);

	//mqtt客户端 读取配置文件
	GetProfileString("./mqttclient.conf", "mqtt_address", mqtt_server_address);
	GetProfileString("./mqttclient.conf", "mqtt_clientid", mqtt_pub_client_id);
	GetProfileString("./mqttclient.conf", "mqtt_sub_clientid", mqtt_sub_client_id);
	GetProfileInt("./mqttclient.conf", "mqtt_timeout", &mqtt_timeout);
	GetProfileString("./mqttclient.conf", "mqtt_username", mqtt_client_username);
	GetProfileString("./mqttclient.conf", "mqtt_password", mqtt_client_password);

	GetProfileString("./mqttclient.conf", "mqtt_topic", mqtt_pub_client_topic);

	char testbuf[100]={0};
	int testtimeout=0;

	printf("%s\n", mqtt_server_address);
	printf("%s\n", mqtt_pub_client_id);
	printf("%s\n", mqtt_sub_client_id);
	printf("%d\n", mqtt_timeout);
	printf("%s\n", mqtt_client_username);
	printf("%s\n", mqtt_client_password);
	printf("%s\n", mqtt_pub_client_topic);
	


	// strcpy(mqtt_server_address,"172.16.254.211:1883");
	// strcpy(mqtt_pub_client_id,"aaabbbccc_pub");
	// strcpy(mqtt_sub_client_id,"aaabbbccc_sub");
	// strcpy(mqtt_client_username,"test_user");
	// strcpy(mqtt_client_password,"jim777");
	// strcpy(mqtt_pub_client_topic,"mytest");
	
	// mqtt_timeout = 1000;

	//全局变量初始化
	tDev4chCtl.devNum = 1;
	strcpy(tDev4chCtl.devName,"4chCtrl_1");
	tDev4chCtl.statusCh1 = 0;
	tDev4chCtl.statusCh2 = 1;
	tDev4chCtl.statusCh3 = 0;
	tDev4chCtl.statusCh4 = 0;

	strcpy(tDev4chCtl.mqttTopicName,"4chennelCtrl/devNum1/4chCtrl_1");//"ah/15/gw1/4chennelCtrl/devNum1/4chCtrl_1"
	
	//json数据格式化

	//创建json对象
	cJSON *json=cJSON_CreateObject();

	//在json对象上，添加数组
	cJSON *array=NULL;
	cJSON_AddItemToObject(json,"chStatus",array=cJSON_CreateArray());

	//在array数组上,添加对象
	cJSON *obj=NULL;
	cJSON_AddItemToArray(array,obj=cJSON_CreateObject());
	cJSON_AddItemToObject(obj,"ch1",cJSON_CreateString("off"));
	cJSON_AddItemToObject(obj,"ch2",cJSON_CreateString("on"));
	cJSON_AddStringToObject(obj,"ch3","off");
	cJSON_AddStringToObject(obj,"ch4","off");
	
	//json数据 转化字符串
	char *s=cJSON_Print(json);
	strcpy(tDev4chCtl.mqttData,s);
	printf("%s\n",s);
	tDev4chCtl.mqttDataLen = strlen(tDev4chCtl.mqttData);

	//json数据解析
	cJSON *rjson=cJSON_Parse(s);
	cJSON *json_arr_item=cJSON_GetObjectItem(rjson,"chStatus");
	cJSON *object = cJSON_GetArrayItem(json_arr_item,0);   //因为这个对象是个数组获取，且只有一个元素所以写下标为0获取

	/*下面就是可以重复使用cJSON_GetObjectItem来获取每个成员的值了*/
	cJSON *item = cJSON_GetObjectItem(object,"ch1");  //
	printf("ch1=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch2");  //
	printf("ch2=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch3");  //
	printf("ch3=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch4");  //
	printf("ch4=%s\n",item->valuestring);
	
	
	//csv文件读取

	// tDev_4channelCtl_Typedef *ptDev4chCtl = NULL;

	// printf("csv_4ch_num=%d\n", getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_4CH_CTRL));
	// tDevTypeNodeTotal.dev4chCtrlTotal = getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_4CH_CTRL);
	
	// tDevTypeNodeTotal.ptDev4ChCtl = (tDev_4channelCtl_Typedef*)calloc(tDevTypeNodeTotal.dev4chCtrlTotal, sizeof(tDev_4channelCtl_Typedef));

	// printf("csv_4ch_num=%d\n", tDevTypeNodeTotal.dev4chCtrlTotal);
	// printf("getConfigErr=%d\n", get4lCtrlConfig(DEV_CONFIG_CSV_FILE_NAME, tDevTypeNodeTotal.ptDev4ChCtl, tDevTypeNodeTotal.dev4chCtrlTotal));
	
	//初始化4路控制器的配置
	initDevConfig(DEV_TYPE_NAME_4CH_CTRL);

	tDev_4channelCtl_Typedef *tDev4chCtl = tDevTypeNodeTotal.ptDev4ChCtl;
	
	//打印配置结构体数据
	for(int num=0; num<tDevTypeNodeTotal.dev4chCtrlTotal; num++)
	{
		printf("dev%dNum=%d\n",num, (tDev4chCtl + num)->devNum);
		printf("dev%dName=%s\n",num, (tDev4chCtl + num)->devName);
		printf("dev%dId=%02x %02x %02x %02x\n",num, (tDev4chCtl + num)->devId[0],(tDev4chCtl + num)->devId[1],(tDev4chCtl + num)->devId[2],(tDev4chCtl + num)->devId[3]);
		printf("read%dCmd=%s\n",num, (tDev4chCtl + num)->readCmd);
		printf("write%dCmd=%s\n",num, (tDev4chCtl + num)->writeCmd);
		printf("\r\n");
	}
	

	//应用线程创建
	pthread_create(&threads[0], NULL, subClient, (void *)0);
	pthread_create(&threads[1], NULL, pubClient, (void *)1);
	pthread_create(&threads[2], NULL, tcpClient_r, (void *)2);
	pthread_create(&threads[3], NULL, tcpClient_w, (void *)3);

	pthread_exit(NULL);

	return 0;
}


