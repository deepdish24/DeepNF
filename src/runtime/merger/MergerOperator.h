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
#include <fstream>

#include "nlohmann/json.hpp"

#include "packet.h"
#include "pcap.h"
#include "RuntimeNode.h"
#include "MergerInfo.h"
#include "ConflictItem.h"
#include "NF.h"
#include "Field.h"
#include "ActionTableHelper.h"

class MergerOperator {

    public:
        MergerOperator(std::string action_file_path);

        // runs the merger
        void run();
        void process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);


    private:
        struct NFPacket {
            public:
                struct packet *pkt;
                int runtime_id;
                NF nf;
        };

        FILE *logfile;
        pcap_t* dst_dev_handle;

        // contains information about virtual network interfaces, conflicting NF pairs, etc.
        MergerInfo* merger_info;

        // contains information about the NF action table
        ActionTableHelper* action_table_helper;

        // maps name of virtual interface to its corresponding RuntimeNode
        std::map<std::string, pcap_t*> src_dev_handle_map;

        // maps packet IDs with list of received packets with that ID
        std::map<int, std::vector<NFPacket*>*> packet_map;
        std::string cur_dev;

        /* FUNCTIONS FOR PERFORMING MERGER OPERATIONS */
        // sets up hardcoded MergerInfo object to do testing on
        MergerInfo* setup_dummy_info();

        // given the two packets and the corresponding ConflictItem, return a merged packet
        NFPacket* resolve_packet_conflict(NFPacket* major_p, NFPacket* minor_p, ConflictItem* conflict);



        /* HELPER FUNCTIONS FOR OPERATING ON PACKETS */
        void configure_device_read_handles(std::string packet_filter_expr);
        void configure_device_write_handle(std::string packet_filter_expr, std::string dev);
        static NFPacket* copy_nfpacket(NFPacket* nfpacket); // returns a pointer to a copy of input NFPacket


        /* HELPER FUNCTIONS THAT PRINT STUFF */
        void print_tcp_packet(struct tcphdr *tcph);
        void print_ip_header(struct ip *iph);
        void print_data(u_char* data, int size);
};


#endif //DEEPNF_MERGEROPERATOR_H
