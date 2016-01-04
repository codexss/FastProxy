#ifndef CONFIGVALUE_H_
#define CONFIGVALUE_H_
#include <string>
class ConfigValue
{
public:
	ConfigValue(std::string name,std::string value);
	virtual ~ConfigValue();
	std::string name;
	std::string value;
};

#endif /* CONFIGVALUE_H_ */
