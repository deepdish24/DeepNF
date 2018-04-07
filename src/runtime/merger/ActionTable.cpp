//
// Created by Victoria on 2018-04-04.
//

#include "ActionTable.h"

/**
 * Initialize hardcoded action table
 */
ActionTable::ActionTable() {
    std::set<Field> dnf_firewall_write_fields = { SIP };
    write_fields_map.insert(std::make_pair(NF::dnf_firewall, dnf_firewall_write_fields));
}

/**
 * @param nf    The NF to check
 * @return a list of all packet fields that the NF might write to
 */
std::set<Field> ActionTable::get_write_fields(NF nf) {
    return write_fields_map.at(nf);
}