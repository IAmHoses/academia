//
//  LFU.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "LFU.hpp"

#include <iostream>

LFU::LFU(const int* RAMframes, bool verbose) {
    LFU::RAMframes = RAMframes;
    LFU::verbose = verbose;
    accesses = misses = writes = drops = 0;
    
    nextPageNumber = 0;
    initializeMemory();
}

void LFU::initializeMemory() {
    while (!framesNotFull()) {
        FrequencyPage* newPage = new FrequencyPage(nextPageNumber++);
        addToPageTable(newPage);
        writeToDisk(newPage);
    }
}

bool LFU::framesNotFull() {
    return pageTable.size() == *RAMframes;
}

void LFU::addToPageTable(FrequencyPage* page) {
    if (!framesNotFull()) {
        pageTable.insert(std::make_pair(page->number(), page));
        pageMinHeap.push(page);
    }
    else {
        std::cout << "WARNING: attempted to add page to full page table" << std::endl;
    }
}

void LFU::writeToDisk(FrequencyPage* page) {
    if (!onDisk(page)) {
        disk.insert(std::make_pair(page->number(), page));
    } else {
        if (page->isDirty()) {
            disk[page->number()] = page;
            writes++;
        }
    }
}

bool LFU::onDisk(FrequencyPage* page) {
    std::unordered_map<int, FrequencyPage*>::const_iterator got = disk.find(page->number());
    
    if (got == disk.end()) {
        return false;
    }
    
    return true;
}

void LFU::simulate() {
    
    std::string input;
    
    while (std::getline(std::cin, input)) {
        accesses++;
        
        std::string address = input.substr(0, input.length() - 2);
        std::string accessType = input.substr(input.length() - 1, 1);
        
        if (accessType == "R") {
            performRead(address);
        }
        
        if (accessType == "W") {
            performWrite(address);
        }
    }
    
    std::cout << "\n---------------------------------------\n\tOUTPUT STATISTICS (LFU)\t\n---------------------------------------\n" << std::endl;
    std::cout << "Number of memory accesses: " << accesses << std::endl;
    std::cout << "Number of misses: " << misses << std::endl;
    std::cout << "Number of writes: " << writes << std::endl;
    std::cout << "Number of drops: " << drops << std::endl;
}

void LFU::performRead(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        FrequencyPage* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            freePage->addMapping(virtualAddress, virtualAddress);
            freePage->incrementFrequency();
            
        } else {
            if (onDisk(virtualAddress)) {
                swapPage(virtualAddress);
            } else {
                evictPage();
                
                freePage = getFirstFreePageOnDisk();
                
                if (freePage != nullptr) {
                    
                    freePage->addMapping(virtualAddress, virtualAddress);
                    freePage->incrementFrequency();
                    addToPageTable(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    FrequencyPage* newPage = new FrequencyPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
                    addToPageTable(newPage);
                    writeToDisk(newPage);
                    
                    if (verbose) {
                        std::cout << newPage << std::endl;
                    }
                }
            }
        }
    }
}

bool LFU::pageFault(std::string virtualAddress) {
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {    // then hit
            return false;
        } else {
            misses++;
        }
    }
    return true;
}

FrequencyPage* LFU::getFirstFreePageInMemory() {
    FrequencyPage* freePage = nullptr;
    
    for (const auto& pair : pageTable) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

FrequencyPage* LFU::getFirstFreePageOnDisk() {
    FrequencyPage* freePage = nullptr;
    
    for (const auto& pair : disk) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

bool LFU::onDisk(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            return true;
        }
    }
    return false;
}

// MARK THE NEW PAGE AS CLEAN ONCE IT COMES INTO MAIN MEMORY

void LFU::swapPage(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            movePageToDisk();
            
            FrequencyPage* overwritingPage = pair.second;
            overwritingPage->markClean();
            overwritingPage->incrementFrequency();
            addToPageTable(overwritingPage);
            
            if (verbose) {
                std::cout << pair.second << std::endl;
            }
        }
    }
}

void LFU::movePageToDisk() {
    FrequencyPage* evictedPage = evictPage();
    
    if (evictedPage->isDirty()) {
        writeToDisk(evictedPage);
    } else {
        drops++;
    }
}

FrequencyPage* LFU::evictPage() {
    FrequencyPage* leastFrequentlyUsed = pageMinHeap.top();
    pageMinHeap.pop();
    
    pageTable.erase(leastFrequentlyUsed->number());
    
    if (verbose) {
        std::cout << leastFrequentlyUsed << " swapped by ";
    }
    
    return leastFrequentlyUsed;
}

void LFU::performWrite(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        FrequencyPage* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            // marking dirty simulates write
            freePage->addMapping(virtualAddress, virtualAddress);
            freePage->incrementFrequency();
            
        } else {
            if (onDisk(virtualAddress)) {
                swapPage(virtualAddress);
                writeToPageInMemory(virtualAddress, true);
            } else {
                evictPage();
                
                freePage = getFirstFreePageOnDisk();
                
                if (freePage != nullptr) {
                    
                    // marking dirty simulates write
                    freePage->addMapping(virtualAddress, virtualAddress);
                    freePage->incrementFrequency();
                    addToPageTable(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    FrequencyPage* newPage = new FrequencyPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
                    addToPageTable(newPage);
                    writeToDisk(newPage);
                    
                    if (verbose) {
                        std::cout << newPage << std::endl;
                    }
                }
            }
        }
    } else {
        writeToPageInMemory(virtualAddress, false);
    }
}

void LFU::writeToPageInMemory(std::string virtualAddress, bool swapPerformed) {
    FrequencyPage* pageToWriteTo = findPageInMemory(virtualAddress);
    
    if (pageToWriteTo != nullptr) {
        pageToWriteTo->markDirty();
        
        if (!swapPerformed) {
            pageToWriteTo->incrementFrequency();
        }
    } else {
        std::cout << "WARNING: unable to find page in memory for write" << std::endl;
    }
}

FrequencyPage* LFU::findPageInMemory(std::string virtualAddress) {
    FrequencyPage* page = nullptr;
    
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {
            page = pair.second;
        }
    }
    
    return page;
}
