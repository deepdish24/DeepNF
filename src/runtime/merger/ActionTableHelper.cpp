//
// Created by Victoria on 2018-03-31.
//

#include "ActionTableHelper.h"

ActionTableHelper::ActionTableHelper() {
    // construct action table with dummy info
    std::set<Field> snort_write_fields = { Field::SIP };
    fields_map.insert(std::make_pair(NF::snort, snort_write_fields));
}


/**
* @return a map mapping each NF to a list of all fields that the NF might write to
*/
std::map<NF, std::set<Field>> ActionTableHelper::get_write_fields_map() {
    return fields_map;
};


/**
 * @param nf    The NF to check
 * @return a list of all packet fields that the NF might write to
 */
std::set<Field> ActionTableHelper::get_write_fields(NF nf) {
    return fields_map[nf];
}