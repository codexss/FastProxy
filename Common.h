#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <fcntl.h>
// 删除字符串两边的空白
std::string& trim(std::string& text);
// 删除字符串左边空白
std::string& ltrim(std::string& text);
// 删除字符串右边空白
std::string& rtrim(std::string& text);

// 设置Socket非阻塞
int setnonblocking(int sock);
// 设置Socket阻塞
int setblocking(int sock);

#endif /* COMMON_H_ */
