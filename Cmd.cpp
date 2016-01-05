#include "Cmd.h"
#include <iostream>
Cmd::~Cmd()
{
}
void Cmd::eval(std::string& str, const std::string old)
{
	{
		// H引用转向Host
		const std::regex hp("\\[H\\]");
		str=std::regex_replace(str,hp,"[Host]");
	}
	{
		// \r \n \t
		const std::regex rp("\\\\r");
		str=std::regex_replace(str,rp,"\r");
		const std::regex np("\\\\n");
		str=std::regex_replace(str,np,"\n");
		const std::regex tp("\\\\t");
		str=std::regex_replace(str,tp,"\t");
	}
	
	const std::regex namep("(\\[[^\\]]+\\])");
	std::smatch namer;
	int regexn = 30;// 替换次数 防止造成死循环
	// 匹配原字符串中的引用
	while(std::regex_search(str, namer,namep) && --regexn >0)
	{
		if(namer.size()<=0)	break;
		// name是匹配到的引用名
		std::string name = namer[0].str();
		name=name.substr(1,name.length()-2);
		// 替换之后的引用
		std::string ns="";
		
		// 请求类型引用
		if(name.compare("M")==0)
		{
			std::istringstream is(old);
			is>>ns;
		}// URI引用
		else if(name.compare("U")==0)
		{
			std::istringstream is(old);
			is>>ns>>ns;
			const std::regex nsnp("(http|https)://[^/]*");
			ns = std::regex_replace(ns,nsnp,"");
		}// 协议版本引用
		else if(name.compare("V")==0)
		{
			std::istringstream is(old);
			is>>ns>>ns>>ns;
		}
		else // 从原始头域查找
		{
			const std::regex nsp(name+"\\s*:\\s*[^\\r\\n]*");
			std::smatch nsr;
			if(std::regex_search(old,nsr,nsp))
			{
				if(namer.size()>0)
				{
					ns = nsr.str();
					const std::regex nsnp(name+"\\s*:\\s*");
					ns = std::regex_replace(ns,nsnp,"");
				}
			}
		}
		
		const std::regex nsp("\\["+name+"\\]");
		str=std::regex_replace(str,nsp,ns);
	}
}
Cmd::Cmd(const int linenum,const std::string cmd,const std::string arg)
	:exec(NULL),arg(arg)
{
	if(cmd.compare("删除")==0)
	{
		if(!(arg.length()>0))
			throw ConfigError(linenum,"未指定删除对象");
		exec = &cmd_del;
	}else if(cmd.compare("设置首行")==0)
	{
		if(!(arg.length()>0))
			throw ConfigError(linenum,"未指定首行数据");
		exec = &cmd_setfirstline;
	}

}
void Cmd::cmd_del(const std::string arg, std::string& header,
		const std::string old)
{
	std::istringstream argis (arg);
	while(!argis.eof())
	{
		std::string name;
		argis>>name;
		const std::regex pattern(name+"\\s*:\\s*[^\\r\\n]*(\\r\\n|\\r|\\n)");
		header = std::regex_replace(header,pattern,"");
	}
}
void Cmd::cmd_setfirstline(const std::string arg, std::string& header,
		const std::string old)
{
	std::string ns = arg;
	eval(ns,old);
	
	const std::regex hp("^[^\\r\\n]*(\\r\\n|\\r|\\n)");
	header=std::regex_replace(header,hp,ns);
}

