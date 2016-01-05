#ifndef CONFIG_H_
#define CONFIG_H_
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "ConfigError.h"
#include "Common.h"
#include "ConfigChunk.h"
#include "ConfigValue.h"
#include "Cmd.h"

class Config {
public:
	Config();
	void init(std::ifstream& is);
	void exec(const std::string cn,std::string &header);
	virtual ~Config();
	std::string getValue(std::string key);
private:
	std::vector<ConfigChunk*> ccs; // Chunk列表
	std::vector<ConfigValue*> cvs; // Value列表
};

#endif /* CONFIG_H_ */
