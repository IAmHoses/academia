//
//  split.h
//
//  Created by Evan Teran on 10/10/2008.
//  Copyright © 2008 Evan Teran. All rights reserved.
//

#ifndef split_h
#define split_h

#include <string>
#include <sstream>
#include <vector>

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

#endif /* split_h */
