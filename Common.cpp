
#include "Common.h"
std::string& trim(std::string& text)
{
	return rtrim(ltrim(text));
}
std::string& ltrim(std::string& text)
{
	if(!text.empty())
	{
		text.erase(0, text.find_first_not_of(" \n\r\t"));
	}
	return text;
}
std::string& rtrim(std::string& text)
{
	if(!text.empty())
	{
		text.erase(text.find_last_not_of(" \n\r\t" )+ 1);
	}
	return text;
}

int setnonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        return -1;
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
		return -1;
    }
	return 0;
}
