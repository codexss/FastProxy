#include "ConfigError.h"

ConfigError::ConfigError(int line,std::string msg)
	:line(line),msg(msg)
{

}

ConfigError::~ConfigError()
{
}

