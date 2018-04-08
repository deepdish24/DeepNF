###############################################################

INSTRUCTIONS FOR RUNNING PROXY

###############################################################


1. To compile proxy, run the following commands from the root DeepNF folder:
    => mkdir build; cd build; 
    => cmake ../; make google_protobuf; make

2. To start running proxy, run the following commands from the /build folder:
    => ./src/runtime/nf_configs/Proxy/proxy 
        [bind port] [server ip] [server port] [dest ip] [dest port]
    (Bind port is the port for proxy to listen for packets on.
     Server ip and port refer to the address to redirect packets to by overwriting 
        packet header.
     Dest ip and port refer to address to send packets to after processing.)