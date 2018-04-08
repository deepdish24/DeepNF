###############################################################

INSTRUCTIONS FOR RUNNING MERGER

###############################################################


1. To compile merger, run the following commands from the root DeepNF folder:
    => mkdir build; cd build; 
    => cmake ../; make google_protobuf; make

2. To start running merger, run the following commands from the /build folder:
    => ./src/runtime/merger/merger
    (Note that the Merger depends on a MergerInfo object, which conveys information about
     which ports to listen to and what address to send merged packets. Currently, this is 
     hardcoded - see the get_dummy_merger_info() function. Feel free to change that function
     or add in functionality to dynamically pass in the MergerInfo.)

