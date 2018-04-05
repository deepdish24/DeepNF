//
// Created by Victoria on 2018-03-30.
//

#ifndef DEEPNF_MERGERINFO_H
#define DEEPNF_MERGERINFO_H

#include "RuntimeNode.h"
#include "ConflictItem.h"

#include <map>
#include <vector>

#include "RuntimeNode.h"
#include "ConflictItem.h"

class MergerInfo {


    private:
        // maps each network interface name to its corresponding runtime node
        std::map<std::string, RuntimeNode*> interface_leaf_map;

        // list of conflicting NFs
        std::vector<ConflictItem*> conflicts_list;

        // maps node id to node
        std::map<int, RuntimeNode*> node_map;

    public:
        MergerInfo(std::map<std::string, RuntimeNode*> interface_leaf_map,
                   std::vector<ConflictItem*> conflicts_list,
                   std::map<int, RuntimeNode*> node_map);

        std::map<std::string, RuntimeNode*> get_interface_leaf_map();

        std::vector<ConflictItem*> get_conflicts_list();

        std::map<int, RuntimeNode*> get_node_map();

        /**
         * @return A deep copy of conflicts_list
         */
        std::vector<ConflictItem*> copy_conflicts_list();

};


#endif //DEEPNF_MERGERINFO_H
