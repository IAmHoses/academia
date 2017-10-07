//
//  ObjectTable.h
//  RuM
//
//  Created by Marc Hosecloth on 12/2/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef ObjectTable_h
#define ObjectTable_h

#include <unordered_map>

#include "Object.h"

class ObjectTable {
    std::unordered_map<std::string, Object *> table;
    
public:
    ObjectTable() {
        table = *new std::unordered_map<std::string, Object *>();
    }
    
    bool contains(std::string objectName) {
        return table.find(objectName) != table.end();
    }
    
    void remove(std::string objectName) {
        table.erase(objectName);
    }
    
    void assign(std::string objectName, Object * object) {
        table[objectName] = object;
    }
    
    Object * retrieve(std::string objectName) {
        if (contains(objectName)) {
            return table[objectName];
        }
        return nullptr;
    }
};

#endif /* ObjectTable_h */
