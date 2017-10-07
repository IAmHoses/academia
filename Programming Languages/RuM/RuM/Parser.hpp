//
//  Parser.hpp
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>

#include "Token.h"
#include "Value.h"
#include "Scope.h"
#include "FunctionTable.h"
#include "ClassTable.h"
#include "Class.h"
#include "ObjectTable.h"
#include "Object.h"

class Parser {
    std::list<Token> tokens;
    std::list<Token>::iterator currentToken;
    std::list<Token>::iterator lookahead1;
    std::list<Token>::iterator lookahead2;
    
    int nested_if_ID;
    
    string parseTree;
    bool syntaxError;
    
    Value * lastValue;
    
    Scope * globalScope;
    std::stack<Scope *> * runtimeStack;
    FunctionTable * functionTable;
    
    ClassTable * classTable;
    ObjectTable * objectTable;
    
public:
    Parser(std::list <Token> _tokens, Scope * _globalScope, std::stack<Scope *> * _runtimeStack, FunctionTable * _functionTable,
           ClassTable * _classTable, ObjectTable * _objectTable);
    ~Parser();
    
    void progressLookaheads();
    
    void parse();
    
    void program();
    void HLA_list();
    void endInput();
    void HLA();
    void endClass();
    
    void classDefinition();
    void functionDefinition();
    Value * functionDefinitionKey();
    void statementList();
    void endFunction();
    void end_if();
    void endWhile();
    
    Value * classDefinitionKey();
    Value * variable();
    void inheritance();
    void parentLink();
    void defineClassMember(Class * newClass);
    void defineClassMethod(Class * newClass);
    
    Value * assignmentOperator();
    Value * openParenthesis();
    std::list<Value *> parameterList();
    Value * reference();
    Value * addressOfOperator();
    Value * parameterLink();
    Value * closeParenthesis();
    
    void statement();
    
    void assignment();
    Object * instantiation(std::string objectID);
    Value * instantiationOperator();
    Value * className();
    Value * structureReference();
    Value * functionInvocation();
    std::list<Value *> argumentList(Object * object = nullptr);
    std::list<Value> list();
    Value * startList();
    Value * endList();
    std::list<Value> listElements();
    Value * semicolon();
    
    void control();
    void if_statement();
    Value * if_key();
    Value * else_key();
    void whileLoop();
    Value * whileKey();
    
    Value * condition(bool control = false, bool invoking = false);
    Value * expression(bool invoking = false);
    Value * term(bool invoking = false);
    Value * factor(bool invoking = false);
    Value * power(Value * passedPower);
    Value * negative(bool invoking = false);
    Value * parentheses(bool invoking = false);
    Value * operand(bool invoking = false);
    Value * Integer();
    Value * Float();
    Value * String();
    Value * Boolean();
    
    Value * conditionalOperator();
    Value * addSubOperator();
    Value * multiDivOperator();
    Value * exponentOperator();
    Value * negation();
};


#endif /* Parser_hpp */
