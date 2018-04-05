#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "packet.h"

packet::packet(const u_char *pkt, int pkt_size)
{
	this->pkt = pkt;
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

packet::packet(std::string sip, int sp, std::string dip, int dp, unsigned int id, std::string data)
{
	ethernet_header = new struct ether_header();
	ethernet_header->ether_type = htons(ETHERTYPE_IP);

	ip_header = new struct ip();
	ip_header->ip_p = IPPROTO_TCP;
	inet_pton(AF_INET, sip.c_str(), &(ip_header->ip_src));
	inet_pton(AF_INET, dip.c_str(), &(ip_header->ip_dst));
	ip_header->ip_id = htons(id);

	tcp_header = new struct tcphdr();
	tcp_header->source = htons(sp);
	tcp_header->dest = htons(dp);
	
	this->data = (u_char *)data.c_str();
	std::cout << this->data << "\n";
	data_size = data.size();
	size = sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr) + data_size;
	ip_header->ip_len = htons(size);
	
	u_char *pkt_char = (u_char*)malloc(size);
	memcpy(pkt_char, (void *)ethernet_header, sizeof(struct ether_header));
	memcpy(pkt_char + sizeof(struct ether_header), (void *)ip_header, sizeof(struct ip));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip), (void *)tcp_header, sizeof(struct tcphdr));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr), (void *)this->data, data_size);
	pkt = pkt_char;
}

packet::~packet() {
	delete this->tcp_header;
	delete this->ip_header;
	delete this->ethernet_header;
}

bool packet::is_null()
{
	char sourceIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_src), sourceIp, INET_ADDRSTRLEN);
	return ntohs(tcp_header->source) == 0 && std::string(sourceIp) == "0.0.0.0";
}

void packet::nullify()
{
	tcp_header->source = 0;
    memset(&(ip_header->ip_src), 0, sizeof(ip_header->ip_src));
}

void packet::print_info()
{
	std::cout << "------------------\n";

	char source_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
	std::cout << "sip = " << source_ip << "\n";
	std::cout << "sport = " << ntohs(tcp_header->source) << "\n";

	char dest_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);
	std::cout << "dip = " << dest_ip << "\n";
	std::cout << "dport = " << ntohs(tcp_header->dest) << "\n";

	std::cout << "Identification = " << ntohs(ip_header->ip_id) << "\n";
	std::cout << "data = " << data << "\n";

}


