//
//  LRU.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef LRU_hpp
#define LRU_hpp

#include "Page.hpp"

#include <stack>

class LRU {
    const int* RAMframes;
    bool verbose;
    int accesses, misses, writes, drops;
    
    int nextPageNumber;
    std::unordered_map<int, Page*> pageTable;
    std::unordered_map<int, Page*> disk;
    
    std::stack<Page*> pageStack;
    std::stack<Page*> auxillaryStack;
    
public:
    LRU(const int* RAMframes, bool verbose);
    void initializeMemory();
    
    bool framesNotFull();
    void addToPageTable(Page* page);
    
    void writeToDisk(Page* page);
    bool onDisk(Page* page);
    
    void simulate();
    void updateLeastRecentlyUsed(Page* mostRecentlyUsed);
    void removeFromPageStack(Page* swappedPage);
    
    void performRead(std::string virtualAddress);
    bool pageFault(std::string virtualAddress);
    Page* getFirstFreePageInMemory();
    Page* getFirstFreePageOnDisk();
    bool onDisk(std::string virtualAddress);
    void swapPage(std::string virtualAddress);
    void movePageToDisk();
    Page* evictPage();
    
    void performWrite(std::string virtualAddress);
    void writeToPageInMemory(std::string virtualAddress, bool swapPerformed);
    Page* findPageInMemory(std::string virtualAddress);
};


#endif /* LRU_hpp */
