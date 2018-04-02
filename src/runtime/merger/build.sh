g++ -std=c++11 -c RuntimeNode.h RuntimeNode.cpp
g++ -std=c++11 -c packet.h packet.cpp
g++ -std=c++11 packet.cpp RuntimeNode.cpp merge.cpp \
	-o merge -I /usr/include/pcap -lpcap
