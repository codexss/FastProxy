#include "SockInfo.h"

SockInfo::SockInfo(int fd, SockType type)
	:fd(fd),type(type)
{
	borther = NULL;
}

SockInfo::SockInfo(int fd, SockType type, SockInfo* borther)
	:fd(fd),type(type),borther(borther)
{
	borther->setBorther(this);
}

SockInfo* SockInfo::getBorther()
{
	return this->borther;
}

void SockInfo::setBorther(SockInfo* borther)
{
	this->borther = borther;
}

SockType SockInfo::getType()
{
	return this->type;
}

SockInfo::~SockInfo()
{
	if(this->borther!=NULL)
		this->borther->borther = NULL;
}
int SockInfo::getFd()
{
	return this->fd;
}
