//
//  Page.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef Page_hpp
#define Page_hpp

#include <unordered_map>
#include <string>

/*
    Page Base Class
 */

class Page {
    static const int PAGE_SIZE = 4048;   // 4K
    static const int PROCESSOR_BITS = 32;   // 32b
    static const int CAPACITY = PAGE_SIZE / PROCESSOR_BITS;
    
    int pageNumber;
    std::unordered_map<std::string, std::string> memoryMap;
    
    bool dirty;
    
public:
    Page(int number);
    Page(int number, std::string virtualAddress, std::string physicalAddress);
    
    int number();
    
    bool isFull();
    void addMapping(std::string virtualAddress, std::string physicalAddress);
    bool contains(std::string virtualAddress);
    
    bool isDirty();
    void markDirty();
    void markClean();
};

/*
    FrequncyPage, Child of Page Base Class
 */

class FrequencyPage : public Page {
    int frequency;
    
public:
    FrequencyPage(int number);
    FrequencyPage(int number, std::string virtualAddress, std::string physicalAddress);
    
    int getFrequency() const;
    void incrementFrequency();
};

/*
    GlobalClockPage, Child of Page Base Class
 */

class GlobalClockPage : public Page {
    bool referenceBit;
    
public:
    GlobalClockPage(int number);
    GlobalClockPage(int number, std::string virtualAddress, std::string physicalAddress);
    
    bool getReferenceBit();
    
    void setReferenceBitToOne();
    void setReferenceBitToZero();
};

/*
    OwnPage, Child of Page Base Class
 */

class OwnPage : public Page {
    int referenceInteger;
    
public:
    OwnPage(int number);
    OwnPage(int number, std::string virtualAddress, std::string physicalAddress);
    
    int getReferenceInteger();
    
    void incrementReferenceInteger();
    void decrementReferenceInteger();
};


#endif /* Page_hpp */
