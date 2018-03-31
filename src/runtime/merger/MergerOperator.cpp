//
// Created by Victoria on 2018-03-31.
//

#include "MergerOperator.h"

MergerOperator::MergerOperator() { }

MergerInfo* MergerOperator::setup_dummy_info() {
    RuntimeNode n1 (1, snort); // node that drops packets
    RuntimeNode n2 (2, snort); // node that sends packets


}

void MergerOperator::run() {
    printf("wtf is this bullshit");

}