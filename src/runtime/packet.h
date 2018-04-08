#ifndef __FAVOR_BSD
#define __FAVOR_BSD
#endif
// #pragma once

#ifndef DEEPNF_PACKET_H
#define DEEPNF_PACKET_H

#include <net/ethernet.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <string>

#include "address.h"



struct packet
{

public:

	packet(const u_char *packet, int packet_size);
    packet(std::string sip, int sp, std::string dip, int dp, unsigned int id, std::string data);
	~packet();

	/* reference:
     * http://tonylukasavage.com/blog/2010/12/19/offline-packet-capture-analysis-with-c-c----amp--libpcap/ */
    const u_char *pkt; 
    int size;
    struct ether_header* ethernet_header;
    struct ip* ip_header;
    struct tcphdr* tcp_header;
    u_char *data;
    int data_size;

	void init_packet(const u_char *pkt, int pkt_size);
    bool is_null();
    void nullify();
    void print_info();

	std::string get_src_ip();
	int get_src_port();
	std::string get_dest_ip();
	int get_dest_port();
	std::string get_payload();
    int get_pkt_id();

	void write_dest_ip(std::string dest_ip);
	void write_dest_port(int dest_port);
	void write_payload(std::string payload);
};

#endif