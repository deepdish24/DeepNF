//
// Created by Victoria on 2018-03-31.
//

#include "ActionTableHelper.h"

ActionTableHelper::ActionTableHelper(json action_table) {
    this->action_table = action_table;
    this->fields_map_initialized = false;
}


/**
* @return a map mapping each NF to a list of all fields that the NF might write to
*/
std::map<NF, std::set<Field>> ActionTableHelper::get_write_fields_map() {
    if (!this->fields_map_initialized) {
        for (int nf_it = 0; nf_it != NF::nf_end; nf_it++) {
            NF nf = (NF) nf_it;
            this->fields_map[nf] = this->get_write_fields(nf);
        }
        this->fields_map_initialized = true;
    }

    return fields_map;
};


/**
 * @param nf    The NF to check
 * @return a list of all packet fields that the NF might write to
 */
std::set<Field> ActionTableHelper::get_write_fields(NF nf) {
    if (this->fields_map_initialized) {
        return fields_map[nf];
    }

    std::set<Field> ret_set;
    std::string nf_str = nf::nf_to_string(nf);

    // check all possible fields
    for ( int field_it = 0; field_it != Field::field_end; field_it++ )
    {
        Field field = (Field) field_it;
        std::string field_str = field::field_to_string(field);
        if (ActionTableHelper::is_writable(action_table[nf_str][field_str])) {
            ret_set.insert(field);
        }
    }

    return ret_set;

}


/**
 * @param   The contents of the action field in the action table format, ie. "R/W", "null", "W", etc.
 * @return  Whether or not the contents indicate that a field is writable
 */
bool ActionTableHelper::is_writable(std::string rw_field) {
    // return true is rw_field is either "W" or "R/W";
    return (rw_field.compare("R/W") == 0 || rw_field.compare("W") == 0);
}