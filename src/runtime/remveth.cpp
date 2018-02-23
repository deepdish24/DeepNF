#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>

int main() { 
	system(("ifconfig > a.txt"));
	std::ifstream mbox_file("a.txt");
	std::string line;
	if (mbox_file.is_open()) {
		int i;
		while(getline(mbox_file, line)) {
			if ((i = line.find("_")) != std::string::npos) {
				std::string v = line.substr(0, i+2); 
				if (v != "gre_s") {
					system(("sudo ip link delete " + v).c_str());
				} 
			} else if ((i = line.find("veth")) == 0) {
				int space_index = line.find(" ");
				std::string v = line.substr(0, space_index);
				system(("sudo ip link delete " + v).c_str());
			} 
		} 
	}        

	return 0;
}
