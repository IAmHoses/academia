//
//  OWN.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/27/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef OWN_hpp
#define OWN_hpp

#include "Page.hpp"

#include <list>

class OWN {
    const int* RAMframes;
    bool verbose;
    int accesses, misses, writes, drops;
    
    int nextPageNumber;
    //    std::unordered_map<int, OwnPage*> pageTable;
    std::unordered_map<int, OwnPage*> disk;
    
    std::list<OwnPage*> clock;
    int clockHand;
    
public:
    OWN(const int* RAMframes, bool verbose);
    void initializeMemory();
    
    bool framesNotFull();
    void addToClock(OwnPage* page, bool initializing = false);
    
    void writeToDisk(OwnPage* page);
    bool onDisk(OwnPage* page);
    
    void rotateClockHand();
    OwnPage* getPageAtClockHand();
    bool clockContains(std::string virtualAddress);
    void updateClock(std::string virtualAddress);
    
    void simulate();
    
    void performRead(std::string virtualAddress);
    bool pageFault(std::string virtualAddress);
    OwnPage* getFirstFreePageInMemory();
    OwnPage* getFirstFreePageOnDisk();
    bool onDisk(std::string virtualAddress);
    void swapPage(std::string virtualAddress);
    void movePageToDisk();
    OwnPage* evictPage();
    
    void performWrite(std::string virtualAddress);
    void writeToPageInMemory(std::string virtualAddress);
    OwnPage* findPageInMemory(std::string virtualAddress);
};

#endif /* OWN_hpp */
