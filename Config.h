#ifndef CONFIG_H_
#define CONFIG_H_
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "ConfigError.h"
#include "Common.h"
#include "ConfigChunk.h"
#include "ConfigValue.h"
#include "Cmd.h"

class Config {
public:
	Config();
	void init(const std::string text);
	void exec(const std::string cn,std::string &header);
	virtual ~Config();
	std::string getValue(std::string key);
private:
	std::vector<ConfigChunk*> ccs; // Chunk列表
	std::vector<ConfigValue*> cvs; // Value列表
};

#endif /* CONFIG_H_ */
