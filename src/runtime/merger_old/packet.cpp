#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>

#include "packet.h"

packet::packet(const u_char *pkt, int pkt_size)
{
	this->pkt_char = pkt;
	size = pkt_size;
	ethernet_header = (struct ether_header*)pkt;
	if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP) {
	    ip_header = (struct ip*)(pkt + sizeof(struct ether_header));
	    if (ip_header->ip_p == IPPROTO_TCP) {
	        tcp_header = (tcphdr*)(pkt + sizeof(struct ether_header) + sizeof(struct ip));
	        data = (u_char*)(pkt + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
	        data_size = pkt_size - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
	    }
	}
}

bool packet::is_null()
{
	char sourceIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_src), sourceIp, INET_ADDRSTRLEN);
	return ntohs(tcp_header->source) == 0 && std::string(sourceIp) == "0.0.0.0";
}

void packet::nullify()
{
	tcp_header->source = htons(0);
    memset(&(ip_header->ip_src), 0, sizeof(ip_header->ip_src));
}

packet* packet::copy() {
	struct packet *pkt_info = new struct packet(this->pkt_char, this->size);
	return pkt_info;
}