//
//  load.h
//  RuM
//
//  Created by Marc Hosecloth on 11/20/16.
//  Copyright © 2016 Marc Hosecloth. All rights reserved.
//

#ifndef load_h
#define load_h

#include <unordered_map>
#include <fstream>

std::unordered_map<std::string, std::string> buildDictionary (std::string file) {
    std::unordered_map <std::string, std::string> keywords;
    
    std::ifstream infile;
    infile.open(file);
    
    std::string key, value;
    
    while (infile >> key >> value) {
        keywords[key] = value;
    }
    
    return keywords;
}

void populateArray(std::string file, std::string* arr) {
    
    std::ifstream infile;
    infile.open(file);
    std::string line;
    int i = 0;
    
    while (infile >> line) {
        arr[i] = line;
        i++;
    }
}

#endif /* load_h */
