//
// Created by Victoria on 2018-04-04.
//

#include "MergerOperator.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <string>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

typedef struct node_thread_params {
    MergerOperator* inst; // the MergerOperator instance to operate on
    int port; // port that the leaf node will send packets to
    int node_id; // the id of the runtime node for this thread
} NODE_THREAD_PARAMS;


MergerOperator::MergerOperator() {
    this->action_table = new ActionTable();
    this->merger_info = MergerInfo::get_dummy_merger_info();
    this->num_nodes = (int) this->merger_info->get_port_to_node_map().size();
    std::vector<ConflictItem*> conflicts_list = this->merger_info->get_conflicts_list();

    // set up mutexes
    packet_map_mutex = PTHREAD_MUTEX_INITIALIZER;

    // set up log
    log.open("log/merger_log.txt", std::ios::out);
    if (!log) std::cerr << "Could not open the file!" << std::endl;
    log_mutex = PTHREAD_MUTEX_INITIALIZER;
}

MergerOperator::MergerOperator(MergerInfo* mi) {
    this->action_table = new ActionTable();
    this->merger_info = mi;
    this->num_nodes = (int) this->merger_info->get_port_to_node_map().size();
    std::vector<ConflictItem*> conflicts_list = this->merger_info->get_conflicts_list();

    // set up mutexes
    packet_map_mutex = PTHREAD_MUTEX_INITIALIZER;

    // set up log
    log.open("log/merger_log.txt", std::ios::out);
    if (!log) std::cerr << "Could not open the file!" << std::endl;
    log_mutex = PTHREAD_MUTEX_INITIALIZER;
}


typedef struct merge_thread_params {
    MergerOperator* inst; // the MergerOperator instance to operate on
    int packet_id; // the packet_id of the packets to merge
} MERGE_THREAD_PARAMS;


void* MergerOperator::merge_packet_wrapper(void *arg) {
    auto *tp = (MERGE_THREAD_PARAMS*) arg;
    MergerOperator *this_mo = tp->inst;
    this_mo->run_merge_packet(tp->packet_id);

    return nullptr;
}


void* MergerOperator::run_node_thread_wrapper(void *arg) {
    auto *tp = (NODE_THREAD_PARAMS*) arg;
    MergerOperator *this_mo = tp->inst;
    this_mo->run_node_thread(tp->port, tp->node_id);

    return nullptr;
}


/**
 * Listens on the given port for packets from the given runtime_id, merging as necessary
 *
 * @param port          The port to listen for packets from runtime_id
 * @param node_id    The id of the runtime node leaf
 */
void MergerOperator::run_node_thread(int port, int node_id) {
    // opens a datagram socket and returns the fd or -1 */
    int sockfd = open_socket();
    if (sockfd < 0) {
        fprintf(stderr, "Cannot open socket: %s", strerror(errno));
        exit(-1);
    }
    printf("opened socket on port: %d\n", port);

    // binds socket with given fd to given port */
    bind_socket(sockfd, port);

    std::map<int, packet *>* this_pkt_map;
    while (true) {
        // listen for packets
        printf("\nlistening for data...\n");
        sockdata *pkt_data = receive_data(sockfd);
        packet* p = packet_from_data(pkt_data);

        p->print_info();
        char sourceIp[INET_ADDRSTRLEN];
	    inet_ntop(AF_INET, &(p->ip_header->ip_src), sourceIp, INET_ADDRSTRLEN);

        // add packet to packet_map
        pthread_mutex_lock(&packet_map_mutex);
        if (packet_map.count(p->ip_header->ip_id) == 0) {
            std::map<int, packet *>* new_node_map = new std::map<int, packet *>();
            packet_map.insert(std::make_pair(p->ip_header->ip_id, new_node_map));
        }
        this_pkt_map = packet_map.at(p->ip_header->ip_id);
        this_pkt_map->insert(std::make_pair(node_id, p));
        printf("Printing this_node_map\n");

        packet_map.insert(std::make_pair(p->ip_header->ip_id, this_pkt_map));
        pthread_mutex_unlock(&packet_map_mutex);

        print_packet_map();

        // if all packets have been received for this packet_id, begin merging process
        if ((int) this_pkt_map->size() == num_nodes) {
            pthread_t merge_thread;
            auto * tp = (MERGE_THREAD_PARAMS*) malloc(sizeof(MERGE_THREAD_PARAMS));
            tp->inst = this;
            tp->packet_id = p->ip_header->ip_id;
            pthread_create(&merge_thread, nullptr, MergerOperator::merge_packet_wrapper, tp);
            pthread_detach(merge_thread);
        }
    }
}

/**
 * Given two packets (one with precedence over the another), return a merged packet that merges all written fields
 * in the two packets, resolving write conflicts appriopriately
 *
 * @param major_p       Info corresponding to the packet with precedence
 * @param minor_p       Info corresponding to the packet without precedence
 * @param conflict      The ConflictItem describing the conflict between the major and minor packets
 * @return  A merged packet containing both major_p and minor_p's writes
 */
MergerOperator::PACKET_INFO* MergerOperator::resolve_packet_conflict(
        PACKET_INFO* major_p,
        PACKET_INFO* minor_p,
        ConflictItem* conflict) {
    printf("MergerOperator::resolve_packet_conflict - major: %d, minor: %d\n", major_p->node_id, minor_p->node_id);

    auto* pi = (PACKET_INFO*) malloc(sizeof(PACKET_INFO));
    pi->node_id = conflict == nullptr ? -1 : conflict->get_parent();

    // create new packet based on changes in major packet
    pi->pkt = new packet(major_p->pkt->pkt, major_p->pkt->size);
    pi->written_fields = new std::set<Field>;

    // if either packet is null, drop the packet (ie. return a nullified packet)
    if (major_p->pkt->is_null() || minor_p->pkt->is_null()) {
        printf("Null packet detected\n");
        pi->pkt->nullify();
        return pi;
    }

    // add writes from minor packet
    std::set<Field>* major_fields = major_p->written_fields;
    std::set<Field>* minor_fields = minor_p->written_fields;

    for (std::set<Field>::iterator it = minor_fields->begin(); it != minor_fields->end(); ++it) {
        Field field = *it;

        // write the minor's field changes as long as the change does NOT conflict with major
        if (major_fields->count(field) == 0) {
            switch (field) {

                case Field::DIP:
                    pi->pkt->write_dest_ip(minor_p->pkt->get_dest_ip());
                    break;

                case Field::DPORT:
                    pi->pkt->write_dest_port(minor_p->pkt->get_dest_port());
                    break;

                case Field::PAYLOAD:
                    pi->pkt->write_payload(minor_p->pkt->get_payload());
                    break;

                default:
                    break;
            }
        }
    }

    // add major and minor fields to pi's written_fields
    pi->written_fields->insert(minor_fields->begin(), minor_fields->end());
    pi->written_fields->insert(major_fields->begin(), major_fields->end());

    printf("Returning merged packet\n");

    return pi;
}


/**
 * Retrieves all packets for the given pkt_id stored in packet_map and outputs a merged packet
 * based on the conflict items in merger_info
 *
 * @param pkt_id    The id of the packet to merge
 * @return Pointer to a packet with all changes merged
 */
packet* MergerOperator::merge_packet(int pkt_id) {
    printf("MergerOperator::merge_packet\n");
    std::map<int, packet*>* this_pkt_map = packet_map.at(pkt_id);

    // convert this_pkt_map to a map from node ids to packet_infos
    std::map<int, MergerOperator::PACKET_INFO*>* pkt_info_map = packet_map_to_packet_info_map(this_pkt_map);

    printf("Printing pkt_info_map:\n");
    for (auto it = pkt_info_map->begin(); it != pkt_info_map->end(); ++it) {
        printf("%d -> {", it->first);
        for (auto it2 = it->second->written_fields->begin(); it2 != it->second->written_fields->end(); ++it2) {
            printf("%s, ", field::field_to_string(*it2).c_str());
        }
        printf("}\n");
    }
    printf("\n");

    if ((int) this_pkt_map->size() != num_nodes) {
        fprintf(stderr, "Called merge_packet on an invalid pkt_id\n");
    }
    std::vector<ConflictItem*> conflicts_list = merger_info->get_conflicts_list();

    bool was_changed = true; // has at least one merge conflict been resolved in this iteration?

    while (was_changed) {
        was_changed = false;
        for (auto it = conflicts_list.begin(); it != conflicts_list.end(); ++it) {
            ConflictItem *ci = *it;

            // begin merging if both packets of the conflict conflict are available
            if (pkt_info_map->count(ci->get_major()) != 0 && pkt_info_map->count(ci->get_minor()) != 0) {
                PACKET_INFO *new_packet = resolve_packet_conflict(
                        pkt_info_map->at(ci->get_major()), pkt_info_map->at(ci->get_minor()), ci);

                // add merged packet's NF's written fields
                if (this->merger_info->get_node_map().count(new_packet->node_id) > 0) {
                    RuntimeNode *rn = this->merger_info->get_node_map().at(new_packet->node_id);
                    std::set<Field> new_write_fields = this->action_table->get_write_fields(rn->get_nf());

                    for (auto it = new_write_fields.begin(); it != new_write_fields.end(); ++it) {
                        new_packet->written_fields->insert(*it);
                    }
                }

                // remove major and minor, then add merged packet to pkt_map
                pkt_info_map->erase(ci->get_major());
                pkt_info_map->erase(ci->get_minor());
                pkt_info_map->insert(std::make_pair(ci->get_parent(), new_packet));

                was_changed = true;
            }
        }
    }

    // at this point, none of the packets should have conflicts any more, just merge them all
    PACKET_INFO* merged_packet = nullptr;
    for (auto it = pkt_info_map->begin(); it != pkt_info_map->end(); ++it) {
        printf("Iterating through pkt_info_map, %d\n", it->first);

        if (merged_packet == nullptr) {
            merged_packet = it->second;
            continue;
        }

        // randomly select major and minor since there should be no conflicts
        merged_packet = resolve_packet_conflict(merged_packet, it->second, (ConflictItem*) nullptr);

        printf("Merged packet:\n");
        merged_packet->pkt->print_info();

    }

    if (merged_packet == nullptr) {
        fprintf(stderr, "No packets in packet_map to merge for id %d", pkt_id);
        exit(-1);
    }

    // remove pkt_id from packet_map
    pthread_mutex_lock(&packet_map_mutex);
    packet_map.erase(pkt_id);
    pthread_mutex_unlock(&packet_map_mutex);

    return merged_packet->pkt;
}


/**
 * Merges all received packets for the given pkt_id, then sends merged packet to receive address
 *
 * @param pkt_id    The id of the packet to merge
 */
void MergerOperator::run_merge_packet(int pkt_id) {
    packet* merged_pkt = this->merge_packet(pkt_id);

    printf("\nFinished merging packet\n");

    // send merged packet to destination address
    int sockfd = open_socket();
    if (sockfd < 0) {
        fprintf(stderr, "Cannot open socket: %s", strerror(errno));
        exit(-1);
    }

    if (send_packet(merged_pkt, sockfd, this->dest_address) < 0) {
        fprintf(stderr, "Send packet error: %s", strerror(errno));
        exit(-1);
    }

    pthread_mutex_lock(&log_mutex);
    log_util::log_nf(log, merged_pkt, "merger", "Finished merging packet");
    pthread_mutex_unlock(&log_mutex);

    printf("Sent merged packet for id: %d, printing packet_map:\n\n", pkt_id);
    print_packet_map();

}


/**
 * Converts a map from node ids to packets to a map from node ids to equivalent packet_infos
 * @return A map of runtime node ids to a packet_info struct equivalent to the node's
 *         packet in packet_map
 */
std::map<int, MergerOperator::PACKET_INFO*>* MergerOperator::packet_map_to_packet_info_map(std::map<int, packet*>* packet_map) {

    auto ret_map = new std::map<int, MergerOperator::PACKET_INFO*>();

    for (auto it = packet_map->begin(); it != packet_map->end(); ++it) {
        int node_id = it->first;
        packet* pkt = it->second;
        printf("Iterating through packet_map_to_packet_info_map: id: %d, pkt: %s\n", node_id, pkt->get_payload().c_str());

        ret_map->insert(std::make_pair(node_id, packet_to_packet_info(pkt, node_id)));
    }

    return ret_map;
};


/**
 * Encapsulates the given packet into a packet_info struct
 *
 * @param packet    The packet to encapsulate
 * @param node_id   The id of the runtime node that send the input packet
 * @return Packet_info struct encapsulating the input packet instance
 */
MergerOperator::PACKET_INFO* MergerOperator::packet_to_packet_info(packet* pkt, int node_id) {
    auto* pi = (PACKET_INFO*) malloc(sizeof(PACKET_INFO));

    if (this->merger_info->get_node_map().count(node_id) == 0) {
        fprintf(stderr, "Called packet_to_packet_info on invalid node_id: %d\n", node_id);
        exit(-1);
    }
    RuntimeNode* rn = this->merger_info->get_node_map().at(node_id);

    pi->pkt = pkt;
    pi->node_id = node_id;
    pi->written_fields = new std::set<Field>(this->action_table->get_write_fields(rn->get_nf()));
    return pi;
}


/**
 * Setup MergerOperator to start listening and merging packets
 */
void MergerOperator::run() {
    // set up destination address
    if (this->merger_info->get_dest_ip().empty()) {
        fprintf(stderr, "No destinatino ip address specified by merger_info\n");
        exit(-1);
    }
    std::string addr_str = stringify(std::string(this->merger_info->get_dest_ip()), this->merger_info->get_dest_port());
    dest_address = address_from_string(addr_str);

    // send up one thread to handle each leaf node
    std::map<int, int> port_to_node_map = this->merger_info->get_port_to_node_map();

    pthread_t threads[num_nodes];
    int thread_i = 0;
    for (auto it = port_to_node_map.begin(); it != port_to_node_map.end(); ++it) {
        auto * tp = (NODE_THREAD_PARAMS*) malloc(sizeof(NODE_THREAD_PARAMS));
        tp->inst = this;
        tp->port = it->first;
        tp->node_id = it->second;
        pthread_create(&threads[thread_i++], nullptr, MergerOperator::run_node_thread_wrapper, tp);
    }

    void *status;
    for (int i = 0; i < num_nodes; i++) {
        pthread_join(threads[i], &status);
    }
}

/**
 * Helper function that prints the contents of packet_map to stdout
 */
void MergerOperator::print_packet_map() {
    for (auto it = packet_map.begin(); it != packet_map.end(); ++it) {
        printf("id: %d -> {", it->first);
        std::map<int, packet*>* this_node_map2 = it->second;
        for (auto it2 = this_node_map2->begin(); it2 != this_node_map2->end(); ++it2) {
            printf("%d: %s, ", it2->first, it2->second->data);
        }
        printf("}\n");
    }
}
#pragma clang diagnostic pop