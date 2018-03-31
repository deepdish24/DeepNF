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

    logfile = fopen("log.txt","w");
    if (logfile == NULL) printf("Unable to create file.");

    // create pcap handles
    std::string dst_dev = "eth3"; // destination to send packets after merge

    configure_device_read_handles(packet_filter_expr);
    configure_device_write_handle(packet_filter_expr, dst_dev);

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