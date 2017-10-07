//
//  Function.h
//  RuM
//
//  Created by Marc Hosecloth on 11/30/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Function_h
#define Function_h

#include <list>

#include "Value.h"
#include "Token.h"

class Function {
    std::list<Value *> parameterList;
    std::list<Token> code;
    Value * returnValue;
    
public:
    Function() {
        parameterList = *new std::list<Value *>();
        code = *new std::list<Token>();
        returnValue = nullptr;
    }
    Function(std::list<Value *> _parameterList, std::list<Token> _code, Value * _returnValue) {
        parameterList = _parameterList;
        code = _code;
        returnValue = _returnValue;
    }
    
    std::list<Value *> getParameterList() {
        return parameterList;
    }
    
    std::list<Token> getCode() {
        return code;
    }
    
    Value * getReturnValue() {
        return returnValue;
    }
};

#endif /* Function_h */
