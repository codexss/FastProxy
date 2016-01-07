#include "FastProxy.h"
#define VERS "FastProxy 0.1"
void version(int argc,char* argv[])
{
	std::cout
		<<std::endl
		<<VERS<<" Code By Tsln"<<std::endl
		<<"\t-p\tPORT\t设置监听端口"<<std::endl
		<<"\t-u\tUID\t设置运行UID"<<std::endl
		<<"\t-f\tFILE\t设置配置文件名"<<std::endl
		<<"\t-d\t    \t抓包模式"<<std::endl
		<<std::endl
		<<"希望有更多的人能跟我一起开发^_^"<<std::endl
		<<std::endl
	;
}

int main(int argc, char* argv[])
{

	std::string confname = "conf.fp", // 配置名
			struid,	// UID
			strport; // 端口
	bool dump = false;
	int argi;
	for(argi=1;argi<argc;argi++)
		if(argv[argi][0]=='-')
		{
			switch(argv[argi][1])
			{
			// 自定义配置
			case 'f':
			case 'F':
				if(argi+1<argc)
					confname = argv[++argi];
				break;
			// 自定义端口
			case 'p':
			case 'P':
				if(argi+1<argc)
					strport = argv[++argi];
				break;
			// 自定义UID
			case 'u':
			case 'U':
				if(argi+1<argc)
					struid = argv[++argi];
				break;
			// 开启抓包
			case 'd':
			case 'D':
				dump=true;
				break;
			default:
				version(argc,argv);
				return 0;
			}
		}
    // 支持绝对路径
		std::string confpath;
    if(confname[0]!='/')
    {
        char _abspath[128];
        realpath(argv[0], _abspath);
        confpath = _abspath;
        confpath = confpath.substr(0, confpath.find_last_of("/")) + "/" + confname;
    }else{
        confpath = confname;
    }

	std::ifstream input(confpath);
	if (!input)
	{
		std::cout << "无法打开配置文件" << confpath << std::endl;
		return 0;
	}
	int port = 60888;
	Config *c = new Config();
	try
	{
		c->init(input);
		// 端口控制
		if (strport.empty())
			strport = c->getValue("PORT");
		if (strport.empty())
			strport = c->getValue("端口");
		if (!strport.empty())
		{

			int nport = atoi(strport.c_str());
			if (nport > 0 && nport <= 0xFFFF)
			{
				port = nport;
			}
		}

	} catch (ConfigError &e)
	{
		std::cout << e;
		delete c;
	}

	// 关闭输入文件
	if (!input)
		input.close();

	Server svr;
	if (svr.init(port, c))
	{
		svr.setDump(dump);
		// UID控制
		if (struid.empty())
			std::string struid = c->getValue("UID");
		if (!struid.empty())
		{
			int uid;
			if (struid.compare("root") == 0)
			{
				uid = 0;
				struid += "(0)";
			}
			else if (struid.compare("net_raw") == 0)
			{
				uid = 3004;
				struid += "(3004)";
			}
			else
			{
				uid = atoi(struid.c_str());
			}
			// 如果当前UID不匹配
			if (uid != getuid())
			{
				if (setuid(uid) == 0)
				{
					std::cout << "切换到UID:" << struid << "成功" << std::endl;
				}
				else
				{
					std::cout << "切换到UID:" << struid << "失败，可能是权限不足"
							<< std::endl;
					return 0;
				}
			}
		}
		std::cout << "服务在" << port << "端口后台运行" << std::endl;
		if(!dump)
		{
			if (fork() == 0)
			{
				svr.loop();
				return 0;
			}
		}else{
			svr.loop();
		}

	}
	return 0;
}
