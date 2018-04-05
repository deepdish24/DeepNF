#include "address_util.h"

struct address* address_from_string(std::string str)
{
	int colon_index = str.find(":");
	if (colon_index != std::string::npos) {
		if (colon_index == 0 || colon_index == str.length() - 1) {
			return NULL;
		}
		std::string ip = str.substr(0, colon_index);
		std::string port_str = str.substr(colon_index + 1);
		int port = std::stoi(port_str);
		struct address *addr = new struct address();
		addr->ip = ip;
		addr->port = port;
		return addr;
	}

	return NULL; 
}

std::string address_to_string(struct address *addr)
{
	return addr->ip + ":" + std::to_string(addr->port);
}

std::string stringify(std::string ip, int port)
{
	return ip + ":" + std::to_string(port);
}