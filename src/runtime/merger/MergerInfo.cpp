//
// Created by Victoria on 2018-03-30.
//

#include "MergerInfo.h"

std::map<std::string, RuntimeNode*> MergerInfo::get_eth_to_runtime_map() {
    return eth_to_runtime_map;
};

std::vector<ConflictItem*> MergerInfo::get_conflicts_list() {
    return conflicts_list;
};

std::vector<RuntimeNode*> MergerInfo::get_service_graph() {
    return service_graph;
}
