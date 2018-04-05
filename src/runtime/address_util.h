#pragma once

#include "address.h"

struct address* address_from_string(std::string str);

std::string address_to_string(struct address *addr);

std::string stringify(std::string ip, int port);