#ifndef CONFIGCHUNK_H_
#define CONFIGCHUNK_H_
#include <string>
#include <vector>
#include "Cmd.h"
class ConfigChunk
{
public:
	ConfigChunk(std::string name);
	virtual ~ConfigChunk();
	void add(Cmd *c);
	void exec(std::string &header);
	std::string name;
private:
	std::vector<Cmd*> cmds;
};

#endif /* CONFIGCHUNK_H_ */
