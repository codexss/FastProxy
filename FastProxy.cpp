#include "FastProxy.h"

int main(int argc,char* argv[])
{
    if(getuid()!=0)
    {
     std::cout<<"请以ROOT权限运行!"<<std::endl;
     return 0;
    }
    std::ifstream input("/system/xbin/default.fp");
    if(!input)
        std::cout<<"无法打开配置文件/system/xbin/default.fp"<<std::endl;
    std::string conf  = "",tmp;
    while(!input.eof())
    {
        getline(input,tmp);
        conf+=tmp;
    }

	int port = 60888;
	Config *c = new Config();
	try{
		c->init(conf
			/*"设置 HTTP代理 192.168.1.1:80\n"
			"设置 HTTPS代理 183.111.169.201:3128\n"*/
			/*"模块 HTTP\n"
			"删除 Host\n"
			"删除 X-Online-Host\n"
			"设置首行 [M] [U] [V]\\r\\n"*/
			);
	}catch(ConfigError &e)
	{
		std::cout<<e;
		delete c;
	}
	Server svr;
	if(!svr.init(port,c))
	{
		std::cerr<<"监听端口"<<port<<"失败"<<std::endl;
	}else{
		if(fork()==0)
		svr.loop();
	}
	/*std::string m = "HTTP",
			h =
					"GET http://www.baidu.com/index.php HTTP/1.0\r\n"
					"Host: www.baidu.com\r\n"
					"Host: www.baidu.com\r\n"
					"Host2: www.sogou.com\r\n"
					;
	c->exec(m,h);
	std::cout<<std::endl<<std::endl<<h;
std::cin>>m;*/
	return 0;
}