#include "ConfigChunk.h"
#include <iostream>
ConfigChunk::ConfigChunk(std::string name)
	:name(name)
{

}

ConfigChunk::~ConfigChunk()
{
}

void ConfigChunk::add(Cmd* c)
{
	cmds.push_back(c);
}

void ConfigChunk::exec(std::string& header)
{
	std::string old=header;
	for(std::vector<Cmd*>::iterator it = cmds.begin();it!=cmds.end();it++)
	{
		(*(it))->exec((*(it))->arg,header,old);
	}
}
