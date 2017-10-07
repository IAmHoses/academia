//
//  Tokenizer.hpp
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <unordered_map>
#include <string>
#include <list>

#include "Token.h"

class Tokenizer {
    char * userinput;
    int inputLength;
    
    std::unordered_map<string, string> keywords;
    std::unordered_map<string, string> regexs;
    
    std::list<Token> tokens;
    bool lexicalError;
    string invalidToken;
    
public:
    Tokenizer(int input_length, char * user_input = nullptr);
    
    std::list<Token> getTokens();
    
    void printUserInput();
    void printTokens();
    
    string preprocessCode(string codeInput, bool interactiveMode);
    
    bool isKeyword(string candidate);
    bool isBoolean(string candidate);
    bool isInteger(string candidate);
    bool filterFloat(string candidate);
    bool isString(string candidate);
    bool isClassName(string candidate);
    bool isVariable(string candidate);
    
    int skipblanks(string input, int p);
    void extractTokens(string input);
    
    void tokenize(string codeInput, bool interactiveMode);
};

#endif /* Tokenizer_hpp */
