#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "packet.h"

packet::packet(const u_char *pkt, int pkt_size)
{
    this->init_packet(pkt, pkt_size);
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

	tcphdr *tcp_header_temp = new struct tcphdr();
	tcp_header_temp->source = htons(sp);
	tcp_header_temp->dest = htons(dp);

	u_char *data_temp = (u_char *)data.c_str();
	std::cout << "[" << data_temp << "]\n";
	int data_size_temp = data.size();
	int size_temp = sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr) + data_size_temp;
	ip_header_temp->ip_len = htons(size_temp);

	u_char *pkt_char = (u_char*)malloc(size_temp);
	memcpy(pkt_char, (void *)ethernet_header_temp, sizeof(struct ether_header));
	memcpy(pkt_char + sizeof(struct ether_header), (void *)ip_header_temp, sizeof(struct ip));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip), (void *)tcp_header_temp, sizeof(struct tcphdr));
	memcpy(pkt_char + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr), (void *)data_temp, data_size_temp);

    this->init_packet(pkt_char, size_temp);
}

void packet::init_packet(const u_char *pkt, int pkt_size) {
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

packet::~packet() {

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

std::string packet::get_src_ip() {
    char source_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
    return std::string(reinterpret_cast<char*>(source_ip));
}
int packet::get_src_port() {
    return ntohs(tcp_header->source);
}
std::string packet::get_dest_ip() {
    char dest_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);
    return std::string(reinterpret_cast<char*>(dest_ip));
}

int packet::get_dest_port() {
    return ntohs(tcp_header->dest);
}

std::string packet::get_payload() {
    return std::string(reinterpret_cast<char*>(data));
}

int packet::get_pkt_id() {
    return ntohs(ip_header->ip_id);
}


void packet::write_dest_ip(std::string dest_ip) {
    inet_pton(AF_INET, dest_ip.c_str(), &(ip_header->ip_dst));
}

void packet::write_dest_port(int dest_port) {
    tcp_header->dest = htons(dest_port);
}

void packet::write_payload(std::string payload) {

    packet* old_pkt = this;

    struct packet new_packet(
            this->get_src_ip(),
            this->get_src_port(),
            this->get_dest_ip(),
            this->get_dest_port(),
            (unsigned int) this->get_pkt_id(),
            std::move(payload)
    );

    init_packet(new_packet.pkt, new_packet.size);
//    free(old_pkt);
}
