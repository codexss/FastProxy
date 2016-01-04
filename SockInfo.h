#ifndef SOCKINFO_H_
#define SOCKINFO_H_
#include <stddef.h> // NULL
typedef enum {SockServer,SockUp,SockDown} SockType;
class SockInfo
{
public:
	SockInfo(int fd,SockType type);
	SockInfo(int fd,SockType type,SockInfo* borther);
	~SockInfo();
	int getFd();
	SockType getType();
	SockInfo* getBorther();
	void setBorther(SockInfo* borther);

private:
	int fd;
	SockType type;
	SockInfo* borther;
};
#endif
