//
//  FIFO.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef FIFO_hpp
#define FIFO_hpp

#include "Page.hpp"

#include <queue>

class FIFO {
    const int* RAMframes;
    bool verbose;
    int accesses, misses, writes, drops;
    
    int nextPageNumber;
    std::unordered_map<int, Page*> pageTable;
    std::unordered_map<int, Page*> disk;
    std::queue<Page*> evictionQueue;
    
public:
    FIFO(const int* RAMframes, bool verbose);
    void initializeMemory();
    
    bool framesNotFull();
    void addToPageTable(Page* page);
    
    void writeToDisk(Page* page);
    bool onDisk(Page* page);
    
    void simulate();
    
    void performRead(std::string virtualAddress);
    bool pageFault(std::string virtualAddress);
    Page* getFirstFreePageInMemory();
    Page* getFirstFreePageOnDisk();
    bool onDisk(std::string virtualAddress);
    void swapPage(std::string virtualAddress);
    void movePageToDisk();
    Page* evictPage();
    
    void performWrite(std::string virtualAddress);
    void writeToPageInMemory(std::string virtualAddress);
    Page* findPageInMemory(std::string virtualAddress);
};

#endif /* FIFO_hpp */
