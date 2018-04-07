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
	ether_header *ethernet_header_temp = new struct ether_header();
	ethernet_header_temp->ether_type = htons(ETHERTYPE_IP);

	auto *ip_header_temp = new struct ip();
	ip_header_temp->ip_p = IPPROTO_TCP;
	inet_pton(AF_INET, sip.c_str(), &(ip_header_temp->ip_src));
	inet_pton(AF_INET, dip.c_str(), &(ip_header_temp->ip_dst));
	ip_header_temp->ip_id = htons(id);
	printf("ip_header_temp->ip_id = htons(id);\n");

	tcphdr *tcp_header_temp = new struct tcphdr();
	tcp_header_temp->source = htons(sp);
	tcp_header_temp->dest = htons(dp);
	printf("tcp_header_temp->dest = htons(dp);\n");

	u_char *data_temp = (u_char *)data.c_str();
	std::cout << "[" << data_temp << "]\n";
	int data_size_temp = data.size();
	int size_temp = sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr) + data_size_temp;
	ip_header_temp->ip_len = htons(size_temp);
	printf("ip_header_temp->ip_len = htons(size_temp);\n");

	u_char *pkt_char = (u_char*)malloc(size_temp);
	memcpy(pkt_char, (void *)ethernet_header_temp, sizeof(struct ether_header));
	memcpy(pkt_char + sizeof(struct ether_header), (void *)ip_header_temp, sizeof(struct ip));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip), (void *)tcp_header_temp, sizeof(struct tcphdr));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr), (void *)data_temp, data_size_temp);
	pkt = pkt_char;
	printf("pkt = pkt_char;;\n");

	printf("free(this->data);\n");

	delete ethernet_header_temp;
	delete ip_header_temp;
	delete tcp_header_temp;

	*this = packet(pkt_char, size_temp);

//	size = size_temp;
//	ethernet_header = (struct ether_header*)pkt;
//	printf("ethernet_header = (struct ether_header*)pkt;\n");
//	if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP) {
//		ip_header = (struct ip*)(pkt + sizeof(struct ether_header));
//		printf("ip_header = (struct ip*)(pkt + sizeof(struct ether_header));\n");
//		if (ip_header->ip_p == IPPROTO_TCP) {
//			tcp_header = (tcphdr*)(pkt + sizeof(struct ether_header) + sizeof(struct ip));
//			this->data = (u_char*)(pkt + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
//			data_size = size - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
//			printf("data_size = size - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));\n");
//		}
//	}

}

packet::~packet() {
	delete this->tcp_header;
	delete this->ip_header;
	delete this->ethernet_header;
}

bool packet::is_null()
{
	printf("is_null called, ntohs(tcp_header->source): %d\n", ntohs(tcp_header->source));
	char sourceIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_src), sourceIp, INET_ADDRSTRLEN);
	return ntohs(tcp_header->source) == 0 && std::string(sourceIp) == "0.0.0.0";
}

void packet::nullify()
{
	printf("nullify() called, packet is null now\n");
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


