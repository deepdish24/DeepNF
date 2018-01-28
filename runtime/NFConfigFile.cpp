#include <string>

using namespace std;

class NFConfigFile {

	// name of the file
	string name;
	// the file contents
	string contents;

public:
	
	NFConfigFile (string name, string contents) {
		this->name = name;
		this->contents = contents;
	}

	string get_name() { return name; }

	string get_contents() { return contents; }
};