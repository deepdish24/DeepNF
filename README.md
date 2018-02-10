### DeepNF
Distributed NFV scheduling framework for senior design

### Downloading dependencies
1. Install nlohmann's json library:
    => brew tap nlohmann/json; brew install nlohmann_json

### Building
1. Create build directory:
    => mkdir build && cd build

2. Create makefile. This only has to be done once for every time a change is made to CMakeLists:
    => cmake ../

If this creates the Makefile anywhere except for the current working directory (which should be
/build), run the following command instead:
    => cmake ../ -B .

3. Build protobuf:
    => make google_protobuf

4. Build the project
    => make

### Running the code (from /build)
To run listener program:
    ./src/listener/listener

To run runtime program:
    ./src/runtime/runtime

To run setup program:
    ./src/setup/setup