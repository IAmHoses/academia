//
//  Scope.h
//  RuM
//
//  Created by Marc Hosecloth on 11/26/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Scope_h
#define Scope_h

#include <unordered_map>

#include "Value.h"

class Scope {
    std::unordered_map<std::string, Value *> symbolTable;
    std::unordered_map<std::string, std::list<Value>> listTable;
    
public:
    Scope() {
        symbolTable = *new std::unordered_map<std::string, Value *>();
        listTable = *new std::unordered_map<std::string, std::list<Value>>();
    }
    
    bool contains(std::string identifier) {
        return (symbolTable.find(identifier) != symbolTable.end()) || (listTable.find(identifier) != listTable.end());
    }
    
    bool inSymbolTable(std::string identifier) {
        return symbolTable.find(identifier) != symbolTable.end();
    }
    
    bool inListTable(std::string identifier) {
        return listTable.find(identifier) != listTable.end();
    }
    
    void remove(std::string identifier) {
        symbolTable.erase(identifier);
    }
    
    void assignVariable(std::string identifier, Value * value) {
        if (!inListTable(identifier)) {
            if (value->getType() == "Reference") {
                if (inSymbolTable(value->getValue())) { // if value initialized in table
                    symbolTable[identifier] = symbolTable[value->getValue()];
                }
            }
            else {
                if (!inSymbolTable(value->getValue())) {    // variable not found in table
                    symbolTable[identifier] = value;
                }
                else {  // must replace varaibles
                    symbolTable[identifier]->set(value->getValue(), value->getType());
                }
            }
        }
        else {
            std::cout << "Unable to assign variable, identifer already exists as list object" << std::endl;
        }
    }
    
    void assignList(std::string identifier, std::list<Value> list) {
        
        bool test = !inSymbolTable(identifier);
        
        if (!inSymbolTable(identifier)) {
//            std::list<Value *> listCopy = *new std::list<Value*>(list);
            listTable[identifier] = list;
            
        }
        else {
            std::cout << "Unable to instantiate list, identifer already assigned variable" << std::endl;
        }
    }

    Value * retreive(std::string identifier, int index = -1) {
        Value * variable = nullptr;
//        std::cout << "Value of retrieved variable from variable table" << symbolTable[identifier].getValue() << std::endl;
        
        if (inSymbolTable(identifier)) {
            variable = symbolTable.find(identifier)->second;
        }
        
        if (inListTable(identifier)) {
            
        }
        
//        std::unordered_map<std::string, Value *>::const_iterator got = symbolTable.find(identifier);
//        
//        if (got != symbolTable.end()) {
//            variable = got->second;
//            
////            std::cout << variable.getValue() << " " << variable.getType() << std::endl;
//        }
        return variable;
    }
};

#endif /* Scope_h */
