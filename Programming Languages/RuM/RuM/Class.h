//
//  Class.h
//  RuM
//
//  Created by Marc Hosecloth on 12/1/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Class_h
#define Class_h

#include <unordered_map>

#include "Value.h"
#include "Function.h"

class Class {
    std::unordered_map<std::string, Value> members;
    std::unordered_map<std::string, Function> methods;
    
public:
    Class() {
        members = *new std::unordered_map<std::string, Value>();
        methods = *new std::unordered_map<std::string, Function>();
    }
    
    std::unordered_map<std::string, Value> getMembers() {
        return members;
    }
    
    std::unordered_map<std::string, Function> getMethods() {
        return methods;
    }
    
    void defineMember(std::string memberName, Value memberValue) {
        std::unordered_map<std::string, Value>::const_iterator got = members.find(memberName);
        
        if (got == members.end()) {
            members[memberName] = memberValue;
        }
        
        else {
            std::cout << "Class member already defined, you goofball!" << std::endl;
        }
    }
    
    void defineMethod(std::string methodName, Function method) {
        std::unordered_map<std::string, Function>::const_iterator got = methods.find(methodName);
        
        if (got == methods.end()) {
            methods[methodName] = method;
            std::cout << "Defined method: " << methodName << std::endl;
        }
        
        else {
            std::cout << "Class member already defined, you goofball!" << std::endl;
        }
    }
};

#endif /* Class_h */
