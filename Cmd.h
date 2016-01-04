#ifndef CMD_H_
#define CMD_H_
#include <string>
#include <sstream>
#include <regex>
#include "ConfigError.h"

typedef void(*cmdcall)(const std::string,std::string&,const std::string);
class Cmd
{
public:
	virtual ~Cmd();
	Cmd(const int linenum,const std::string cmd,const std::string arg);
	cmdcall exec;
	std::string arg;
	// 替换引用
	static void eval(std::string &str,const std::string old);
	// 删除头域
	static void cmd_del(const std::string arg,std::string& header,const std::string old);
	// 设置首行内容
	static void cmd_setfirstline(const std::string arg,std::string& header,const std::string old);



};

#endif /* CMD_H_ */
