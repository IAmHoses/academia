//
//  Token.h
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef Token_h
#define Token_h

using std::string;

class Token {
    string tokenCharSequence;
    string tokenCategory;
    
public:
    Token(string parsedChars, string derivedCategory) {
        tokenCharSequence = parsedChars;
        tokenCategory = derivedCategory;
    }
    
    string charSequence() {
        return tokenCharSequence;
    }
    
    string category() {
        return tokenCategory;
    }
};

#endif /* Token_h */
