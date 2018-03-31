g++ -std=c++11 -c NF.h RuntimeNode.h RuntimeNode.cpp
g++ -std=c++11 -c packet.h packet.cpp
g++ -std=c++11 -c ConflictItem.h ConflictItem.cpp
g++ -std=c++11 -c MergerInfo.h MergerInfo.cpp
g++ -std=c++11 MergerOperator.h MergerOperator.h

g++ -std=c++11 packet.cpp RuntimeNode.cpp ConflictItem.cpp MergerInfo.cpp MergerOperator.cpp merge.cpp -o merge -I /usr/include/pcap -lpcap