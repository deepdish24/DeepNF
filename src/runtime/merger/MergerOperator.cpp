//
// Created by Victoria on 2018-04-04.
//

#include "MergerOperator.h"
#include "MergerInfo.h"


MergerOperator::MergerOperator() {
    printf("MergerOperator::MergerOperator \n");

    // set up action table
    this->action_table = new ActionTable();
    this->merger_info = MergerInfo::get_dummy_merger_info();
}

void MergerOperator::run() {
    printf("MergerOperator::run() \n");
}