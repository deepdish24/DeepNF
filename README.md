### DeepNF
Distributed NFV scheduling framework for senior design

### Building
1. Create build directory:
    => mkdir build && cd build
2. Create makefile. This only has to be done once for every time a change is made to CMakeLists:
    => (from within /build) cmake ../
3. Build the project
    => make

### Running the code
To run listener program:
    ./src/listener/listener

To run runtime program:
    ./src/runtime/runtime