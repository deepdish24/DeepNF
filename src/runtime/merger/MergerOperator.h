//
// Created by Victoria on 2018-04-04.
//

#ifndef DEEPNF_MERGEROPERATOR_H
#define DEEPNF_MERGEROPERATOR_H

#include <arpa/inet.h>
#include <errno.h>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <vector>

#include "ActionTable.h"
#include "MergerInfo.h"

class MergerOperator {

public:

    MergerOperator();
    void run();

private:

    // contains information about virtual network interfaces, conflicting NF pairs, etc.
    MergerInfo* merger_info;

    // contains information about the NF action table
    ActionTable* action_table;
};



#endif //DEEPNF_MERGEROPERATOR_H
