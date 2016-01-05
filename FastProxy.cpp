#include "FastProxy.h"

int main(int argc,char* argv[])
{
    #ifdef __arm
    std::string confpath = "/system/xbin/default.fp";
    #else
    std::string confpath = "./default.fp";
    #endif // __arm

    std::ifstream input(confpath);
    if(!input)
    {
        std::cout<<"无法打开配置文件"<<confpath<<std::endl;
        return 0;
    }/*
    std::string conf  = "",tmp;
    while(!input.eof())
    {
        getline(input,tmp);
        conf+=tmp+"\n";
    }
*/
	int port = 60888;
	Config *c = new Config();
	try{
		c->init(input);
        // 端口控制
        std::string strport = c->getValue("PORT");
        if(strport.empty())
        {
            strport = c->getValue("端口");
        }
        if(!strport.empty())
        {

            int nport = atoi(strport.c_str());
            if(nport > 0 && nport <= 0xFFFF)
            {
                port = nport;
            }
        }
        // UID控制
        std::string struid = c->getValue("UID");
        if(!struid.empty())
        {
            int uid;
            if(struid.compare("root")==0)
            {
                uid=0;
                struid += "(0)";
            }else if(struid.compare("net_raw")==0)
            {
                uid=3004;
                struid += "(3004)";
            }else{
                uid = atoi(struid.c_str());
            }
            // 如果当前UID不匹配
            if(uid != getuid())
            {
                if(setuid(uid)==0)
                {
                    std::cout<<"切换到UID:"<<struid<<"成功"<<std::endl;
                }else{
                    std::cout<<"切换到UID:"<<struid<<"失败，可能是权限不足"<<std::endl;
                    return 0;
                }
            }
        }
	}catch(ConfigError &e)
	{
		std::cout<<e;
		delete c;
	}

	// 关闭输入文件
	if(!input)
        input.close();

	Server svr;
	if(svr.init(port,c))
	{
		if(fork()==0)
            svr.loop();
        else
            std::cout<<"服务在"<<port<<"端口后台运行"<<std::endl;
	}
	return 0;
}
