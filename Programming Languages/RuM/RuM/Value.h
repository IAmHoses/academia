//
//  Value.h
//  RuM
//
//  Created by Marc Hosecloth on 11/23/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Value_h
#define Value_h

class Value {
    string valueString;
    string valueType;
    bool referenceConfiguration;    // used to preserve semantics after Parser::operand() retrieves from scope and overwrites type
    
public:
    Value() {
        valueString = "nullptr";
        valueType = "nullptr";
        referenceConfiguration = false;
    }
    
    Value(string valString, string valType) {
        valueString = valString;
        valueType = valType;
        referenceConfiguration = false;
    }
    
    void set(string newVal, string newType) {
        valueString = newVal;
        valueType = newType;
    }
    
    string getValue() {
        return valueString;
    }
    
    string getType() {
        return valueType;
    }
    
    void configureReference(bool config) {
        referenceConfiguration = config;
    }
    
    bool is_reference() {
        return referenceConfiguration;
    }
};

#endif /* Value_h */
