//
//  FunctionTable.h
//  RuM
//
//  Created by Marc Hosecloth on 11/30/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef FunctionTable_h
#define FunctionTable_h

#include <unordered_map>
#include <stack>

#include "Function.h"

class FunctionTable {
    std::unordered_map<std::string, Function *> table;
    
public:
    FunctionTable() {
        table = *new std::unordered_map<std::string, Function *>();
        table["print"] = new Function();
        table["copy"] = new Function();
    }
    
    void print(Value * arg) {
        
        if (arg->getType() == "Reference") {
            std::cout << arg << std::endl;
        }
        
        else {
            std::cout << arg->getValue() << std::endl;
        }
    }
    
    Value * copy(Value * arg) {
        Value * deepCopy = new Value(arg->getValue(), arg->getType());
        
        return deepCopy;
    }
    
    bool contains(std::string identifier) {
        return table.find(identifier) != table.end();
    }
    
    void define(std::string functionName, Function * function) {
        if (!contains(functionName)) {
            table[functionName] = function;
        } else {
            std::cout << "Function already defined, you goofball!" << std::endl;
        }
    }
    
    Function * retrieve(std::string functionName) {
        Function * function = nullptr;
        
        if (contains(functionName)) {
            function = table[functionName];
        } else {
            std::cout << "Retrieval from function table failed: function undefined!" << std::endl;
        }
        
        return function;
    }
    
    Scope * prepareChildScope(std::string functionName, std::list<Value *> arguments) {
        Scope * childScope = nullptr;

        /*
         1. check that Function even exists within the fucking table
         2. verify Function signature
            MATCH THIS UP: parameterList[i]->getType == "Reference|Variable" && arguments[i]->is_reference == true|false
         3. assign return value to new cope
         4. assign (parametereterList[i]->getValue, arguments[i]) tuples to the new scope
         */
        
        // 1. check that Function even exists within the fucking table
        if (contains(functionName)) {
            Function * function = table[functionName];
            
            // 2. verify function signature
            bool signatureMatch = true;
            
            if (function->getParameterList().size() == arguments.size()) {
                std::list<Value *> params = function->getParameterList();
                std::list<Value *>::const_iterator args_iter = arguments.begin();
                
                while (args_iter != arguments.end()) {
//                    std::cout << params.front()->getValue() << " " << params.front()->getType() << " " << params.front()->is_reference() << std::endl;
                    
//                    std::cout << (*args_iter)->getValue() << " " << (*args_iter)->getType() << " " << (*args_iter)->is_reference() << std::endl;
                    
                    if (((params.front()->getType() == "Reference" && (*args_iter)->is_reference() == false) ||
                        (params.front()->getType() == "Variable" && (*args_iter)->is_reference() == true))) {
                            
                        signatureMatch = false;
                        
                    }
                    params.pop_front();
                        
                    args_iter++;
                }
                
                if (signatureMatch) {
                    // initialize new scope
                    childScope = new Scope();
                    childScope->assignVariable((function->getReturnValue())->getValue(), new Value());
                    
                    params = function->getParameterList();
                    args_iter = arguments.begin();
                        
                    while (args_iter != arguments.end()) {
                        childScope->assignVariable(params.front()->getValue(), (*args_iter));
                        
                        params.pop_front();
                        
                        args_iter++;
                    }
                }
            }
            else {
                signatureMatch = false;
                std::cout << "length of parameters list != length of arguments list :(" << std::endl;
            }
        }
        return childScope;
    }
};

class ObjectFunctionTable: public FunctionTable {
    std::string objectIdentifier;
    Scope * objectScope;
    
public:
    ObjectFunctionTable(std::string _objectIdentifier, Scope * _objectScope) {
        objectIdentifier = _objectIdentifier;
        objectScope = _objectScope;
        
        define("toString", new Function());
        define("get", new Function());
        define("set", new Function());
    }
    
    Value * toString() {
        return new Value(objectIdentifier, "String");
    }
    
    Value * get(std::string memberName) {
        Value * member = objectScope->retreive(memberName);
        return new Value(member->getValue(), member->getType());
    }
    
    void set(std::string memberName, Value * value) {
        objectScope->assignVariable(memberName, value);
    }
    
    Scope * prepareObjectChildScope(std::string methodName, std::list<Value *> arguments, Scope * objGlobalScope) {
        Scope * childScope = nullptr;
        
        /*
         1. check that Function even exists within the fucking table
         2. verify Function signature
         MATCH THIS UP: parameterList[i]->getType == "Reference|Variable" && arguments[i]->is_reference == true|false
         3. assign return value to new cope
         4. assign (parametereterList[i]->getValue, arguments[i]) tuples to the new scope
         */
        
        // 1. check that Function even exists within the fucking table
        if (contains(methodName)) {
            Function * function = retrieve(methodName);
            
            // 2. verify function signature
            bool signatureMatch = true;
            
            if (function->getParameterList().size() == arguments.size()) {
                std::list<Value *> params = function->getParameterList();
                std::list<Value *>::const_iterator args_iter = arguments.begin();
                
                while (args_iter != arguments.end()) {
                    //                    std::cout << params.front()->getValue() << " " << params.front()->getType() << " " << params.front()->is_reference() << std::endl;
                    
                    //                    std::cout << (*args_iter)->getValue() << " " << (*args_iter)->getType() << " " << (*args_iter)->is_reference() << std::endl;
                    
                    if (((params.front()->getType() == "Reference" && (*args_iter)->is_reference() == false) ||
                         (params.front()->getType() == "Variable" && (*args_iter)->is_reference() == true))) {
                        
                        signatureMatch = false;
                        
                    }
                    params.pop_front();
                    
                    args_iter++;
                }
                
                if (signatureMatch) {
                    // initialize new scope
                    childScope = new Scope();
                    childScope->assignVariable((function->getReturnValue())->getValue(), new Value());
                    
                    params = function->getParameterList();
                    args_iter = arguments.begin();
                    
                    while (args_iter != arguments.end()) {
                        
                        if (!objGlobalScope->contains((*args_iter)->getValue())) {
                            childScope->assignVariable(params.front()->getValue(), (*args_iter));
                        }
                        
                        params.pop_front();
                        
                        args_iter++;
                    }
                }
            }
            else {
                signatureMatch = false;
                std::cout << "length of parameters list != length of arguments list :(" << std::endl;
            }
        }
        return childScope;
    }
};

#endif /* FunctionTable_h */
