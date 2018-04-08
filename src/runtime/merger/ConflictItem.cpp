//
// Created by Victoria on 2018-03-31.
//


#include "ConflictItem.h"

ConflictItem::ConflictItem(int major, int minor, int parent) {
    this->major = major;
    this->minor = minor;
    this->parent = parent;
}

ConflictItem::ConflictItem(int major, int minor, int parent, std::vector <Field> conflicts) {
    this->major = major;
    this->minor = minor;
    this->parent = parent;
    this->conflicts = conflicts;
}

int ConflictItem::get_major() { return this->major; }

int ConflictItem::get_minor() { return this->minor; }

int ConflictItem::get_parent() { return this->parent; }

std::vector<Field> ConflictItem::get_conflicts() { return this->conflicts; }

std::string ConflictItem::to_string() {

    size_t bufsize = 1000;
    char* buffer = 0;
    int size = snprintf(buffer, bufsize, "(maj=%d, min=%d, par=%d)", this->major, this->minor, this->parent);

    std::string ret_str = std::string(buffer);
    return ret_str;

}