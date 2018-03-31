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

        // list of all nodes in the service graph
        std::vector<RuntimeNode*> service_graph;

    public:
        MergerInfo(std::map<std::string, RuntimeNode*> interface_leaf_map,
                   std::vector<ConflictItem*> conflicts_list,
                   std::vector<RuntimeNode*> service_graph);

        std::map<std::string, RuntimeNode*> get_interface_leaf_map();

        std::vector<ConflictItem*> get_conflicts_list();

        std::vector<RuntimeNode*> get_service_graph();

};


#endif //DEEPNF_MERGERINFO_H
