//
//  LFU.hpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#ifndef LFU_hpp
#define LFU_hpp

#include "Page.hpp"

#include <queue>

class LFU {
    const int* RAMframes;
    bool verbose;
    int accesses, misses, writes, drops;
    
    int nextPageNumber;
    std::unordered_map<int, FrequencyPage*> pageTable;
    std::unordered_map<int, FrequencyPage*> disk;
    
    struct compare {
        bool operator()(const FrequencyPage* l, const FrequencyPage* r) {
            return l->getFrequency() > r->getFrequency();
        }
    };
    
    std::priority_queue<FrequencyPage*, std::vector<FrequencyPage*>, compare> pageMinHeap;
    
public:
    LFU(const int* RAMframes, bool verbose);
    void initializeMemory();
    
    bool framesNotFull();
    void addToPageTable(FrequencyPage* page);
    
    void writeToDisk(FrequencyPage* page);
    bool onDisk(FrequencyPage* page);
    
    void simulate();
    
    void performRead(std::string virtualAddress);
    bool pageFault(std::string virtualAddress);
    FrequencyPage* getFirstFreePageInMemory();
    FrequencyPage* getFirstFreePageOnDisk();
    bool onDisk(std::string virtualAddress);
    void swapPage(std::string virtualAddress);
    void movePageToDisk();
    FrequencyPage* evictPage();
    
    void performWrite(std::string virtualAddress);
    void writeToPageInMemory(std::string virtualAddress, bool swapPerformed);
    FrequencyPage* findPageInMemory(std::string virtualAddress);
};

#endif /* LFU_hpp */
