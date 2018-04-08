//
// Created by Victoria on 2018-03-30.
//

#include "MergerInfo.h"

#include <utility>


MergerInfo::MergerInfo(std::map<int, int> port_to_node_map,
                       std::vector<ConflictItem*> conflicts_list,
                       std::map<int, RuntimeNode*> node_map,
                       std::string dest_ip,
                       int dest_port) {
    this->port_to_node_map = std::move(port_to_node_map);
    this->conflicts_list = std::move(conflicts_list);
    this->node_map = std::move(node_map);
    this->dest_ip = std::move(dest_ip);
    this->dest_port = dest_port;
}

std::map<int, int> MergerInfo::get_port_to_node_map() {
    return port_to_node_map;
};

std::vector<ConflictItem*> MergerInfo::get_conflicts_list() {
    return conflicts_list;
};

std::map<int, RuntimeNode*> MergerInfo::get_node_map() {
    return node_map;
}

std::string MergerInfo::get_dest_ip() {
    return dest_ip;
}

int MergerInfo::get_dest_port() {
    return dest_port;
}


/**
 *
 * @return A pointer to a MergerInfo object with dummy data for testing purposes
 */
MergerInfo* MergerInfo::get_dummy_merger_info() {
    std::map<int, int> port_to_node_map;
    port_to_node_map.insert(std::make_pair(8000, 0));
    port_to_node_map.insert(std::make_pair(8001, 1));

    std::vector<ConflictItem*> conflicts_list;

    std::map<int, RuntimeNode*> node_map;
    RuntimeNode* rn1 = new RuntimeNode(0, compressor);
    RuntimeNode* rn2 = new RuntimeNode(1, proxy);
    node_map.insert(std::make_pair(0, rn1));
    node_map.insert(std::make_pair(1, rn2));

    std::string dest_ip = "127.0.0.1";
    int dest_port = 9999;

    MergerInfo* mi = new MergerInfo(port_to_node_map, conflicts_list, node_map, dest_ip, dest_port);
    return mi;
}
