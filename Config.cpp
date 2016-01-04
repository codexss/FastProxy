#include "Config.h"
Config::Config(){

}
void Config::init(const std::string text){
	std::istringstream is(text);
	std::string line;
	int linenum=0;
	ConfigChunk* cc=NULL;
	while(!is.eof())
	{
		linenum++;
		getline(is,line);
		// 去掉空白和注释
		line = trim(line);
		if(line.empty() || line.at(0)=='#') continue;
		/*
		 *  处理输入内容
		 */

		// 分离操作和参数(cmd和arg)
		std::string cmd,arg;
		std::istringstream lis(line);
		lis>>cmd;
		if(!lis.eof())
		{
			getline(lis,arg);
			arg=trim(arg);
		}

		// 处理操作
#define IF_CMD(x) if(cmd.compare(x)==0)
		IF_CMD("模块")
		{
			ccs.push_back(cc = new ConfigChunk(arg));
			continue;
		}
		// 设置变量
		IF_CMD("设置")
		{
			std::istringstream argis(arg);
			std::string varn,varv;// name value
			// 分离 name value
			if(!argis.eof())
			{
				argis>>varn;
				if(!argis.eof())
				{
					getline(argis,varv);
					varv=trim(varv);
				}
			}
			// 保存
			if(!varn.empty() && !varv.empty())
			{
				cvs.push_back(new ConfigValue(varn,varv));
			}
			continue;
		}
		// 判断是否已经有了Chunk
		if(cc==NULL)
		{
			throw ConfigError(linenum,"应该先定义模块再定义操作");
			continue;
		}

		Cmd *c = new Cmd(linenum,cmd,arg);
		if(c->exec!=NULL)
		{
			cc->add(c);
		}else{
			delete c;
		}
	}
}

void Config::exec(const std::string cn, std::string& header)
{
	// 查找Chunk
	for(std::vector<ConfigChunk*>::iterator it = ccs.begin();it!=ccs.end();it++)
	{
		if((*(it))->name.compare(cn)==0)
		{
			(*(it))->exec(header);
			break; // 只执行第一个匹配的
		}
	}
}

Config::~Config() {
	// 清理Chunk内存
	for(std::vector<ConfigChunk*>::iterator it = ccs.begin();it!=ccs.end();)
	{
		delete *(it);
		ccs.erase(it);
	}
	// 清理Value
	for(std::vector<ConfigValue*>::iterator it = cvs.begin();it!=cvs.end();)
	{
		delete *(it);
		cvs.erase(it);
	}
}
std::string Config::getValue(std::string key)
{
	for(std::vector<ConfigValue*>::iterator it = cvs.begin();it!=cvs.end();it++)
	{
		if((*it)->name.compare(key)==0)
			return (*it)->value;
	}
	return "";
}
