#ifndef SERVER_H_
#define SERVER_H_
#include "Config.h"
#include "Common.h"
#include "SockInfo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
// forwardUp用到的传输类型
typedef char HeaderType;
#define HT_HTTP		0
#define HT_HTTPS	1
#define HT_OTHER	2
//enum HeaderType{HT_HTTP=0,HT_HTTPS=1，HT_OTHER=2};
class Server;
typedef struct{
	Server* server;
	SockInfo* info;
}ServerArg;
class Server
{
public:
	Server();
	~Server();
	bool init(int port,Config* config);
	int loop();
	void setDump(bool dump);//是否抓包
	bool getDump();
	static void* acceptClient(void* arg);
	static void* forwardUp(void* arg);
	static void* forwardDown(void* arg);
	static void* destorySock(void* arg);
private:
	int epfd;
	bool dump; // 是否抓包
	Config* config;
};

#endif // SERVER_H_
