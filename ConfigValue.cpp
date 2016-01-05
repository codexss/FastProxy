#include "ConfigValue.h"

ConfigValue::ConfigValue(std::string name,std::string value)
	:name(name),value(value)
{
    // 转换为大写
    int i;
    for(i=0;i<this->name.size();i++)
        if(islower(this->name[i]))
            this->name[i] = toupper(this->name[i]);
}

ConfigValue::~ConfigValue()
{
}

