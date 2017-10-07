//
//  GC.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef GC_hpp
#define GC_hpp

#include "Page.hpp"

#include <list>

class GC {
    const int* RAMframes;
    bool verbose;
    int accesses, misses, writes, drops;
    
    int nextPageNumber;
//    std::unordered_map<int, GlobalClockPage*> pageTable;
    std::unordered_map<int, GlobalClockPage*> disk;
    
    std::list<GlobalClockPage*> clock;
    int clockHand;
    
public:
    GC(const int* RAMframes, bool verbose);
    void initializeMemory();
    
    bool framesNotFull();
    void addToClock(GlobalClockPage* page, bool initializing = false);
    
    void writeToDisk(GlobalClockPage* page);
    bool onDisk(GlobalClockPage* page);
    
    void rotateClockHand();
    GlobalClockPage* getPageAtClockHand();
    bool clockContains(std::string virtualAddress);
    void updateClock(std::string virtualAddress);
    
    void simulate();
    
    void performRead(std::string virtualAddress);
    bool pageFault(std::string virtualAddress);
    GlobalClockPage* getFirstFreePageInMemory();
    GlobalClockPage* getFirstFreePageOnDisk();
    bool onDisk(std::string virtualAddress);
    void swapPage(std::string virtualAddress);
    void movePageToDisk();
    GlobalClockPage* evictPage();
    
    void performWrite(std::string virtualAddress);
    void writeToPageInMemory(std::string virtualAddress);
    GlobalClockPage* findPageInMemory(std::string virtualAddress);
};

#endif /* GC_hpp */
