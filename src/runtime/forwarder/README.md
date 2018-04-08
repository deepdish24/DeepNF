###############################################################

INSTRUCTIONS FOR RUNNING FORWARDER

###############################################################

The forwarder program is located in forwarder.cpp. There is a sample
program in tester.cpp to be used for testing purposes; it just
listens on a port number indicated by the first argument and prints
all received messages.

To run forwarder on local machine:

    1. Prepare a input.txt file that lists mappings from the real port
       number of a container to its virtual IP and portno. Each listing
       should be in the format:
            [real portno];[virtual IP]:[virtual portno]
       Refer to sample_input.txt for an example.

    2. Compile forwarder
            => mkdir build; cd build; 
            => cmake ../; make google_protobuf; make
            
    3. Run forwarder from /build
            => ./src/runtime/forwarder/forwarder [path to input.txt file relative to /build]
            (ie. ./src/runtime/forwarder/forwarder  ../src/runtime/forwarder/sample_input.txt)
            
       Make sure all virtual ports are open and accepting TCP connections
       before sending any messages to the forwarder, or else the program
       will fail.


To create a new container running forwarder (assuming docker is installed):
    
    1. From the root directory of DeepNF, run:
            => docker build -t=forwarder -f src/runtime/forwarder/Dockerfile .
            (Be warned that this takes an extremely long time.)
            => docker run -d -t -i --name forwarder forwarder:latest /bin/bash
        
    2. Enter the containers and follow instructions above to run forwarder inside container
        
To run tester:
    1. Compile tester
        => mkdir build; cd build; 
        => cmake ../; make google_protobuf; make
        
    2. To run packet receiver from /build
        => ./src/runtime/forwarder/tester 1 [port to listen from]
        
    3. To run packet sender from /build
        => ./src/runtime/forwarder/tester 0 [ip to send to] [port to send to] [msg to send] [packet id]
        
    4. To run null packet sender from /build
        => ./src/runtime/forwarder/null_tester 0 [ip to send to] [port to send to] [packet id]