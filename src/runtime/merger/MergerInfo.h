//
// Created by Victoria on 2018-03-30.
//

#ifndef DEEPNF_MERGERINFO_H
#define DEEPNF_MERGERINFO_H

#include <map>
#include <vector>

#include "../../common/RuntimeNode.h"
#include "ConflictItem.h"

class MergerInfo {


    private:
        // maps each port to its corresponding runtime node id
        std::map<int, int> port_to_node_map;

        // list of conflicting NFs
        std::vector<ConflictItem*> conflicts_list;

        // maps node id to node
        std::map<int, RuntimeNode*> node_map;

        std::string dest_ip; // ip address to send merged packets to
        int dest_port; // port to send merged packets to

    public:
        MergerInfo(std::map<int, int> port_to_node_map,
                   std::vector<ConflictItem*> conflicts_list,
                   std::map<int, RuntimeNode*> node_map,
                   std::string dest_ip,
                   int dest_port);

        std::map<int, int> get_port_to_node_map();

        std::vector<ConflictItem*> get_conflicts_list();

        std::map<int, RuntimeNode*> get_node_map();

        std::string get_dest_ip();

        int get_dest_port();

        /**
         *
         * @return A pointer to a MergerInfo object with dummy data for testing purposes
         */
        static MergerInfo* get_dummy_merger_info();

};


#endif //DEEPNF_MERGERINFO_H
