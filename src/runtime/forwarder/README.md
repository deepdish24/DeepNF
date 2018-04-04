###############################################################

INSTRUCTIONS FOR RUNNING FORWARDER

###############################################################

The forwarder program is located in forwarder.cpp. There is a sample
receiver in receiver.cpp to be used for testing purposes; it just
listens on a port number indicated by the first argument and prints
all received messages.

To run forwarder:

    1. Prepare a input.txt file that lists mappings from the real port
       number of a container to its virtual IP and portno. Each listing
       should be in the format:
            [real portno];[virtual IP]:[virtual portno]
       Refer to sample_input.txt for an example.

    2. Compile forwarder
            => make clean; make all

    3. Run forwarder
            => ./forwarder [path to input.txt file]
       Make sure all virtual ports are open and accepting TCP connections
       before sending any messages to the forwarder, or else the program
       will fail.


