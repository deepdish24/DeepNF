###############################################################

INSTRUCTIONS FOR RUNNING PROXY

###############################################################


1. To compile proxy, run the following commands from the root DeepNF folder:
    => mkdir build; cd build; 
    => cmake ../; make google_protobuf; make

2. To start running firewall, run the following commands from the /build folder:
    => ./src/runtime/nf_configs/DnfFirewall/fw [bind port] [should_drop] [destIP:port destIP:port ....] (example: ./fw 800 173.16.1.4:800 173.16.1.5:800)  
    Set should_drop to 0 to not drop the packet, set should_drop to 1 to drop the packet