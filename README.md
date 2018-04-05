### DeepNF
Distributed NFV scheduling framework for senior design

### Downloading dependencies
1. Install nlohmann's json library:
    => brew tap nlohmann/json; brew install nlohmann_json

### Setting up instance
1. Install cmake:
    => wget https://cmake.org/files/v3.11/cmake-3.11.0.tar.gz
    => tar xvf cmake-3.11.0.tar.gz
    => cd cmake-3.11.0.tar.gz && ./bootstrap && make && sudo make install
    => $PATH=$PATH:/usr/local/bin/cmake (if cmake is not recognized by bash)
2. Install nolohmann/json
    => sudo apt install linuxbrew-wrapper
    => brew tap nlohmann/json
    => brew install nlohmann_json
    => include the following line in CMakeLists.txt (in modules that use json library)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/ubuntu/.linuxbrew/Cellar/nlohmann_json/3.1.2/include")

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


### Running scripts under runtime

To run src/runtime/nf_configs/DnfFirewall/dnf_firewall.cpp:
    ./fw portno destIP:port [destIP:port destIP:port ....] (example: ./fw 800 173.16.1.4:800 173.16.1.5:800)  

To run src/runtime/nf_configs/pktgen/pkt_sender.cpp:
    ./sender -n <num_packets> destIP:port [destIP:port destIP:port ....] (example: ./sender -n 100 173.16.1.3:800)

To run src/runtime/nf_configs/pktgen/pkt_receiver.cpp:
    ./receiver portno (example: ./receiver 800)
