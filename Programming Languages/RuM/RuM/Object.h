//
//  Object.h
//  RuM
//
//  Created by Marc Hosecloth on 12/1/16
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include <stack>

#include "Value.h"
#include "Scope.h"
#include "FunctionTable.h"
#include "ClassTable.h"
#include "ObjectTable.h"

class Object {
    Scope * objectScope;
    std::stack<Scope *> * objectRuntimeStack;
    ObjectFunctionTable * objectFunctionTable;
    
public:
    Object(std::string objectIdentifier, Class * blueprint) {
        objectScope = new Scope();
        objectRuntimeStack = new std::stack<Scope *>();
        objectFunctionTable = new ObjectFunctionTable(objectIdentifier, objectScope);
        
        for (auto it : blueprint->getMembers()) {
            objectScope->assignVariable(it.first, new Value(it.second.getValue(), it.second.getType()));
        }
        for (auto it : blueprint->getMethods()) {
            Function method = it.second;
            
            Value * returnValue = new Value((method.getReturnValue())->getValue(), (method.getReturnValue())->getType());
            
            objectFunctionTable->define(it.first, new Function(method.getParameterList(), method.getCode(), returnValue));
        }
    }
    
    bool containsMember(std::string memberName) {
        return objectScope->contains(memberName);
    }
    
    bool containsMethod(std::string methodName) {
        return objectFunctionTable->contains(methodName);
    }
    
    Value * dispatch(std::string functionName, std::list<Value *> arguments) {
        Value * returnValue = nullptr;
        
        if (functionName == "toString" && arguments.size() == 0) {
            returnValue = objectFunctionTable->toString();
        }
        
        else if (functionName == "get" && arguments.size() == 1) {
            returnValue = objectFunctionTable->get((arguments.front())->getValue());
        }
        
        else if (functionName == "set" && arguments.size() == 2) {
            std::string memberName = (arguments.front())->getValue();
            arguments.pop_front();
            
            objectFunctionTable->set(memberName, arguments.front());
        }
        
        else {
            return new Value("DYNAMIC_DISPATCH", "DYNAMIC_DISPATCH");
//            Scope * childScope = objectFunctionTable->prepareChildScope(functionName, arguments);
//            
//            if (childScope != nullptr) {
//                objectRuntimeStack->push(childScope);
//                
//                // invoke function :D
//                Function * function = objectFunctionTable->retrieve(functionName);
//                
//                if (function != nullptr) {
//                    Parser subroutineParser = *new Parser(function->getCode(), objectScope, objectRuntimeStack, objectFunctionTable, new ClassTable(), new ObjectTable());
//                    
//                    subroutineParser.parse();
//                    
//                    // retrieve return value
//                    std::string returnValueID = (function->getReturnValue())->getValue();
//                    
//                    returnValue = (objectRuntimeStack->top())->retreive(returnValueID);
//                    
//                }
//                
//                objectRuntimeStack->pop();
//            }
        }

        return returnValue;
    }
    
    Function * createObjectChildScope(std::string methodName, std::list<Value *> arguments, Scope * objGlobalScope) {
        Scope * objectChildScope = objectFunctionTable->prepareObjectChildScope(methodName, arguments, objGlobalScope);
        
        if (objectChildScope != nullptr) {
            objectRuntimeStack->push(objectChildScope);
            return getMethod(methodName);
        }
        
        return nullptr;
    }

//    Scope * prepareObjectChildScope(std::string functionName, std::list<Value *> arguments) {
//        return objectFunctionTable->prepareChildScope(functionName, arguments);
//    }
//    
//    void pushObjectChildScope(Scope * objectChildScope) {
//        objectRuntimeStack->push(objectChildScope);
//    }
    
    Function * getMethod(std::string methodName) {
        if (containsMethod(methodName)) {
            return objectFunctionTable->retrieve(methodName);
        }
        return nullptr;
    }
    
    Scope * getScope() {
        return objectScope;
    }
    std::stack<Scope *> * getRuntimeStack() {
        return objectRuntimeStack;
    }
    ObjectFunctionTable * getFunctionTable() {
        return objectFunctionTable;
    }
    
    void destroyObjectChildScope() {
        if (objectRuntimeStack->size() > 0) {
            objectRuntimeStack->pop();
        }
    }
};

#endif /* Object_h */
