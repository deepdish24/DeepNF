#include <string>
#include <set>

#ifndef SERVICEGRAPHNODE_H
#define SERVICEGRAPHNODE_H

class ServiceGraphNode {
public:
    // network function running on the node
    std::string nf;
    // set of pointers to node neighbors
    std::set<ServiceGraphNode*> neighbors = {};
    // parent of node
    ServiceGraphNode *parent;
    
    ServiceGraphNode(std::string f);
    void add_neighbor(ServiceGraphNode n);
    void add_parent(ServiceGraphNode n);
    bool isLeaf();
};

#endif