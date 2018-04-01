//
// Created by Victoria on 2018-03-30.
//

#include "MergerInfo.h"



MergerInfo::MergerInfo(std::map<std::string, RuntimeNode*> interface_leaf_map,
                       std::vector<ConflictItem*> conflicts_list,
                       std::map<int, RuntimeNode*> service_graph) {
    this->interface_leaf_map = interface_leaf_map;
    this->conflicts_list = conflicts_list;
    this->node_map = node_map;
}

std::map<std::string, RuntimeNode*> MergerInfo::get_interface_leaf_map() {
    return interface_leaf_map;
};

std::vector<ConflictItem*> MergerInfo::get_conflicts_list() {
    return conflicts_list;
};

std::map<int, RuntimeNode*> MergerInfo::get_node_map() {
    return node_map;
}

/**
 * @return A deep copy of conflicts_list
 */
std::vector<ConflictItem*> MergerInfo::copy_conflicts_list() {
    std::vector<ConflictItem*> copy_list;

    for (std::vector<ConflictItem*>::iterator it = conflicts_list.begin(); it != conflicts_list.end(); ++it) {
        ConflictItem* ci = *it;

        // create a copy of ci
        ConflictItem* copy_ci = new ConflictItem(ci->get_major(),
                                                 ci->get_minor(),
                                                 ci->get_parent(),
                                                 ci->get_conflicts());
        copy_list.push_back(copy_ci);
    }

    return copy_list;

}
