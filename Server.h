#ifndef SERVER_H_
#define SERVER_H_
#include "Config.h"
#include "Common.h"
#include "SockInfo.h"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
class Server;
/*
class ServerArg
{
public:
	ServerArg(Server* server,SockInfo* info):server(server),info(info){};
	Server* server;
	SockInfo* info;
};*/
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
	static void* acceptClient(void* arg);
	static void* forwardUp(void* arg);
	static void* forwardDown(void* arg);
	static void* destorySock(void* arg);
private:
	int epfd;
	Config* config;
};

#endif // SERVER_H_
