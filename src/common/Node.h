#pragma once

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "NF.h"

using namespace std;

class Node
{
    public:
        Node(string n, NF f);
        ~Node();

        void set_image_name();
        string get_name();
        NF get_nf();
        string get_image_name();
        string get_config_dir();
        void create_config_dir();
        vector<string> get_config_files();
        void add_config_file(string path);

    private:
        int id;
        // name of container
        string name;
        // network function running on the node
        NF nf;
        // name of docker image
        string image_name;
        // directory containing the rules and config files for the network function
        string config_dir;

        vector<int> in_edges; // list of node IDs representing in-neighbors of this node
        vector<int> out_edges; // list of node IDs representing out-neighbors of this node

 };