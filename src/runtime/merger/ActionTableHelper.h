//
// Created by Victoria on 2018-03-31.
//

#ifndef DEEPNF_ACTIONTABLEHELPER_H
#define DEEPNF_ACTIONTABLEHELPER_H

#include <set>
#include <string>

#include "nlohmann/json.hpp"
#include "NF.h"
#include "Field.h"

using json = nlohmann::json;

class ActionTableHelper {

    public:
        ActionTableHelper(json action_table);

         /**
         * @return a map mapping each NF to a list of all fields that the NF might write to
         */
        std::map<NF, std::set<Field>> get_write_fields_map();

        /**
         * @param nf    The NF to check
         * @return a list of all packet fields that the NF might write to
         */
        std::set<Field> get_write_fields(NF nf);

         /**
         * @param   The contents of the action field in the action table format, ie. "R/W", "null", "W", etc.
         * @return  Whether or not the contents indicate that a field is writable
         */
        static bool is_writable(std::string rw_field);

    private:
        // json action table object that this ActionTableHelper instance will parse
        json action_table;

        std::map<NF, std::set<Field>> fields_map; // cache results of get_write_fields_map to increase efficiency
        bool fields_map_initialized;

};


#endif //DEEPNF_ACTIONTABLEHELPER_H
