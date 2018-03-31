//
// Created by Victoria on 2018-03-31.
//

#include "MergerOperator.h"


std::string packet_filter_expr = "tcp";


MergerOperator::MergerOperator() { }

/* FUNCTIONS FOR PERFORMING MERGER OPERATIONS */

/* sets up hardcoded MergerInfo object to do testing on */
MergerInfo* MergerOperator::setup_dummy_info() {
    RuntimeNode n1 (1, snort); // node that drops packets
    RuntimeNode n2 (2, snort); // node that sends packets

    std::map<std::string, RuntimeNode*> interface_leaf_map;
    interface_leaf_map.insert(std::make_pair("eth1", &n1));
    interface_leaf_map.insert(std::make_pair("eth2", &n1));

    std::vector<ConflictItem*> conflicts_list;
    std::vector<RuntimeNode*> service_graph;

    MergerInfo* mi = new MergerInfo(interface_leaf_map, conflicts_list, service_graph);
    return mi;
}


void MergerOperator::run() {
    printf("wtf is this bullshit\n");

    // create dummy MergerInfo object
    this->merger_info = setup_dummy_info();

    this->logfile = fopen("log.txt","w");
    if (this->logfile == NULL) printf("Unable to create file.");

    // create pcap handles
    std::string dst_dev = "eth3"; // destination to send packets after merge

    configure_device_read_handles(packet_filter_expr);
    configure_device_write_handle(packet_filter_expr, dst_dev);

    printf("finished setting up bullshit\n");

    // listen for and process incoming packets
    for (std::map<std::string, RuntimeNode>::iterator it = interface_leaf_map.begin(); it != interface_leaf_map.end(); ++it) {
        /* loop for callback function */
        cur_dev = it->first;
        pcap_loop(src_dev_handle_map[it->first], 3, this->process_packet, NULL);
    }
}

void MergerOperator::process_packet(u_char *arg,
                    const struct pcap_pkthdr* pkthdr,
                    const u_char* packet)
{
    struct packet *pkt_info = new struct packet(packet, pkthdr->len);

    this->print_ip_header(pkt_info->ip_header);
    this->print_tcp_packet(pkt_info->tcp_header);
    this->print_data(pkt_info->data, pkt_info->data_size);

    int packet_id = ntohs(pkt_info->ip_header->ip_id);

    /* add packet to the map */
    std::vector<nf_packet> *pkts;
    if (packet_map.count(packet_id) > 0) {
        pkts = packet_map[packet_id];
    } else {
        pkts = new std::vector<nf_packet>();
    }
    nf_packet p;
    p.pkt = pkt_info;
    RuntimeNode n = interface_leaf_map.at(cur_dev);
    p.nf = n.get_nf();
    pkts->push_back(p);
    packet_map[packet_id] = pkts;

}



/* HELPER FUNCTIONS FOR OPERATING ON PACKETS */
void MergerOperator::configure_device_read_handles(std::string packet_filter_expr)
{
    std::map<std::string, RuntimeNode*> interface_leap_map = this->merger_info->get_interface_leaf_map();

    for (std::map<std::string, RuntimeNode>::iterator it = interface_leaf_map.begin(); it != interface_leaf_map.end(); ++it) {

        std::string dev = it->first;

        char errbuf[PCAP_ERRBUF_SIZE];
        struct bpf_program fp;        /* hold compiled program */
        bpf_u_int32 maskp;            /* subnet mask */
        bpf_u_int32 netp;             /* ip */

        if (pcap_lookupnet(dev.c_str(), &netp, &maskp, errbuf) < 0) {
            std::cerr << errbuf << std::endl;
            exit(-1);
        }

        /* open device for reading in promiscuous mode */
        src_dev_handle_map[dev] = pcap_open_live(dev.c_str(), BUFSIZ, 1,-1, errbuf);

        if(src_dev_handle_map[dev] == NULL) {
            printf("pcap_open_live(): %s\n", errbuf);
            exit(-1);
        }

        /* Now we'll compile the filter expression*/
        if(pcap_compile(src_dev_handle_map[dev], &fp, packet_filter_expr.c_str(), 0, netp) == -1) {
            fprintf(stderr, "Error calling pcap_compile\n");
            exit(-1);
        }

        /* set the filter */
        if(pcap_setfilter(src_dev_handle_map[dev], &fp) == -1) {
            fprintf(stderr, "Error setting filter\n");
            exit(-1);
        }

    }
}


void MergerOperator::configure_device_write_handle(std::string packet_filter_expr,
                                   std::string dev)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    const u_char *packet;
    struct bpf_program fp;        /* hold compiled program */
    bpf_u_int32 maskp;            /* subnet mask */
    bpf_u_int32 netp;             /* ip */

    if (pcap_lookupnet(dev.c_str(), &netp, &maskp, errbuf) < 0) {
        std::cerr << errbuf << std::endl;
    }


    /* open device for reading in promiscuous mode */
    dst_dev_handle = pcap_open_live(dev.c_str(), BUFSIZ, 1,-1, errbuf);

    if(dst_dev_handle == NULL) {
        printf("pcap_open_live(): %s\n", errbuf);
        exit(-1);
    }
}


/* HELPER FUNCTIONS THAT PRINT STUFF */
void MergerOperator::print_tcp_packet(struct tcphdr *tcph)
{
    fprintf(this->logfile,"\n");
    fprintf(this->logfile,"TCP Header\n");
    fprintf(this->logfile,"   |-Source Port      : %u\n",ntohs(tcph->source));
    fprintf(this->logfile,"   |-Destination Port : %u\n",htons(tcph->dest));
    fprintf(this->logfile,"   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    fprintf(this->logfile,"   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    fprintf(this->logfile,"   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    //fprintf(this->logfile,"   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //fprintf(this->logfile,"   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    fprintf(this->logfile,"   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    fprintf(this->logfile,"   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    fprintf(this->logfile,"   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    fprintf(this->logfile,"   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    fprintf(this->logfile,"   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    fprintf(this->logfile,"   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    fprintf(this->logfile,"   |-Window         : %d\n",ntohs(tcph->window));
    fprintf(this->logfile,"   |-Checksum       : %d\n",ntohs(tcph->check));
    fprintf(this->logfile,"   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    fprintf(this->logfile,"\n");
    fprintf(this->logfile,"                        DATA Dump                         ");
    fprintf(this->logfile,"\n");

}



void MergerOperator::print_ip_header(struct ip *iph)
{
    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(iph->ip_src), sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(iph->ip_dst), destIp, INET_ADDRSTRLEN);

    fprintf(this->logfile,"\n");
    fprintf(this->logfile,"IP Header\n");
    fprintf(this->logfile,"   |-IP Version        : %d\n",(unsigned int)iph->ip_v);
    fprintf(this->logfile,"   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ip_hl,((unsigned int)(iph->ip_hl))*4);
    fprintf(this->logfile,"   |-Type Of Service   : %d\n",(unsigned int)iph->ip_tos);
    fprintf(this->logfile,"   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->ip_len));
    fprintf(this->logfile,"   |-Identification    : %d\n",ntohs(iph->ip_id));
    //fprintf(this->logfile,"   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(this->logfile,"   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(this->logfile,"   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(this->logfile,"   |-TTL      : %d\n",(unsigned int)iph->ip_ttl);
    fprintf(this->logfile,"   |-Protocol : %d\n",(unsigned int)iph->ip_p);
    fprintf(this->logfile,"   |-Checksum : %d\n",ntohs(iph->ip_sum));
    fprintf(this->logfile,"   |-Source IP        : %s\n", sourceIp);
    fprintf(this->logfile,"   |-Destination IP   : %s\n", destIp);
}

void MergerOperator::print_data(u_char* data, int size)
{
    std::string dataStr = "";

    /* convert non-printable characters, other than carriage return, line feed,
     * or tab into periods when displayed. */
    for (int i = 0; i < size; i++) {
        if ((data[i] >= 32 && data[i] <= 126) || data[i] == 10 || data[i] == 11 || data[i] == 13) {
            dataStr += (char)data[i];
        } else {
            dataStr += ".";
        }
    }

    fprintf(this->logfile,"Data Payload\n%s", dataStr.c_str());

    fprintf(this->logfile,"\n###########################################################\n");
}
