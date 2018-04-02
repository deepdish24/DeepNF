//
// Created by Victoria on 2018-04-02.
//

#ifndef DEEPNF_NFPACKET_H
#define DEEPNF_NFPACKET_H

#include "NF.h"
#include "Field.h"

#include <set>

class NFPacket {
    public:

        NFPacket();

        struct packet *pkt;
        int runtime_id;
        NF nf;
        // list of additional fields that have been written to, NOT INCLUDING fields written to by the
        // packet's NF
        std::set<Field> written_fields;
};


#endif //DEEPNF_NFPACKET_H
