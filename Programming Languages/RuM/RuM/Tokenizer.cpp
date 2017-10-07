//
//  Tokenizer.cpp
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#include <iostream>
#include <regex>

#include "Tokenizer.hpp"
#include "load.h"
#include "split.h"

using std::cout;
using std::endl;
using std::regex;
using std::regex_replace;

Tokenizer::Tokenizer(int input_length, char * user_input) {
    keywords = buildDictionary("/Users/Eidolon/Developer/Programming Languages/RuM/RuM/keywords.txt");
    tokens = *new std::list<Token>();
    
    inputLength = input_length;
    
    if (user_input != nullptr) {
        userinput = user_input;
    }
}

std::list<Token> Tokenizer::getTokens() {
    return tokens;
}

void Tokenizer::printUserInput() {
    cout << "START" << endl;
    for (int i = 0; i < 50; i++) {
        cout << userinput[i];
    }
    cout << endl << "END" << endl;
}

void Tokenizer::printTokens() {
    cout << "\n--------\n TOKENS\n--------" << endl;
    for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        cout << it->charSequence() << " (" << it->category() << ")" << endl;
    }
}


string Tokenizer::preprocessCode(string codeInput, bool interactiveMode) {
    string preprocessedOutput = "";
    
    if (interactiveMode == true) {
        for (int i = 0; i <= inputLength; i++) {
            if (isprint(userinput[i])) {
                preprocessedOutput += userinput[i];
            }
        }
    }
    
    if (interactiveMode == false) {
        preprocessedOutput = codeInput;
    }
    
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\n"), " ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\("), " ( ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\)"), " ) ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\,"), " , ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\;"), " ; ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\="), " = ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex(" \\=  \\= "), " == ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\["), " [ ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\]"), " ] ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\>"), " > ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\<"), " < ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\+"), " + ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\-"), " - ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\*"), " * ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\/"), " / ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\^"), " ^ ");
    preprocessedOutput = regex_replace(preprocessedOutput, regex("\\&"), " & ");
    
    return preprocessedOutput;
}

bool Tokenizer::isKeyword(string candidate) {
    if (keywords.find(candidate) != keywords.end()) {
        tokens.push_back(*new Token(candidate, keywords[candidate]));
        return true;
    } else {
        return false;
    }
}

bool Tokenizer::isBoolean(string candidate) {
    regex Boolean("true|false");
    
    if (regex_match(candidate, Boolean)) {
        tokens.push_back(*new Token(candidate, "Boolean"));
        return true;
    } else {
        return false;
    }
}

bool Tokenizer::isInteger(string candidate) {
    regex Integer("[0-9]+");
    
    if (regex_match(candidate, Integer)) {
        tokens.push_back(*new Token(candidate, "Integer"));
        return true;
    } else {
        return false;
    }
}

bool Tokenizer::filterFloat(string candidate) {
    regex Float("[0-9]+\\.[0-9]+");
    regex ClassReference("[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+");
    regex ReferenceInvocation("\\.[a-zA-Z0-9_]+");
    
    if (regex_match(candidate, Float)) {
        tokens.push_back(*new Token(candidate, "Float"));
        return true;
    }
    else if (regex_match(candidate, ClassReference)) {
        std::vector<string> splitCandidate = split(candidate, '.');
        tokens.push_back(*new Token(splitCandidate.at(0), "Variable"));
        tokens.push_back(*new Token(".", keywords["."]));
        tokens.push_back(*new Token(splitCandidate.at(1), "Variable"));
        return true;
    }
    else if (regex_match(candidate, ReferenceInvocation)) {
        std::vector<string> splitCandidate = split(candidate, '.');
        tokens.push_back(*new Token(".", keywords["."]));
        tokens.push_back(*new Token(splitCandidate.at(1), "Variable"));
        return true;
    }
    else {
        return false;
    }
}

bool Tokenizer::isString(string candidate) {
    regex String("\".*\"");
    
    if (regex_match(candidate, String)) {
        tokens.push_back(*new Token(candidate, "String"));
        return true;
    } else {
        return false;
    }
}

bool Tokenizer::isClassName(string candidate) {
    regex ClassName("'[a-zA-Z0-9_]+'");
    
    if (regex_match(candidate, ClassName)) {
        tokens.push_back(*new Token(candidate, "ClassName"));
        
        return true;
    } else {
        return false;
    }
}

bool Tokenizer::isVariable(string candidate) {
    regex Variable("[a-zA-Z0-9_]+");
    
    if (regex_match(candidate, Variable)) {
        tokens.push_back(*new Token(candidate, "Variable"));
        return true;
    } else {
        return false;
    }
}

int Tokenizer::skipblanks(string input, int p) {
    while (input[p] == ' ')
        ++p;
    return p;
}

void Tokenizer::extractTokens(string input) {
    bool endOfInputReached = false;
    int position = 0;
    string potentialToken = "";
    
    while (endOfInputReached != true) {
        if (input[position] == '\0') { // skip garbage user input
            position++;
            continue;
        }
        
        position = skipblanks(input, position);    // skip whitespace
        
        if (input[position] == '$') {
            tokens.push_back(*new Token(string(1, input[position]), keywords[string(1, input[position])]));
            endOfInputReached = true;
            continue;
        }
        
        if (input[position] == '"') {   // SPECIAL CASE FOR STRING
            potentialToken += input[position++];
            
            while (input[position] != '"') {
                potentialToken += input[position++];
            }
            
            potentialToken += input[position++];
        } else if (input[position] == '\'') {   // SPECIAL CASE FOR CLASS NAME
            potentialToken += input[position++];
            
            while (input[position] != '\'') {
                potentialToken += input[position++];
            }
            
            potentialToken += input[position++];
        } else {
            while (input[position] != ' ') {
                potentialToken += input[position++];
                //                cout << "State of newToken: " << potentialToken << endl;
            }
        }
        
        bool tokenAccepted = isKeyword(potentialToken);
        
        if (tokenAccepted != true) {
            tokenAccepted = isBoolean(potentialToken);
        }
        
        if (tokenAccepted != true) {
            tokenAccepted = isInteger(potentialToken);
        }
        
        if (tokenAccepted != true) {
            tokenAccepted = filterFloat(potentialToken);
        }
        
        if (tokenAccepted != true) {
            tokenAccepted = isString(potentialToken);
        }
        
        if (tokenAccepted != true) {
            tokenAccepted = isClassName(potentialToken);
        }
        
        if (tokenAccepted != true) {
            tokenAccepted = Tokenizer::isVariable(potentialToken);
        }
        
        if (tokenAccepted == false) {
            invalidToken = potentialToken;
            lexicalError = true;
            break;
        }
        
        potentialToken = "";
    }
}

void Tokenizer::tokenize(string codeInput, bool interactiveMode) {
    string preprocessedCode = preprocessCode(codeInput, interactiveMode);
    extractTokens(preprocessedCode);
    
    if (lexicalError == false) {
        printTokens();
    } else {
        if (invalidToken.length() > 0) {
            cout << "Lexical Error, invalid RuM token: " << invalidToken << endl;
        }
    }
    
    cout << endl;
    
    lexicalError = false;
}
