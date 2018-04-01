#ifndef DEEPNF_PACKET_H
#define DEEPNF_PACKET_H

#include <net/ethernet.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header

struct packet
{

public:

	packet(const u_char *packet, int packet_size);

	/* reference:
     * http://tonylukasavage.com/blog/2010/12/19/offline-packet-capture-analysis-with-c-c----amp--libpcap/ */
    const u_char *pkt_char;
    int size;
    struct ether_header* ethernet_header;
    struct ip* ip_header;
    struct tcphdr* tcp_header;
    u_char *data;
    int data_size;
    
    bool is_null();
    void nullify();
    packet* copy(); // returns an exact copy of the current packet instance
};

#endif