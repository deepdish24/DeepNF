#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <errno.h>
// #include <sys/socket.h>

// #include "packet.h"
// #include "pcap.h"


// void print_data(u_char* data, int size);
// void print_ip_header(struct ip *iph);
// void print_tcp_packet(struct tcphdr *tcph);
// void forward_packet(const u_char* packet, int size);
// void process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr,
//         const u_char* packet);
// void configure_device_read_handle(std::string packet_filter_expr,
//                                   std::string dev);

// void configure_device_write_handle(std::string packet_filter_expr,
//                                    std::string dev);
// void run_firewall(struct packet *pkt_info);


// FILE *logfile;
// pcap_t* src_dev_handle;
// pcap_t* dst_dev_handle;

std::string read(int sockfd);

void process();

void write(std::string data, std::string ip, int port);

int main(int argc,char **argv)
{   
    if (argc != 3) {
        std::cerr << "need 2 arguments, ip and port\n";
        return -1;
    }

    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(800);
    
    bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listen_fd, 10);
    
    while (true) {

        std::string data = read(listen_fd);

        // TODO: process packet

        // forward packet 
        write(data, argv[1], std::stoi(argv[2]));

        // std::cout << buf << "\n";
        // close(comm_fd);
    }

    close(listen_fd);

    // /* more info here: https://linux.die.net/man/7/pcap-filter */
    // std::string packet_filter_expr = "tcp";
    
    // logfile = fopen("log.txt","w");
    // if (logfile == NULL) printf("Unable to create file.");

    // /* Now get a device */
    // // dev = pcap_lookupdev(errbuf);
    
    // std::string src_dev = "eth1";
    // std::string dst_dev = "eth2";
    
    // configure_device_read_handle(packet_filter_expr, src_dev);
    // configure_device_write_handle(packet_filter_expr, dst_dev);
    
    // /* loop for callback function */
    // pcap_loop(src_dev_handle, -1, process_packet, NULL);

    return 0;
}


std::string read(int sockfd)
{
   struct sockaddr_in clientaddr;
   socklen_t clientaddrlen = sizeof(clientaddr);
   int comm_fd = accept(sockfd, (struct sockaddr*)&clientaddr, &clientaddrlen);    

   char buf[1024];
   int n = read(comm_fd, buf, 1023);
   if (n < 0) {
    std::cerr << "read error: " << strerror(errno) << std::endl;
   }
   std::cout << "read " << n << " bytes\n";
   buf[n] = 0;

   close(comm_fd);

   return std::string(buf);
}

void write(std::string data, std::string ip, int port)
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);  
    if (sockfd < 0) {
      fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));    
      exit(1);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &(servaddr.sin_addr));
    
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "connect error: " << strerror(errno) << std::endl;
        exit(-1);
    }    

    int num_bytes = write(sockfd, data.c_str(), data.size());
    if (num_bytes < 0) {
        std::cerr << "write error: " << strerror(errno) << "\n";
    }
    std::cout << "wrote " << num_bytes << " bytes\n";
    
    close(sockfd);
}

// void configure_device_read_handle(std::string packet_filter_expr,
//                                   std::string dev)
// {
//     char errbuf[PCAP_ERRBUF_SIZE];
//     const u_char *packet;
//     struct bpf_program fp;        /* hold compiled program */
//     bpf_u_int32 maskp;            /* subnet mask */
//     bpf_u_int32 netp;             /* ip */

//     if (pcap_lookupnet(dev.c_str(), &netp, &maskp, errbuf) < 0) {
//         std::cerr << errbuf << std::endl;
//         exit(-1);
//     }

 
//     /* open device for reading in promiscuous mode */
//     src_dev_handle = pcap_open_live(dev.c_str(), BUFSIZ, 1,-1, errbuf);

//     if(src_dev_handle == NULL) {
//         printf("pcap_open_live(): %s\n", errbuf);
//         exit(-1);
//     }
    
//     /* Now we'll compile the filter expression*/
//     if(pcap_compile(src_dev_handle, &fp, packet_filter_expr.c_str(), 0, netp) == -1) {
//         fprintf(stderr, "Error calling pcap_compile\n");
//         exit(-1);
//     }
    
//     /* set the filter */
//     if(pcap_setfilter(src_dev_handle, &fp) == -1) {
//         fprintf(stderr, "Error setting filter\n");
//         exit(-1);
//     }
// }

// void configure_device_write_handle(std::string packet_filter_expr,
//                                   std::string dev)
// {
//     char errbuf[PCAP_ERRBUF_SIZE];
//     const u_char *packet;
//     struct bpf_program fp;        /* hold compiled program */
//     bpf_u_int32 maskp;            /* subnet mask */
//     bpf_u_int32 netp;             /* ip */

//     if (pcap_lookupnet(dev.c_str(), &netp, &maskp, errbuf) < 0) {
//         std::cerr << errbuf << std::endl;
//     }

    
//     /* open device for reading in promiscuous mode */
//     dst_dev_handle = pcap_open_live(dev.c_str(), BUFSIZ, 1,-1, errbuf);

//     if(dst_dev_handle == NULL) {
//         printf("pcap_open_live(): %s\n", errbuf);
//         exit(-1);
//     }
// }

// void process_packet(u_char *arg,
//                     const struct pcap_pkthdr* pkthdr,
//                     const u_char* packet)
// {
    
//     struct packet pkt_info(packet, pkthdr->len);

//     print_ip_header(pkt_info.ip_header);
//     print_tcp_packet(pkt_info.tcp_header);
//     print_data(pkt_info.data, pkt_info.data_size);

//     run_firewall(&pkt_info);
// }

// void print_tcp_packet(struct tcphdr *tcph)
// {        
//     fprintf(logfile,"\n");
//     fprintf(logfile,"TCP Header\n");
//     fprintf(logfile,"   |-Source Port      : %u\n",ntohs(tcph->source));
//     fprintf(logfile,"   |-Destination Port : %u\n",htons(tcph->dest));
//     fprintf(logfile,"   |-Sequence Number    : %u\n",ntohl(tcph->seq));
//     fprintf(logfile,"   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
//     fprintf(logfile,"   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
//     //fprintf(logfile,"   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
//     //fprintf(logfile,"   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
//     fprintf(logfile,"   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
//     fprintf(logfile,"   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
//     fprintf(logfile,"   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
//     fprintf(logfile,"   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
//     fprintf(logfile,"   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
//     fprintf(logfile,"   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
//     fprintf(logfile,"   |-Window         : %d\n",ntohs(tcph->window));
//     fprintf(logfile,"   |-Checksum       : %d\n",ntohs(tcph->check));
//     fprintf(logfile,"   |-Urgent Pointer : %d\n",tcph->urg_ptr);
//     fprintf(logfile,"\n");
//     fprintf(logfile,"                        DATA Dump                         ");
//     fprintf(logfile,"\n");
         
// }



// void print_ip_header(struct ip *iph)
// {
//     char sourceIp[INET_ADDRSTRLEN];
//     char destIp[INET_ADDRSTRLEN];

//     inet_ntop(AF_INET, &(iph->ip_src), sourceIp, INET_ADDRSTRLEN);
//     inet_ntop(AF_INET, &(iph->ip_dst), destIp, INET_ADDRSTRLEN);
     
//     fprintf(logfile,"\n");
//     fprintf(logfile,"IP Header\n");
//     fprintf(logfile,"   |-IP Version        : %d\n",(unsigned int)iph->ip_v);
//     fprintf(logfile,"   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ip_hl,((unsigned int)(iph->ip_hl))*4);
//     fprintf(logfile,"   |-Type Of Service   : %d\n",(unsigned int)iph->ip_tos);
//     fprintf(logfile,"   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->ip_len));
//     fprintf(logfile,"   |-Identification    : %d\n",ntohs(iph->ip_id));
//     //fprintf(logfile,"   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
//     //fprintf(logfile,"   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
//     //fprintf(logfile,"   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
//     fprintf(logfile,"   |-TTL      : %d\n",(unsigned int)iph->ip_ttl);
//     fprintf(logfile,"   |-Protocol : %d\n",(unsigned int)iph->ip_p);
//     fprintf(logfile,"   |-Checksum : %d\n",ntohs(iph->ip_sum));
//     fprintf(logfile,"   |-Source IP        : %s\n", sourceIp);
//     fprintf(logfile,"   |-Destination IP   : %s\n", destIp);
// }

// void print_data(u_char* data, int size)
// {
//     std::string dataStr = "";

//     /* convert non-printable characters, other than carriage return, line feed,
//      * or tab into periods when displayed. */
//     for (int i = 0; i < size; i++) {
//         if ((data[i] >= 32 && data[i] <= 126) || data[i] == 10 || data[i] == 11 || data[i] == 13) {
//             dataStr += (char)data[i];
//         } else {
//             dataStr += ".";
//         }
//     }

//     fprintf(logfile,"Data Payload\n%s", dataStr.c_str());

//     fprintf(logfile,"\n###########################################################\n");
// }

// void forward_packet(const u_char* packet, int size)
// {
//     if (pcap_sendpacket(dst_dev_handle, packet, size) < 0) {
//         std::cerr << strerror(errno) << std::endl;
//     }
// }

// void run_firewall(struct packet *pkt_info)
// {
//     /* if dip = 80, send a null packet (drop packet) */
//     if (htons(pkt_info->tcp_header->dest) == 8000) {
//         pkt_info->nullify();
//     }
//     forward_packet(pkt_info->pkt, pkt_info->size);
// }

