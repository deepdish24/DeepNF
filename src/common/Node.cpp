#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "Node.h"

using namespace std;

void Node::set_image_name() {
	switch(nf) {
		case snort: image_name = name + "_snort";
		break;
		case haproxy: image_name = name + "_haproxy";
		break;
		default: break;
	}
};


Node::Node(string n, NF f) {
    name = n;
    nf = f;
    config_dir = "/home/ec2-user/" + name;

    set_image_name();
};

Node::~Node() {

};

string Node::get_name() { return name; };

NF Node::get_nf() { return nf; };

string Node::get_image_name() { return image_name; };

string Node::get_config_dir() { return config_dir; };

void Node::create_config_dir() {
    if (mkdir(config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == EEXIST) {
        system(("rm -r " + config_dir).c_str());
        mkdir(config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
};

//vector<string> Node::get_config_files() { return config_files; };
//
//void Node::add_config_file(string path) { config_files.push_back(path); };