#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "Machine.h"

Machine::Machine() {

}

Machine::~Machine() {

}

string Machine::get_id() { return id; }
string Machine::get_ip() { return ip; }
string Machine::get_bridge_ip() { return bridge_ip; }