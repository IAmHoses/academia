//
//  ClassTable.h
//  RuM
//
//  Created by Marc Hosecloth on 12/1/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef ClassTable_h
#define ClassTable_h

#include <unordered_map>

#include "Class.h"

class ClassTable {
    std::unordered_map<std::string, Class *> table;
    
public:
    ClassTable() {
        table = *new std::unordered_map<std::string, Class *>();
    }
    
    void define(std::string className, Class * newClass) {
        std::unordered_map<std::string, Class *>::const_iterator got = table.find(className);
        
        if (got == table.end()) {
            table[className] = newClass;
        } else {
            std::cout << "Class already defined, you goofball!" << std::endl;
        }
    }
    
    Class * retrieve(std::string className) {
        Class * foundClass = nullptr;
        
        std::unordered_map<std::string, Class *>::const_iterator got = table.find(className);
        
        if (got != table.end()) {
            foundClass = table[className];
        } else {
            std::cout << "Retrieval from class table failed: class undefined!" << std::endl;
        }
        
        return foundClass;
    }
};

#endif /* ClassTable_h */
