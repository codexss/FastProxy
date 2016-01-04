#ifndef CONFIGERROR_H_
#define CONFIGERROR_H_
#include <iostream>
#include <string>
class ConfigError{
public:
	ConfigError(int line,std::string msg);
	~ConfigError();
	friend std::ostream &operator<<(std::ostream &os,const ConfigError &e)
	{
		return os
				<<"在第"<<e.line<<"行发生错误"<<std::endl
				<<e.msg<<std::endl;
	}
private:
	int line;
	std::string msg;
};

#endif /* CONFIGERROR_H_ */
