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
        std::map<std::string, int> port_to_node_map;

        // list of conflicting NFs
        std::vector<ConflictItem*> conflicts_list;

        // maps node id to node
        std::map<int, RuntimeNode*> node_map;

    public:
        MergerInfo(std::map<std::string, int> port_to_node_map,
                   std::vector<ConflictItem*> conflicts_list,
                   std::map<int, RuntimeNode*> node_map);

        std::map<std::string, int> get_port_to_node_map();

        std::vector<ConflictItem*> get_conflicts_list();

        std::map<int, RuntimeNode*> get_node_map();

        /**
         *
         * @return A pointer to a MergerInfo object with dummy data for testing purposes
         */
        static MergerInfo* get_dummy_merger_info();

};


#endif //DEEPNF_MERGERINFO_H
