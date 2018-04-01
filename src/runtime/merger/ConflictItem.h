//
// Created by Victoria on 2018-03-31.
//

#ifndef DEEPNF_CONFLICTITEM_H
#define DEEPNF_CONFLICTITEM_H

#include <map>
#include <vector>

#include "Field.h"

class ConflictItem {

    public:

        ConflictItem(int major, int minor, int parent);
        ConflictItem(int major, int minor, int parent, std::vector<Field> conflicts);

        int get_major();
        int get_minor();
        int get_parent();
        std::vector<Field> get_conflicts();

        void add_conflict(Field conflict);

    private:
        int major; // id of the runtime node containing the NF with precedence in the conflict
        int minor; // id of the runtime node containing the NF without precedence in the conflict
        int parent; // id of the runtime node that is the parent of the major and minor
        std::vector<Field> conflicts; // a list of conflicting actions between the major and minor
};


#endif //DEEPNF_CONFLICTITEM_H
