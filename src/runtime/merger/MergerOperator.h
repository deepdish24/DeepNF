//
// Created by Victoria on 2018-03-31.
//

#ifndef DEEPNF_MERGEROPERATOR_H
#define DEEPNF_MERGEROPERATOR_H

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <vector>

#include "packet.h"
#include "pcap.h"
#include "RuntimeNode.h"
#include "MergerInfo.h"
#include "NF.h"

class MergerOperator {

    public:
        MergerOperator();

        // runs the merger
        void run();


    private:
        struct nf_packet {
            public:
                struct packet *pkt;
                NF nf;
        };

        std::map<std::string, RuntimeNode> interface_leaf_map;
        std::map<std::string, pcap_t*> src_dev_handle_map;
        std::map<int, std::vector<nf_packet>*> packet_map; // maps packet IDs with list of received packets with that ID
        std::string cur_dev;

        // sets up hardcoded MergerInfo object to do testing on
        MergerInfo* setup_dummy_info();



};


#endif //DEEPNF_MERGEROPERATOR_H
