//
//  Parser.cpp
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#include <cmath>

#include "Parser.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::stoi;
using std::stof;

Parser::Parser(std::list<Token> _tokens, Scope * _globalScope, std::stack<Scope *> * _runtimeStack, FunctionTable * _functionTable,
               ClassTable * _classTable, ObjectTable * _objectTable) {
    tokens = _tokens;
    
    if (tokens.size() == 0) {
        cout << "Token list empty, unable to parse" << endl;
    }
    else if (tokens.size() == 1) {
        currentToken = lookahead1 = lookahead2 = tokens.begin();
    }
    else if(tokens.size() == 2) {
        currentToken = tokens.begin();
        lookahead1 = tokens.begin();
        lookahead1++;
        lookahead2 = tokens.begin();
        lookahead2++;
    }
    else {
        currentToken = tokens.begin();
        lookahead1 = tokens.begin();
        lookahead1++;
        lookahead2 = tokens.begin()++;
        lookahead2++;
        lookahead2++;
    }
    
    nested_if_ID = 0;
    
    parseTree = "";
    
    lastValue = nullptr;
    
    globalScope = _globalScope;
    runtimeStack = _runtimeStack;
    functionTable = _functionTable;
    
    classTable = _classTable;
    objectTable = _objectTable;
}

Parser::~Parser() {
    if (lastValue != nullptr) {
        delete lastValue;
    }
}

void Parser::progressLookaheads() {
    if (currentToken->category() != "end_input") {
        currentToken++;
    }
    if (lookahead1->category() != "end_input") {
        lookahead1++;
    }
    if (lookahead2->category() != "end_input") {
        lookahead2++;
    }
//    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
}

void Parser::parse() {
    program();
    
    if (syntaxError == false) {
//        cout << "------------\n PARSE TREE\n------------" << endl;
//        cout << parseTree << '\n' << endl;
        
        if (lastValue != nullptr) {
            if (lastValue->getType() == "Variable" || lastValue->getType() == "Reference") {
                Value * var = globalScope->retreive(lastValue->getValue());
                
                if (var != nullptr) {    // successfully retrieved initialized variable
                    if (lastValue->getType() == "Variable") {
                        cout << var->getValue() << endl;
                    }
                    
                    if (lastValue->getType() == "Reference") {
                        cout << var << endl;
                    }
                }
                
            } else {
                cout << lastValue->getValue() << endl;
            }
        }
    } else {
        cout << "Syntax error!" << endl;
    }
}

void Parser::program() {
//    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
    
    parseTree += "[program ";
    
    HLA_list();
    endInput();
    
    cout << "Current Token after program: " << currentToken->category() << endl;
    
    if (currentToken->category() != "end_input") {
        syntaxError = true;
    } else {
        parseTree += "]";
    }
}

void Parser::HLA_list() {
    bool openBracket = false;
    
    // case: empty program
    if (currentToken->category() != "end_input" && currentToken->category() != "end_class") {
        parseTree += "[HLA-list ";
        openBracket = true;
    }
    
    while (true) {
        if(currentToken->category() == "end_input" || currentToken->category() == "end_class") {
            break;
        }
        parseTree += "[HLA ";
        HLA();
        parseTree += "]";
    }
    
    if (openBracket == true) {
        parseTree += "]";
    }
    
    endClass();
}

void Parser::endInput() {
    if (currentToken->category() == "end_input") {
        parseTree += "[end-input $]";
    }
}

void Parser::HLA() {
    if (currentToken->category() == "class_key") {
        classDefinition();
    }
    else if (currentToken->category() == "function_key") {
        functionDefinition();
    }
    else {
        statementList();
    }
}

void Parser::endClass() {
    if (currentToken->category() == "end_class") {
        parseTree += "[end-class end_class]";
        progressLookaheads();
    }
}

void Parser::classDefinition() {
    Class * newClass = nullptr;
    bool classValid = true;
    
    if (currentToken->category() == "class_key") {
        parseTree += "[class-definition";
        
        Value * key = classDefinitionKey();
        
        Value * className = variable();
        
        if (key == nullptr || className == nullptr) {
            classValid = false;
        }
        
        if (currentToken->category() == "inheritance") {
            inheritance();
            parentLink();
            variable();
            
            // do classValid checks!
        }
        
        if (classValid) {
            newClass = new Class();
            
            while (currentToken->category() != "end_class") {
                
                if (currentToken->category() == "Variable") {
                    defineClassMember(newClass);
                }
                
                if (currentToken->category() == "function_key") {
                    defineClassMethod(newClass);
                }
            }
        }
        
        if (currentToken->category() == "end_class") {
            classTable->define(className->getValue(), newClass);
        }
        
        // does nothing else but step over endClass
        HLA_list();
        parseTree += "]";
    }
}

Value * Parser::classDefinitionKey() {
    Value * key = nullptr;
    
    if (currentToken->category() == "class_key") {
        parseTree += "[classdef-key classdef] ";
        key = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
        
    }
    
    return key;
}

Value * Parser::variable() {
    Value * identifier = nullptr;
    
    if (currentToken->category() == "Variable") {
        parseTree += "[Variable " + currentToken->charSequence() + "]";
        identifier = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return identifier;
}

void Parser::inheritance() {
    if (currentToken->category() == "inheritance") {
        parseTree += "[inheritance inherits]";
        progressLookaheads();
    }
}

void Parser::parentLink() {
    if (currentToken->category() == "parent_link") {
        parseTree += "[parent_link from]";
        progressLookaheads();
    }
}

void Parser::defineClassMember(Class * newClass) {
    Value * memberName = variable();
    
    Value * op = assignmentOperator();
    
    Value * memberValue = condition();
    
    Value * endOfAssignment = semicolon();
    
    if (memberName != nullptr && op != nullptr && memberValue != nullptr && endOfAssignment != nullptr) {
        newClass->defineMember(memberName->getValue(), *memberValue);
        
//        cout << memberName->getValue() << " " << (newClass->getMembers())[memberName->getValue()].getValue() << endl;
    }
    
}

void Parser::defineClassMethod(Class * newClass) {
    Value * key = functionDefinitionKey();
    Value * returnValue = variable();
    Value * op = assignmentOperator();
    Value * methodName = variable();
    Value * openParen = openParenthesis();
    
    std::list<Value *> params = parameterList();
    
    Value * closeParen = closeParenthesis();
    std::list<Token> code = *new std::list<Token>();
    
    if (key != nullptr && returnValue != nullptr && op != nullptr && methodName != nullptr
        && openParen != nullptr && closeParen != nullptr) {
        
        while (currentToken->category() != "end_function") {
            //            cout << currentToken->charSequence() << " " << currentToken->category() << endl;
            
            code.push_back(*new Token(currentToken->charSequence(), currentToken->category()));
            progressLookaheads();
        }
        code.push_back(*new Token("$", "end_input"));
        newClass->defineMethod(methodName->getValue(), *new Function(params, code, returnValue));
    }
    
    statementList();    // does nothing else but step over endFunction
}

void Parser::functionDefinition() {
    Value * key = functionDefinitionKey();
    Value * returnValue = variable();
    Value * op = assignmentOperator();
    Value * functionName = variable();
    Value * openParen = openParenthesis();
    
    std::list<Value *> params = parameterList();
    
    Value * closeParen = closeParenthesis();
    std::list<Token> code = *new std::list<Token>();
    
    if (key != nullptr && returnValue != nullptr && op != nullptr && functionName != nullptr
        && openParen != nullptr && closeParen != nullptr) {
        
        while (currentToken->category() != "end_function") {
//            cout << currentToken->charSequence() << " " << currentToken->category() << endl;
            
            code.push_back(*new Token(currentToken->charSequence(), currentToken->category()));
            progressLookaheads();
        }
        code.push_back(*new Token("$", "end_input"));
        
        functionTable->define(functionName->getValue(), new Function(params, code, returnValue));
    }
    
    statementList();    // does nothing else but step over endFunction
}

Value * Parser::functionDefinitionKey() {
    Value * key = nullptr;
    
    if (currentToken->category() == "function_key") {
        parseTree += "[fundef-key fundef]";
        key = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return key;
}

Value * Parser::assignmentOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "assignment") {
        parseTree += "[assignment-operator =]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return op;
}

Value * Parser::openParenthesis() {
    Value * openParen = nullptr;
    
    if (currentToken->category() == "open_parenthesis") {
        parseTree += "[open-parenthesis (]";
        openParen = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return openParen;
}

std::list<Value *> Parser::parameterList() {
    std::list<Value *> parameterList;
    
    if (currentToken->category() != "close_parenthesis") {
        parseTree += "[parameter-list ";
        Value * param = reference();
        
        if (param != nullptr) {
            parameterList.push_back(param);
        }
        
        else {
            cout << "Parameter unsucessfully retrieved" << endl;
        }
        
        while (currentToken->category() != "close_parenthesis") {
            Value * link = parameterLink();
            
            if (link != nullptr) {
                Value * param = reference();
                if (param != nullptr) {
                    parameterList.push_back(param);
                }
                
                else {
                    cout << "Parameter unsucessfully retrieved" << endl;
                }
            }
        }
        parseTree += "]";
    }
    
    return parameterList;
}

Value * Parser::reference() {
    Value * result = nullptr;
    
    parseTree += "[reference ";
    Value * op = addressOfOperator();
    Value * var = variable();
    parseTree += "]";
    
    if (var != nullptr) {
        if (op != nullptr) {
            // typecast to Reference!
            var->set(var->getValue(), "Reference");
        }
        // stay vanilla variable
        result = var;
    }
    
    return result;
}

Value * Parser::addressOfOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "address_of") {
        parseTree += "[address-of &]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return op;
}

Value * Parser::parameterLink() {
    Value * link = nullptr;
    
    if (currentToken->category() == "parameter_link") {
        parseTree += "[parameter-link ,]";
        link = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return link;
}

Value * Parser::closeParenthesis() {
    Value * closeParen = nullptr;
    
    if (currentToken->category() == "close_parenthesis") {
        parseTree += "[close-parenthesis )]";
        closeParen = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return closeParen;
}

void Parser::statementList() {
    bool openBracket = false;
    
    // case: empty program
    if (currentToken->category() != "end_input"
        && currentToken->category() != "end_class"
        && currentToken->category() != "end_function"
        && currentToken->category() != "else_key"
        && currentToken->category() != "end_if"
        && currentToken->category() != "end_while")  {
        
        parseTree += "[statement-list ";
        openBracket = true;
    }
    
    while (true) {
        if(currentToken->category() == "end_input"
           || currentToken->category() == "class_key"
           || currentToken->category() == "function_key"
           || currentToken->category() == "end_class"
           || currentToken->category() == "end_function"
           || currentToken->category() == "else_key"
           || currentToken->category() == "end_if"
           || currentToken->category() == "end_while") {
            break;
        }
        
        //        cout << currentToken->charSequence() << " " << currentToken->category() << endl;
        parseTree += "[statement ";
        statement();
        parseTree += "]";
    }
    
    if (openBracket == true) {
        parseTree += "]";
    }
    
    endFunction();
    end_if();
    endWhile();
}

void Parser::endFunction() {
    if (currentToken->category() == "end_function") {
        parseTree += "[end-function endfun]";
        progressLookaheads();
    }
}

void Parser::end_if() {
    if (currentToken->category() == "end_if") {
        parseTree += "[end-if endif]";
        progressLookaheads();
        nested_if_ID--;
    }
}

void Parser::endWhile() {
    if (currentToken->category() == "end_while") {
        parseTree += "[end-while endwhile]";
        progressLookaheads();
    }
}

void Parser::statement() {
    
    Value * statement = nullptr;
    
    if (lookahead1->category() == "assignment" || lookahead2->category() == "assignment") {
        parseTree += "[assignment ";
        assignment();
        parseTree += "]";
    }
    
    else if (currentToken->category() == "if_key" || currentToken->category() == "while_key") {
        parseTree += "[control ";
        control();
        parseTree += "]";
    }
    
    else {
        parseTree += "[condition ";
        statement = condition();
        parseTree += "]";
    }
    
//    if (lastValue != nullptr) {
//        delete lastValue;
//    }
    
    if (statement != nullptr) {
        lastValue = statement;
    }
}

void Parser::assignment() {
    Value * ref = reference();
    Value * op = assignmentOperator();
    Value * valRHS = nullptr;
    std::list<Value> listRHS;
    Object * objRHS = nullptr;
    
    bool instantiatingObj = false;
    bool instantiatingList = false;
    
    if (ref != nullptr && op != nullptr) {  // then parse RHS
        if (currentToken->category() == "instantiation") {
            instantiatingObj = true;
            parseTree += "[instantiation ";
            objRHS = instantiation(ref->getValue());
            
            parseTree += "]";
        }
        
        else if (currentToken->category() == "start_list") {
            
            instantiatingList = true;
            parseTree += "[list ";
            listRHS = list();
            
            parseTree += "]";
        }
        
        else {
            parseTree += "[condition ";
            valRHS = condition();
            
            parseTree += "]";
        }
    }
    
    Value * endOfAssignment = semicolon();
    
    if (endOfAssignment != nullptr) {
        if (instantiatingObj == true && objRHS != nullptr) {
            bool uninitialized = true;
            
            if (!runtimeStack->empty() && (runtimeStack->top())->contains(ref->getValue())) {
                uninitialized = false;
//                cout << "Removed " << ref->getValue() << " from runtime stack!" << endl;
//                (runtimeStack->top())->remove(ref->getValue());
            }
            
            if (globalScope->contains(ref->getValue())) {
                uninitialized = false;
//                cout << "Removed " << ref->getValue() << " from global scope!" << endl;
//                globalScope->remove(ref->getValue());
            }
            
            if (uninitialized) {
                objectTable->assign(ref->getValue(), objRHS);
            } else {
                cout << "Variable with object identifier already initialized, cannot instantate object" << endl;
            }
            
        }
        
        if (instantiatingList == true) {
            
            if (!objectTable->contains(ref->getValue())) {
                
                if (!runtimeStack->empty()) {

                    (runtimeStack->top())->assignList(ref->getValue(), listRHS);
                }
                else {
                    
                    globalScope->assignList(ref->getValue(), listRHS);
                }
            } else {
                //                cout << "Removed " << ref->getValue() << " from object table!" << endl;
                //                objectTable->remove(ref->getValue());
                cout << "Object with variable identifier already instantiated, cannot initialize list" << endl;
            }
        }
        
        if (valRHS != nullptr) {
            
            if (!objectTable->contains(ref->getValue())) {
                if (!runtimeStack->empty()) {
                    (runtimeStack->top())->assignVariable(ref->getValue(), valRHS);
                }
                else {
                    globalScope->assignVariable(ref->getValue(), valRHS);
                }
            } else {
//                cout << "Removed " << ref->getValue() << " from object table!" << endl;
//                objectTable->remove(ref->getValue());
                cout << "Object with variable identifier already instantiated, cannot initialize variable" << endl;
            }
        }
    }
    else {  // change this into an exception handler!
        cout << "Semicolon required at end of assignment, ya fool!" << endl;
    }
    
 
    
}

Object * Parser::instantiation(std::string objectID) {
    Object * newObject = nullptr;
    
    Value * op = instantiationOperator();
    Value * openParen = openParenthesis();
    Value * classID = className();
    Value * closeParen = closeParenthesis();
    
    if (op != nullptr && openParen != nullptr && classID != nullptr && closeParen != nullptr) {
        Class * classBlueprint = classTable->retrieve(classID->getValue());
        
        newObject = new Object(objectID, classBlueprint);
    }
    
    if (currentToken->category() == "structure_reference") {
        structureReference();
        
        parseTree += "[function-invocation ";
        functionInvocation();
        parseTree += "]";
    }
    
    return newObject;
}

Value * Parser::instantiationOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "instantiation") {
        parseTree += "[instantiation new]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return op;
}

Value * Parser::className() {
    Value * className = nullptr;
//    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
    
    if (currentToken->category() == "ClassName") {
        parseTree += "[ClassName " + currentToken->charSequence() + "]";
        
//        cout << currentToken->charSequence() << " " << currentToken->category() << endl;
        
        string extractedClassName = "";
        for (int i = 1; i < currentToken->charSequence().length() - 1; i++) {
            extractedClassName += currentToken->charSequence()[i];
            
        }
        className = new Value(extractedClassName, "String");
        
        progressLookaheads();
    }
    
    return className;
}

Value * Parser::structureReference() {
    Value * op = nullptr;
    
    if (currentToken->category() == "structure_reference") {
        parseTree += "[structure-reference .]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}

Value * Parser::functionInvocation() {
    Value * returnValue = nullptr;
    
    if (lookahead1->category() == "structure_reference" || lookahead2->category() == "structure_reference") {
        Value * objectID = reference();
        Value * op = structureReference();
        
        if (objectID != nullptr && op != nullptr) {
            Object * object = objectTable->retrieve(objectID->getValue());
            
            if (object != nullptr) {
                Value * functionName = variable();
                Value * openParen = openParenthesis();
                
                std::list<Value *> arguments = argumentList(object);
                
                Value * closeParen = closeParenthesis();
                
                if (functionName != nullptr && openParen != nullptr && closeParen != nullptr) {
                    returnValue = object->dispatch(functionName->getValue(), arguments);
                    
                    if (returnValue->getValue() == "DYNAMIC_DISPATCH" || returnValue->getType() == "DYNAMIC_DISPATCH") {
                        Function * method  = object->createObjectChildScope(functionName->getValue(), arguments, object->getScope());
                        
                        if (method != nullptr) {    // object child scope pushed onto stack, invoke that ish!
                            Parser subroutineParser = *new Parser(method->getCode(), object->getScope(), object->getRuntimeStack(), object->getFunctionTable(), classTable, objectTable);
                            
                            subroutineParser.parse();
                            
                            std::string returnValueID = (method->getReturnValue())->getValue();
                            
                            returnValue = ((object->getRuntimeStack())->top())->retreive(returnValueID);
                            
                        }
                        object->destroyObjectChildScope();
                    }
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
                    return returnValue;
                }
            }
        }
    }

    Value * functionName = variable();
    Value * openParen = openParenthesis();
    
    std::list<Value *> arguments = argumentList();
    
    Value * closeParen = closeParenthesis();
    
    if (functionName != nullptr && openParen != nullptr && closeParen != nullptr) {
        
        if (functionName->getValue() == "print" && arguments.size() == 1) {
            functionTable->print(arguments.front());
        }
        
        else if (functionName->getValue() == "copy" && arguments.size() == 1) {
            returnValue = functionTable->copy(arguments.front());
        }
        
        else {
            Scope * childScope = functionTable->prepareChildScope(functionName->getValue(), arguments);
            
            if (childScope != nullptr) {
                runtimeStack->push(childScope);
                
                // invoke function :D
                Function * function = functionTable->retrieve(functionName->getValue());
                
                if (function != nullptr) {
                    Parser subroutineParser = *new Parser(function->getCode(), globalScope, runtimeStack, functionTable, classTable, objectTable);
                    
                    subroutineParser.parse();
                    
                    // retrieve return value
                    std::string returnValueID = (function->getReturnValue())->getValue();
                    
                    returnValue = (runtimeStack->top())->retreive(returnValueID);
                    
                }
                
                runtimeStack->pop();
            }
        }
    }
    
    return returnValue;
}

std::list<Value *> Parser::argumentList(Object * object) {
    std::list<Value *> arguments;
    
    if (currentToken->category() != "close_parenthesis") {
        parseTree += "[argument-list ";
        
        parseTree += "[condition ";
        Value * argument = nullptr;
        
        if (object != nullptr) {    // check objectScope first if the default parameter isn't nullptr
            if (object->containsMember(currentToken->charSequence())) {
                
                argument = variable();
            }
        }
        
        if (argument == nullptr) {
            argument = condition(false, true);
        }
        
        if (argument != nullptr) {
            arguments.push_back(argument);
        }
        
        else {
            cout << "Argument unsucessfully retrieved" << endl;
        }
        
        parseTree += "]";
        
        while (currentToken->category() != "close_parenthesis") {
            Value * link = parameterLink();
            
            if (link != nullptr) {
                parseTree += "[condition ";
                Value * argument = condition(false, true);
                
                if (argument != nullptr) {
                    arguments.push_back(argument);
                }
                
                else {
                    cout << "Argument unsucessfully retrieved" << endl;
                }
                parseTree += "]";
            }
        }
        parseTree += "]";
    }
    
    return arguments;
}

std::list<Value> Parser::list() {
    Value * start = startList();
    
    std::list<Value> elements = listElements();
    
    Value * end = endList();
    
    return elements;
}

Value * Parser::startList() {
    Value * start = nullptr;
    
    if (currentToken->category() == "start_list") {
        parseTree += "[start-list {]";
        start = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return start;
}

std::list<Value> Parser::listElements() {
    std::list<Value> elements = *new std::list<Value>();
    
    if (currentToken->category() != "end_list") {
        parseTree += "[list-elements ";
        
        Value * element = nullptr;
        
        if (currentToken->category() == "instantiation") {
            parseTree += "[instantiation ";
//            instantiation();
            parseTree += "]";
        }
        else if (currentToken->category() == "start_list") {
            parseTree += "[list ";
//            list();
            parseTree += "]";
        }
        else {
            parseTree += "[condition ";
            element = condition();
            parseTree += "]";
            
            if (element != nullptr) {
                
                elements.push_back(*element);
                
                
            }
            else {
                cout << "List element unsucessfully retrieved" << endl;
            }
        }
        
        while (currentToken->category() != "end_list") {
            Value * link = parameterLink();
            
            if (link != nullptr) {
                Value * element = nullptr;
                
                if (currentToken->category() == "instantiation") {
                    parseTree += "[instantiation ";
//                    instantiation();
                    parseTree += "]";
                }
                else if (currentToken->category() == "start_list") {
                    parseTree += "[list ";
//                    list();
                    parseTree += "]";
                }
                else {
                    parseTree += "[condition ";
                    element = condition();
                    parseTree += "]";
                    
                    if (element != nullptr) {
                        elements.push_back(*element);
                        
                    }
                    else {
                        cout << "List element unsucessfully retrieved" << endl;
                    }
                }
            }
        }
        
        parseTree += "]";
    }
    
    return elements;
}

Value * Parser::endList() {
    Value * end = nullptr;
    
    if (currentToken->category() == "end_list") {
        parseTree += "[end-list }]";
        end = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return end;
}

Value * Parser::semicolon() {
    Value * semicolon = nullptr;
    
    if (currentToken->category() == "semicolon") {
        parseTree += "[semicolon ;]";
        semicolon = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return semicolon;
}

void Parser::control() {
    if (currentToken->category() == "if_key") {
        parseTree += "[if-statement ";
        if_statement();
        parseTree += "]";
    }
    if (currentToken->category() == "while_key") {
        parseTree += "[while-loop ";
        whileLoop();
        parseTree += "]";
    }
}

void Parser::if_statement() {
    
    int local_if_ID = 0;
    bool if_condition_accepted = false;
//    bool skippingStatements = false;
    
    Value * key = if_key();
    
    if (key != nullptr) {
        local_if_ID = ++nested_if_ID;
    }
    
    Value * openParen = openParenthesis();
    
    parseTree += "[condition ";
    Value * if_condition = condition(true);
    parseTree += "]";
    
    Value * closeParen = closeParenthesis();
    
    if (key != nullptr && openParen != nullptr && if_condition != nullptr && closeParen != nullptr) {
        if (if_condition->getValue() == "1") {
            if_condition_accepted = true;
            statementList();
        }
        if (if_condition->getValue() == "0") {
            bool doneSkipping = false;
            
            while (!doneSkipping) {
                while (currentToken->category() != "end_if" && currentToken->category() != "else_key") {    // skip all that bullshit
//                    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
                    progressLookaheads();
                    
                    if (currentToken->category() == "if_key") { // encountered another if token, extend nested if
                        nested_if_ID++;
                    }
                }
                if (currentToken->category() == "end_if") {
                    nested_if_ID--;
                    
                    if (nested_if_ID == local_if_ID) {  // caught at false end_if, move on
                        progressLookaheads();
                    }
                    if (nested_if_ID == local_if_ID - 1) {  // no else after skipped if, back in previous nested_if block
                        doneSkipping = true;
                        nested_if_ID++;
                    }
                }
                if (currentToken->category() == "else_key") {
                    if (nested_if_ID > local_if_ID) { //> caught at dangling else, move on
                        progressLookaheads();
                    }
                    if (nested_if_ID == local_if_ID) {  // skipped over all of current nested if block
                        doneSkipping = true;
                    }
                }
            }
        }
    }
    
    if (currentToken->category() == "else_key" && local_if_ID == nested_if_ID) {  // parse that shit
        if (if_condition_accepted == false) {   // go through else on that nested level
            Value * key = else_key();
            
            if (key != nullptr) {
                statementList();
            }
        } else {
            bool doneSkipping = false;
            
            while (!doneSkipping) {
                while (currentToken->category() != "end_if") {    // skip all that bullshit
//                    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
                    progressLookaheads();
                    
                    if (currentToken->category() == "if_key") { // encountered another if token, extend nested if
                        nested_if_ID++;
                    }
                }
                if (currentToken->category() == "end_if") {
                    nested_if_ID--;
                    
                    if (nested_if_ID == local_if_ID - 1) {  // back in previous nested_if block
                        doneSkipping = true;
                        nested_if_ID++;
                    }
                }
            }
        }
    }
}

Value * Parser::if_key() {
    Value * key = nullptr;
    
    if (currentToken->category() == "if_key") {
        parseTree += "[if-key if]";
        key = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return key;
}

Value * Parser::else_key() {
    Value * key = nullptr;
    
    if (currentToken->category() == "else_key") {
        parseTree += "[else-key else]";
        key = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return key;
}

void Parser::whileLoop() {
    // save start of condition
    std::list<Token>::iterator conditionStart;
    std::list<Token>::iterator conditionLookahead1;
    std::list<Token>::iterator conditionLookahead2;
    
    Value * key = whileKey();
    Value * openParen = openParenthesis();
    
    if (key != nullptr && openParen != nullptr) {
        conditionStart = currentToken;
        conditionLookahead1 = lookahead1;
        conditionLookahead2 = lookahead2;
    }
    
    parseTree += "[condition ";
    Value * whileCondition = condition();
    parseTree += "]";
    Value * closeParen = closeParenthesis();
    
    if (key != nullptr && openParen != nullptr && whileCondition != nullptr && closeParen != nullptr) {
        // initailize loop
        while (whileCondition->getValue() == "1") {
            statementList();
            
            currentToken = conditionStart;
            lookahead1 = conditionLookahead1;
            lookahead2 = conditionLookahead2;
            
            whileCondition = condition();
            
            closeParen = closeParenthesis();
        }
        
        // while loop terminated but still at beginning
        while (currentToken->category() != "end_while") {
//            cout << currentToken->charSequence() << " " << currentToken->category() << endl;
            progressLookaheads();
        }
    }
}

Value * Parser::whileKey() {
    Value * key = nullptr;
    
    if (currentToken->category() == "while_key") {
        parseTree += "[while-key while]";
        key = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    return key;
}

Value * Parser::condition(bool control, bool invoking) {
    
    // DESIGN: implement integer/float and object bool evaluations (nullptr = false, address = true)
    
    Value * result = nullptr;
    
    parseTree += "[expression ";
    Value * leftExpression = expression(invoking);
    parseTree += "]";
    
    if (currentToken->category() == "close_parenthesis" && control == true) {  // evaluation of standalone expression
        if (leftExpression == nullptr) {
            return new Value("0", "Boolean");
        }
        else if (leftExpression->getType() == "Boolean") {
            if (leftExpression->getValue() == "false") {
                result = new Value("0", "Boolean");
            }
            if (leftExpression->getValue() == "true") {
                result = new Value("1", "Boolean");
            }
        }
        else if (leftExpression->getType() == "Integer" || leftExpression->getType() == "Float") {
            if (stoi(leftExpression->getValue()) == 0) {
                result = new Value("0", "Boolean");
            }
            else{
                result = new Value("1", "Boolean");
            }
        }
        else if (leftExpression->getType() == "String") {
            result = new Value("1", "Boolean");
        }
        else {
            // reserved for classes
        }
    }
    
    else {
        while (currentToken->category() == "equals" || currentToken->category() == "greater_than"
               || currentToken->category() == "less_than") {
            Value * op = conditionalOperator();
            
            parseTree += "[expression ";
            Value * rightExpression = expression(invoking);
            parseTree += "]";
            
            if (op != nullptr && rightExpression != nullptr) {
                if (op->getType() == "equals") {
                    if ((leftExpression->getType() == "Integer" && rightExpression->getType() == "Integer") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Integer" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Integer")) {
                        
                        bool evaluation = stof(leftExpression->getValue()) == stof(rightExpression->getValue());
                        
                        if (result == nullptr) {
                            result = new Value(to_string(evaluation) , "Boolean");
                        } else {
                            result->set(to_string(stoi(result->getValue()) && evaluation), "Boolean");
                        }
                    }
                    if ((leftExpression->getType() == "String" && rightExpression->getType() == "String") ||
                        (leftExpression->getType() == "Boolean" && rightExpression->getType() == "Boolean")) {
                        
                        bool evaluation = leftExpression->getValue() == rightExpression->getValue();
                        
                        if (result == nullptr) {
                            result = new Value(to_string(evaluation) , "Boolean");
                        } else {
                            result->set(to_string(stoi(result->getValue()) && evaluation), "Boolean");
                        }
                    }
                }
                if (op->getType() == "greater_than") {
                    if ((leftExpression->getType() == "Integer" && rightExpression->getType() == "Integer") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Integer" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Integer")) {
                        
                        bool evaluation = stof(leftExpression->getValue()) > stof(rightExpression->getValue());
                        
                        if (result == nullptr) {
                            result = new Value(to_string(evaluation) , "Boolean");
                        } else {
                            result->set(to_string(stoi(result->getValue()) && evaluation), "Boolean");
                        }
                    }
                }
                if (op->getType() == "less_than") {
                    if ((leftExpression->getType() == "Integer" && rightExpression->getType() == "Integer") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Integer" && rightExpression->getType() == "Float") ||
                        (leftExpression->getType() == "Float" && rightExpression->getType() == "Integer")) {
                        
                        bool evaluation = stof(leftExpression->getValue()) < stof(rightExpression->getValue());
                        
                        if (result == nullptr) {
                            result = new Value(to_string(evaluation) , "Boolean");
                        } else {
                            result->set(to_string(stoi(result->getValue()) && evaluation), "Boolean");
                        }
                    }
                }
                /*
                 Allows for proper evaluation down the chain (i.e. 3 == 3 == 5), you can't have 3 == 3 evaluate to 1 and
                 then check 1 == 5. Rather, you have to shift the right expression over one to the left. Same logic applies
                 to all other boolean statements (i.e. 1 < 5 > 2)
                 */
                if (result != nullptr) {
                    leftExpression = rightExpression;
                }
            }
        }
    }
    
    // if there was no op and RHS, then return LHS
    if (result == nullptr) {
        return leftExpression;
    }
    
    return result;
}

Value * Parser::expression(bool invoking) {
    Value * result = nullptr;
    
    parseTree += "[term ";
    Value * leftTerm = term(invoking);
    parseTree += "]";
    
    while (currentToken->category() == "addition" || currentToken->category() == "subtraction") {
        Value * op = addSubOperator();
        
        parseTree += "[term ";
        Value * rightTerm = term(invoking);
        parseTree += "]";
        
        if (op != nullptr && rightTerm != nullptr) {
            if (op->getType() == "addition") {
                if (leftTerm->getType() == "Integer" && rightTerm->getType() == "Integer") {
                    if (result == nullptr) {
                        int sum = stoi(leftTerm->getValue()) + stoi(rightTerm->getValue());
                        result = new Value(to_string(sum) , "Integer");
                    } else {
                        int sum = stoi(result->getValue()) + stoi(rightTerm->getValue());
                        result->set(to_string(sum), "Integer");
                    }
                }
                if ((leftTerm->getType() == "Float" && rightTerm->getType() == "Float") ||
                    (leftTerm->getType() == "Integer" && rightTerm->getType() == "Float") ||
                    (leftTerm->getType() == "Float" && rightTerm->getType() == "Integer")) {
                    if (result == nullptr) {
                        float sum = stof(leftTerm->getValue()) + stof(rightTerm->getValue());
                        result = new Value(to_string(sum) , "Float");
                    } else {
                        float sum = stof(result->getValue()) + stof(rightTerm->getValue());
                        result->set(to_string(sum), "Float");
                    }
                }
                if ((leftTerm->getType() == "String" && (rightTerm->getType() == "Integer" || rightTerm->getType() == "Float" || rightTerm->getType() == "String" || rightTerm->getType() == "Boolean"))
                    ||
                    (rightTerm->getType() == "String" && (leftTerm->getType() == "Integer" || leftTerm->getType() == "Float" || leftTerm->getType() == "String" || leftTerm->getType() == "Boolean"))
                    ) {
                    if (result == nullptr) {
                        string sum = leftTerm->getValue().append(rightTerm->getValue());
                        result = new Value(sum , "String");
                    } else {
                        string sum = result->getValue().append(rightTerm->getValue());
                        result->set(sum, "String");
                    }
                }
            }
            if (op->getType() == "subtraction") {
                if (leftTerm->getType() == "Integer" && rightTerm->getType() == "Integer") {
                    if (result == nullptr) {
                        int difference = stoi(leftTerm->getValue()) - stoi(rightTerm->getValue());
                        result = new Value(to_string(difference) , "Integer");
                    } else {
                        int difference = stoi(result->getValue()) - stoi(rightTerm->getValue());
                        result->set(to_string(difference), "Integer");
                    }
                }
                if ((leftTerm->getType() == "Float" && rightTerm->getType() == "Float") ||
                    (leftTerm->getType() == "Integer" && rightTerm->getType() == "Float") ||
                    (leftTerm->getType() == "Float" && rightTerm->getType() == "Integer")) {
                    if (result == nullptr) {
                        float difference = stof(leftTerm->getValue()) - stof(rightTerm->getValue());
                        result = new Value(to_string(difference) , "Float");
                    } else {
                        float difference = stof(result->getValue()) - stof(rightTerm->getValue());
                        result->set(to_string(difference), "Float");
                    }
                }
            }
            // allows for proper type checking when concatenating a bunch of different types
            if (result != nullptr) {
                leftTerm = result;
            }
        }
    }
    
    // if there was no op and RHS, then return LHS
    if (result == nullptr) {
        return leftTerm;
    }
    
    return result;
}

Value * Parser::term(bool invoking) {
    Value * result = nullptr;
    
    parseTree += "[factor";
    Value * leftFactor = factor(invoking);
    parseTree += "]";
    
    while (currentToken->category() == "multiplication" || currentToken->category() == "division") {
        Value * op = multiDivOperator();
        
        parseTree += "[factor";
        Value * rightFactor = factor(invoking);
        parseTree += "]";
        
        if (op != nullptr && rightFactor != nullptr) {
            if (op->getType() == "multiplication") {
                if (leftFactor->getType() == "Integer" && rightFactor->getType() == "Integer") {
                    if (result == nullptr) {
                        int product = stoi(leftFactor->getValue()) * stoi(rightFactor->getValue());
                        result = new Value(to_string(product) , "Integer");
                    } else {
                        int product = stoi(result->getValue()) * stoi(rightFactor->getValue());
                        result->set(to_string(product), "Integer");
                    }
                }
                if ((leftFactor->getType() == "Float" && rightFactor->getType() == "Float") ||
                    (leftFactor->getType() == "Integer" && rightFactor->getType() == "Float") ||
                    (leftFactor->getType() == "Float" && rightFactor->getType() == "Integer")) {
                    if (result == nullptr) {
                        float product = stof(leftFactor->getValue()) * stof(rightFactor->getValue());
                        result = new Value(to_string(product) , "Float");
                    } else {
                        float product = stof(result->getValue()) * stof(rightFactor->getValue());
                        result->set(to_string(product), "Float");
                    }
                }
            }
            if (op->getType() == "division") {
                if ((leftFactor->getType() == "Integer" && rightFactor->getType() == "Integer") ||
                    (leftFactor->getType() == "Float" && rightFactor->getType() == "Float") ||
                    (leftFactor->getType() == "Integer" && rightFactor->getType() == "Float") ||
                    (leftFactor->getType() == "Float" && rightFactor->getType() == "Integer")) {
                    
                    if (result == nullptr) {
                        float quotient = stof(leftFactor->getValue()) / stof(rightFactor->getValue());
                        result = new Value(to_string(quotient) , "Float");
                    } else {
                        float quotient = stof(result->getValue()) / stof(rightFactor->getValue());
                        result->set(to_string(quotient), "Float");
                    }
                }
            }
            // allows for proper type checking when concatenating a bunch of different types
            if (result != nullptr) {
                leftFactor = result;
            }
        }
    }
    
    // if there was no op and RHS, then return LHS
    if (result == nullptr) {
        return leftFactor;
    }
    
    return result;
}

Value * Parser::factor(bool invoking) {
    Value * result = nullptr;
    
    parseTree += "[negative ";
    Value * base = negative(invoking);
    parseTree += "]";
    
    Value * pow = power(new Value("1", "Float"));
    
    if (pow->getValue() != "1") {
        float product = powf(stof(base->getValue()), stof(pow->getValue()));
        result = new Value(to_string(product), "Float");
    }
    
    if (result == nullptr) {
        return base;
    }
    
    return result;
}

Value * Parser::power(Value * passedPower) {
    Value * op = exponentOperator();
    
    // try to grab recursed powers that will multiply to initialized power = 1
    if (op != nullptr) {
        Value * recursedPower = expression();
        
        // if you have the operator and a parsed recurssed power, recurse further
        if (recursedPower != nullptr) {
            passedPower->set(to_string(stof(passedPower->getValue()) * stof(power(recursedPower)->getValue())), "Float");
        }
    }
    
    // else, base case
    return passedPower;
    
//    Value * recursedPower = Integer();
//    
//    if (recursedPower == nullptr) {
//        recursedPower = Float();
//    }
//    
//    if (recursedPower == nullptr) {
//        recursedPower = variable();
//    }
    
}

Value * Parser::negative(bool invoking) {
    Value * result = nullptr;
    
    Value * op = negation();
    
    parseTree += "[parentheses ";
    result = parentheses(invoking);
    parseTree += "]";
    
    if (op != nullptr && result != nullptr) {
        result->set(to_string(-1 * stof(result->getValue())), "Float");
    }
    
    return result;
}

Value * Parser::parentheses(bool invoking) {
    Value * openParen = nullptr;
    Value * quantity = nullptr;
    Value * closeParen = nullptr;
    
    if (currentToken->category() == "open_parenthesis") {
        openParen = openParenthesis();
        quantity = expression(invoking);
        closeParen = closeParenthesis();
        
        if (openParen == nullptr || quantity == nullptr || closeParen == nullptr) {
            // THROW A FUCKING ERROR PLEASE :D
            cout << "ERROR: parentheses quantity failed to parse! :(" << endl;
        }
    }
    
    else {
        parseTree += "[operand ";
        quantity = operand();
        parseTree += "]";
    }
    
    return quantity;
}

Value * Parser::operand(bool invoking) {
    Value * operand = nullptr;
    
//    cout << currentToken->charSequence() << " " << currentToken->category() << endl;
//    cout << lookahead1->charSequence() << " " << lookahead1->category() << endl;
    
    if (lookahead1->category() == "structure_reference" || lookahead1->category() == "open_parenthesis") {
        parseTree += "[function-invocation ";
        operand = functionInvocation();
        parseTree += "]";
    }
    
    else if (currentToken->category() == "instantiation") {
        parseTree += "[instantiation ";
//        instantiation();
        parseTree += "]";
    }
    
    else if ((currentToken->category() == "Variable" || currentToken->category() == "Reference") && lookahead1->category() == "start_list") {
        
    }
    
    else if (currentToken->category() == "address_of" || currentToken->category() == "Variable") {
        /*
         THINK ABOUT THIS IN THE CONTEXT OF OPERANDS!!! IT'S VERY DIFFERENT THAN THE SEMANTICS REGARDING
         LASTVALUE AND RETURNING EITHER THE ADDRESS OR THE VALUE. IN THIS CONTEXT, YOU NEED TO PRESERVE
         */
        
        operand = reference();
        
        if (operand != nullptr) {   // reference or variable sucessfully parsed
            
            if (operand->getType() == "Reference" && invoking == true) {    // then retrieve shallow copy to return
                Value * arg = nullptr;
                
                if (!runtimeStack->empty()) {
                    arg = (runtimeStack->top())->retreive(operand->getValue());
                }
                    
                if (arg == nullptr) {   // failed retrieval from local scope, check global scope
                    arg = globalScope->retreive(operand->getValue());
                }
                    
                if (arg != nullptr) {   // successfully retrieved initialized variable
                    operand = arg;
                    operand->configureReference(true);
                }
                    
                else {  // failed to retrieve initialized variable
                    operand = nullptr;
                }
            }
            
            if (operand->getType() == "Variable") {
                Value * var = nullptr;
                
                if (!runtimeStack->empty()) {
                    var = (runtimeStack->top())->retreive(operand->getValue());
                }
                
                if (var == nullptr) {   // failed retrieval from local scope, check global scope
                    var = globalScope->retreive(operand->getValue());
                }
                
                if (var != nullptr) {   // successfully retrieved initialized variable
                    
                    if (invoking == true) {
                        operand = new Value(var->getValue(), var->getType());   // then prepare deep copy to return
                    }
                    
                    else {
                        operand = var;
                    }
                    
                    operand->configureReference(false);
                }
                
                else {  // failed to retrieve initialized variable
                    operand = nullptr;
                }
            }
        }
    }
    
    else {
        if (operand == nullptr) {
            operand = Integer();
        }
        if (operand == nullptr) {
            operand = Float();
        }
        if (operand == nullptr) {
            operand = String();
        }
        if (operand == nullptr) {
            operand = Boolean();
        }
    }
    
    return operand;
}

Value * Parser::Integer() {
    Value * Integer = nullptr;
    
    if (currentToken->category() == "Integer") {
        parseTree += "[Integer " + currentToken->charSequence() + "]";
        Integer = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return Integer;
}

Value * Parser::Float() {
    Value * Float = nullptr;
    
    if (currentToken->category() == "Float") {
        parseTree += "[Float " + currentToken->charSequence() + "]";
        Float = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }

    return Float;
}

Value * Parser::String() {
    Value * String = nullptr;
    
    if (currentToken->category() == "String") {
        parseTree += "[String " + currentToken->charSequence() + "]";
        
        string extractedString = "";
        for (int i = 1; i < currentToken->charSequence().length() - 1; i++) {
            extractedString += currentToken->charSequence()[i];
        }
        
        String = new Value(extractedString, currentToken->category());
        progressLookaheads();
    }
    
    return String;
}

Value * Parser::Boolean() {
    Value * Boolean = nullptr;
    
    if (currentToken->category() == "Boolean") {
        parseTree += "[Boolean " + currentToken->charSequence() + "]";
        Boolean = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return Boolean;
}

Value * Parser::conditionalOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "equals") {
        parseTree += "[equals ==]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    if (currentToken->category() == "greater_than") {
        parseTree += "[greater-than >]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    if (currentToken->category() == "less_than") {
        parseTree += "[less-than <]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}

Value * Parser::addSubOperator() {
    Value * op = nullptr;

    if (currentToken->category() == "addition") {
        parseTree += "[addition +]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    if (currentToken->category() == "subtraction") {
        parseTree += "[subtraction -]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}

Value * Parser::multiDivOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "multiplication") {
        parseTree += "[multiplication *]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    if (currentToken->category() == "division") {
        parseTree += "[division /]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}

Value * Parser::exponentOperator() {
    Value * op = nullptr;
    
    if (currentToken->category() == "exponent") {
        parseTree += "[exponent ^]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}

Value * Parser::negation() {
    Value * op = nullptr;
    
    if (currentToken->category() == "subtraction") {
        parseTree += "[subtraction -]";
        op = new Value(currentToken->charSequence(), currentToken->category());
        progressLookaheads();
    }
    
    return op;
}
