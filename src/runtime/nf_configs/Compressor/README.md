###############################################################

INSTRUCTIONS FOR RUNNING PROXY

###############################################################


1. To compile compressor, run the following commands from the root DeepNF folder:
    => mkdir build; cd build; 
    => cmake ../; make google_protobuf; make

2. To start running proxy, run the following commands from the /build folder:
    => ./src/runtime/nf_configs/Compressor/compressor
        [bind port] [new_msg] [list of dest address]
    (Bind port is the port for proxy to listen for packets on.
     New msg is the "compressed message". Compressor will overwrite payload of any 
        received packets with this message.
     Dest addresses should be a space-delimited list of addresses in the format 
             "127.0.0.1:8000".)
     
