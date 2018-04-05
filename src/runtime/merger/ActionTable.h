//
// Created by Victoria on 2018-04-04.
//

#ifndef DEEPNF_ACTIONTABLE_H
#define DEEPNF_ACTIONTABLE_H

#include <set>
#include <map>

#include "../../common/NF.h"
#include "../../common/Field.h"



class ActionTable {
    public:
        /**
         * Initialize hardcoded action table
         */
        ActionTable();

        /**
         * @param nf    The NF to check
         * @return a list of all packet fields that the NF might write to
         */
        std::set<Field> get_write_fields(NF nf);

    private:
        std::map<NF, std::set<Field>> write_fields_map;

};


#endif //DEEPNF_ACTIONTABLE_H
