#include "Server.h"


Server::Server()
{
}

Server::~Server()
{
}
void Server::setDump(bool dump)
{
	this->dump = dump;
}
bool Server::getDump()
{
	return this->dump;
}
bool Server::init(int port, Config* config)
{
	this->config = config;

	int svrfd = socket(AF_INET,SOCK_STREAM,0);
	if(svrfd==-1)
	{
        std::cout<<"打开Socket失败"<<std::endl;
		return false;
    }
	//绑定地址
	struct sockaddr_in svraddr;
	memset(&svraddr,0,sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svraddr.sin_port=htons(port);
	int opt = 1;
	setsockopt(svrfd, SOL_SOCKET,
			SO_REUSEADDR, (const void *) &opt, sizeof(opt));
	if(bind(svrfd,(sockaddr*)&svraddr,sizeof(svraddr))==-1)
	{
		close(svrfd);
		 std::cout<<"绑定端口"<<port<<"失败"<<std::endl;
		return false;
	}

	// 监听端口 最大队列20
	listen(svrfd,20);

	if(setnonblocking(svrfd)==-1)
	{
		close(svrfd);
		 std::cout<<"epoll非阻塞失败"<<std::endl;
		return false;
	}
	// 创建EPOLL监听
	int epfd = epoll_create(1024);
	if(epfd==-1)
	{
		close(svrfd);
		 std::cout<<"epoll创建失败"<<std::endl;
		return false;
	}
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = new SockInfo(svrfd,SockServer);
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,svrfd,&ev)==-1)
	{
		close(svrfd);
		close(epfd);
		 std::cout<<"epoll添加失败"<<std::endl;
		return false;
	}else{
		this->epfd = epfd;
	}

	return true;
}
int Server::loop()
{
	struct epoll_event evs[32];
	for(;;)
	{
		int evc = epoll_wait(this->epfd,evs,sizeof(evs)/sizeof(evs[0]),-1);
		if(evc==-1)
		{
			// 屏蔽高层打断
			if(errno==EAGAIN)
				continue;
			return -1;
		}
		int i;
		for(i=0;i<evc;++i)
		{
			SockInfo* info = ((SockInfo*)evs[i].data.ptr);
			// 判断触发事件的socket类型
			ServerArg* arg = (ServerArg*)malloc(sizeof(ServerArg));
			arg->server = this;
			arg->info = info;
			pthread_t pid; // 线程ID
			switch(info->getType())
			{
				case SockServer://如果是服务器accept
					//pthread_create(&pid, NULL, acceptClient, arg);
					acceptClient(arg);
					break;
				default:
					char c=0;
					if(recv(info->getFd(),&c,1,MSG_PEEK)!=1)
					{
						//pthread_create(&pid, NULL, destorySock, arg);
						destorySock(arg);
					}else{

						switch(info->getType())
						{
							case SockUp:

								//pthread_create(&pid, NULL, forwardUp, arg);
								forwardUp(arg);
								break;
							case SockDown:
								//pthread_create(&pid, NULL, forwardDown, arg);
								forwardDown(arg);
								break;
							default:
								break;
						}
					}
					break;
			}
			// 其他Socket可读
		}/* 事件i列表循环 */
	}/* epoll死循环 */

	return 0;
}

void* Server::acceptClient(void* arg)
{
	SockInfo *info = ((ServerArg*)arg)->info;
	Server *server = ((ServerArg*)arg)->server;
	free(arg);
	int fd = info->getFd();
	int client;
	struct sockaddr_in clientaddr;
	socklen_t clientlen=1;
	/* 这里必须初始化一下
	不然在安卓下会出错 */
	while((client=accept(fd,(sockaddr *)&clientaddr,&clientlen))!=-1)
	{
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.ptr = new SockInfo(client,SockUp);
		if(setnonblocking(client)==-1)
		{
			close(client);
			continue;
		}
		if(epoll_ctl(server->epfd,EPOLL_CTL_ADD,client,&ev)==-1)
		{
			close(client);
			continue;
		}
	}
	return NULL;
}

void* Server::forwardDown(void* arg)
{
	SockInfo *info = ((ServerArg*)arg)->info;
	//Server *server = ((ServerArg*)arg)->server;
	free(arg);
	if(info->getBorther()!=NULL)
	{
		int src = info->getFd();
		int dest = info->getBorther()->getFd();
		int len;
		char buf[1024*8];

		//setblocking(dest);
		while((len=recv(src,buf,sizeof(buf),0))>0)
		{
			//setblocking(dest);
			send(dest,buf,len,0);
			//setnonblocking(dest);
		}
		//setnonblocking(dest);
	}
	return NULL;
}

void* Server::forwardUp(void* arg)
{
	SockInfo *info = ((ServerArg*)arg)->info;
	Server *server = ((ServerArg*)arg)->server;
	free(arg);
	int src=info->getFd();
	char buf[1024*16];
	int len;
	while((len=recv(src,buf,sizeof(buf)-1,0))>0)
	{

		buf[len]=0;
		HeaderType ht = HT_OTHER;
		if(
				strncmp(buf,"GET",3)==0 ||
				strncmp(buf,"POST",4)==0 ||
				strncmp(buf,"HEAD",4)==0 ||
				strncmp(buf,"PUT",3)==0 ||
				strncmp(buf,"OPTIONS",7)==0 ||
				strncmp(buf,"DELETE",6)==0 ||
				strncmp(buf,"TRACE",5)==0
		)
			ht=HT_HTTP;
		else if(strncmp(buf,"CONNECT",7)==0)
			ht=HT_HTTPS;
		// 如果连接了目标
		if(info->getBorther()==NULL)
		{
			//没有连接目标
			std::string addr;
			if(ht==HT_HTTP)
				addr=server->config->getValue("HTTP代理");
			else
				// HTTPS连接
				addr=server->config->getValue("HTTPS代理");

			// 如果地址设置的地址不为空
			if(!addr.empty())
			{
				std::string ip =addr.substr(0,addr.find_first_of(":"));
				std::string port =addr.substr(addr.find_last_of(":")+1,addr.length());

				int nfd;
				if((nfd=socket(AF_INET,SOCK_STREAM,0))!=-1)
				{
					struct sockaddr_in dest;
					memset(&dest,0,sizeof(dest));
					dest.sin_family = AF_INET;
					dest.sin_port = htons(atoi(port.c_str()));
					dest.sin_addr.s_addr = inet_addr(ip.c_str());
					if(-1 != connect(nfd,(struct sockaddr*)&dest,sizeof(struct sockaddr)))
					{
						if(-1!=setnonblocking(nfd))
						{
							struct epoll_event ev;
							ev.events = EPOLLIN | EPOLLET;
							ev.data.ptr = new SockInfo(nfd,SockDown,info);
							if(epoll_ctl(server->epfd,EPOLL_CTL_ADD,nfd,&ev)!=-1)
							{
								//std::cout<<"服务器连接成功"<<std::endl;
							}else{
								close(nfd);
							}
						}else{
							close(nfd);
						}
					}else{
						close(nfd);
					}
				}
			}/* 设置地址不为空 */
		}/* 如果未连接 */
		if(info->getBorther()!=NULL)
		{
			if(ht!=HT_OTHER)
			{
				std::string header(buf);
				if(ht==HT_HTTP)
				{
					server->config->exec("HTTP",header);
					send(info->getBorther()->getFd(),header.data(),header.length(),0);
				}
				else
				{
					/*
					* 把\r\n\r\n之前的数据分离，避免修改时破坏加密数据
					*/
					header = header.substr(0,header.find("\r\n\r\n")+4);
					int byreadpos = header.length();
					server->config->exec("HTTPS",header);
					send(info->getBorther()->getFd(),header.data(),header.length(),0);
					// 发送剩余的数据
					if(byreadpos < len)
						send(info->getBorther()->getFd(),((char*)buf) + byreadpos,len-byreadpos,0);

				}
				// 输出header信息
				if(server->getDump())
					std::cout
					<<"=========================================="<<std::endl
					<<header<<std::endl;
			}
			else
			{
				// 这里是转发HTTPS加密的数据
				send(info->getBorther()->getFd(),buf,len,0);
			}

		}
	}/* 接收循环 */
	return NULL;
}
void* Server::destorySock(void* arg)
{
	SockInfo *info = ((ServerArg*)arg)->info;
	Server *server = ((ServerArg*)arg)->server;
	free(arg);
	if(info->getBorther()!=NULL)
	{
		info->getBorther()->setBorther(NULL);
		ServerArg* narg = (ServerArg*)malloc(sizeof(ServerArg));
			narg->server = server;
			narg->info = info->getBorther();
		server->destorySock(narg);
	}
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(server->epfd,EPOLL_CTL_DEL,info->getFd(),&ev);
	close(info->getFd());

	return NULL;
}