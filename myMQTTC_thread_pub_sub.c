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

#include "ccl.h"

#define NUM_THREADS	    4
#define ADDRESS         "172.16.254.211:1883"           //更改此处地址
#define CLIENTID        "aaabbbccc_pub"                 //更改此处客户端ID
#define SUB_CLIENTID    "aaabbbccc_sub"                 //更改此处客户端ID
#define TOPIC           "mytest"                        //更改发送的话题
// #define PAYLOAD         "Hello Man, Can you see me ?!"  //
#define QOS             1
#define TIMEOUT         10000L
#define USERNAME        "test_user"
#define PASSWORD	    "jim777"
#define DISCONNECT	    "out"

#define true    1
#define false   0

int CONNECT = 1;
int PUB_FLAG = false;
char buf[1024] = {0};

int TCP_SEND_FLAG = false;
char s_buf[1024]={0};
int s_buf_send_len = 0;

char *payload=buf;
int payloadlen=0;


    //tcp 客户端连接服务器地址和端口
    char ip_buf[16]={0};

    char *server_ip_addr = ip_buf;
    int server_ip_port = 10086;     //默认值



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

    MQTTClient_create(&client, ADDRESS, SUB_CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
	conn_opts.password = PASSWORD;
	
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do 
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');

    MQTTClient_unsubscribe(client, TOPIC);
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
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
	conn_opts.password = PASSWORD;

	 //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // pubmsg.payload = payload;//PAYLOAD;
    // pubmsg.payloadlen = payloadlen; //strlen(PAYLOAD);
    // pubmsg.qos = QOS;
    // pubmsg.retained = 0;
	
    while(CONNECT)
    {
        if(PUB_FLAG)
        {
            pubmsg.payload = payload;//PAYLOAD;
            pubmsg.payloadlen = payloadlen; //strlen(PAYLOAD);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            
            PUB_FLAG = false;
            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
            printf("Waiting for up to %d seconds for publication of %s\n"
                    "on topic %s for client with ClientID: %s\n",
                    (int)(TIMEOUT/1000), payload, TOPIC, CLIENTID);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
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

// //发送
//     if((send(socket_fd, send_message, strlen(send_message), 0)) < 0) 
//     {
//         fprintf(stderr, "send message error: %s errno : %d", strerror(errno), errno);
//         // return 0;
//         exit(1);  
//     }

//     if((rec_len = recv(socket_fd, buf, sizeof(buf), 0)) == -1) 
//     {  
//        perror("recv error");  
//        exit(1);  
//     }
//     else
//     {
//         buf[rec_len]='\0';

//         payload = buf;
//         payloadlen = rec_len+1;
//         PUB_FLAG = true;
//         printf("%s\n",buf);
//     }
    

    while(1)
    {

        // rec_len=send(socket_fd,buf,strlen(buf),0);

        // if(TCP_SEND_FLAG)
        // {
        //     TCP_SEND_FLAG = false;
        //     rec_len=send(socket_fd, s_buf, s_buf_send_len, 0);
        //     printf("send_tcp_data_len=%d\n", rec_len);
        // }

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

    // char ip_buf[16]={0};

    // char *server_ip_addr = ip_buf;
    // int server_ip_port = 10086;

    // //读取配置文件，获取ip/port
    // GetProfileString("./tcpclient.conf", "tcpclient_ip", ip_buf);
    // GetProfileInt("./tcpclient.conf", "tcpclient_port", &server_ip_port);

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

// //发送
//     if((send(socket_fd, send_message, strlen(send_message), 0)) < 0) 
//     {
//         fprintf(stderr, "send message error: %s errno : %d", strerror(errno), errno);
//         // return 0;
//         exit(1);  
//     }

//     if((rec_len = recv(socket_fd, buf, sizeof(buf), 0)) == -1) 
//     {  
//        perror("recv error");  
//        exit(1);  
//     }
//     else
//     {
//         buf[rec_len]='\0';

//         payload = buf;
//         payloadlen = rec_len+1;
//         PUB_FLAG = true;
//         printf("%s\n",buf);
//     }
    

    while(1)
    {

        // rec_len=send(socket_fd,buf,strlen(buf),0);

        if(TCP_SEND_FLAG)
        {
            TCP_SEND_FLAG = false;
            rec_len=send(socket_fd, s_buf, s_buf_send_len, 0);
            printf("send_tcp_data_len=%d\n", rec_len);
        }

        // if((rec_len = recv(socket_fd, buf, sizeof(buf), 0)) == -1) 
        // {  
        //    perror("recv error");  
        //    exit(1);  
        // }
        // buf[rec_len]='\0';

        // payload = buf;
        // payloadlen = rec_len+1;
        // PUB_FLAG = true;
        
        // printf("%s\n",buf);
    }
    
    close(socket_fd);
    socket_fd = -1;
}

int main(int argc, char* argv[])
{
	pthread_t threads[NUM_THREADS];
    long t;

    //读取配置文件，获取ip/port
    GetProfileString("./tcpclient.conf", "tcpclient_ip", ip_buf);
    GetProfileInt("./tcpclient.conf", "tcpclient_port", &server_ip_port);

    pthread_create(&threads[0], NULL, subClient, (void *)0);
	pthread_create(&threads[1], NULL, pubClient, (void *)1);
    pthread_create(&threads[2], NULL, tcpClient_r, (void *)2);
    pthread_create(&threads[3], NULL, tcpClient_w, (void *)3);

    pthread_exit(NULL);

    return 0;
}