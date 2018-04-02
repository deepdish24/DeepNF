#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <vector>

#include "packet.h"
#include "pcap.h"
#include "RuntimeNode.h"
#include "MergerOperator.h"

int main(int argc,char **argv)
{

    MergerOperator* mo = new MergerOperator();
//    mo->run();

    return 0;
}

