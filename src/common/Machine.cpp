
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "Machine.h"

Machine::Machine(int id) {
	this->id = id;
}

Machine::~Machine() {

}

int Machine::get_id() { return id; }

std::string Machine::get_ip() { return ip; }

std::string Machine::get_bridge_ip() { return bridge_ip; }

std::vector<int> Machine::get_node_ids() { return nodes; }

void Machine::set_bridge_ip(std::string bridge_ip) { this->bridge_ip = bridge_ip; }

void Machine::set_ip(std::string ip) { this->ip = ip; }

void Machine::add_node_id(int n) {
	nodes.push_back(n);
}