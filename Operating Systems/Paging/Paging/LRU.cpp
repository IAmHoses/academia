//
//  LRU.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "LRU.hpp"

#include <iostream>

LRU::LRU(const int* RAMframes, bool verbose) {
    LRU::RAMframes = RAMframes;
    LRU::verbose = verbose;
    accesses = misses = writes = drops = 0;
    
    nextPageNumber = 0;
    initializeMemory();
}

void LRU::initializeMemory() {
    while (!framesNotFull()) {
        Page* newPage = new Page(nextPageNumber++);
        addToPageTable(newPage);
        writeToDisk(newPage);
    }
}

bool LRU::framesNotFull() {
    return pageTable.size() == *LRU::RAMframes;
}

void LRU::addToPageTable(Page* page) {
    if (!framesNotFull()) {
        pageTable.insert(std::make_pair(page->number(), page));
        pageStack.push(page);
        updateLeastRecentlyUsed(page);
    }
    else {
        std::cout << "WARNING: attempted to add page to full page table" << std::endl;
    }
}

void LRU::writeToDisk(Page* page) {
    if (!onDisk(page)) {
        disk.insert(std::make_pair(page->number(), page));
    } else {
        if (page->isDirty()) {
            disk[page->number()] = page;
            writes++;
        }
    }
}

bool LRU::onDisk(Page* page) {
    std::unordered_map<int, Page*>::const_iterator got = disk.find(page->number());
    
    if (got == disk.end()) {
        return false;
    }
    
    return true;
}

void LRU::simulate() {
    
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
    
    std::cout << "\n---------------------------------------\n\tOUTPUT STATISTICS (LRU)\t\n---------------------------------------\n" << std::endl;
    std::cout << "Number of memory accesses: " << accesses << std::endl;
    std::cout << "Number of misses: " << misses << std::endl;
    std::cout << "Number of writes: " << writes << std::endl;
    std::cout << "Number of drops: " << drops << std::endl;
}

void LRU::updateLeastRecentlyUsed(Page* mostRecentlyUsed) {
    /* 
     idea: always put most recently used at bottom of stack, then
            that way, the least recently used is always at top of stack
    */
    
    Page* MRU = nullptr;
    
    while (pageStack.top() != mostRecentlyUsed) {   // find MRU
        auxillaryStack.push(pageStack.top());
        pageStack.pop();
    }
    
    if (!pageStack.empty()) {   // save MRU
        MRU = pageStack.top();
        pageStack.pop();
    }
    
    while (!pageStack.empty()) {    // pour rest of page stack into aux
        auxillaryStack.push(pageStack.top());
        pageStack.pop();
    }
    
    if (MRU != nullptr) {
        pageStack.push(MRU);    // put MRU on bottom of page stack
    } else {
        std::cout << "WARNING: failure to update least recently used" << std::endl;
    }
    
    while (!auxillaryStack.empty()) {   // pour aux stack back to page stack
        pageStack.push(auxillaryStack.top());
        auxillaryStack.pop();
    }
}

void LRU::performRead(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        Page* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            freePage->addMapping(virtualAddress, virtualAddress);
            updateLeastRecentlyUsed(freePage);
            
        } else {
            if (onDisk(virtualAddress)) {
                swapPage(virtualAddress);
            } else {
                evictPage();
                
                freePage = getFirstFreePageOnDisk();
                
                if (freePage != nullptr) {
                    freePage->addMapping(virtualAddress, virtualAddress);
                    addToPageTable(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    Page* newPage = new Page(nextPageNumber++, virtualAddress,virtualAddress);
                    
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

bool LRU::pageFault(std::string virtualAddress) {
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {    // then hit
            return false;
        } else {
            misses++;
        }
    }
    return true;
}

Page* LRU::getFirstFreePageInMemory() {
    Page* freePage = nullptr;
    
    for (const auto& pair : pageTable) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

Page* LRU::getFirstFreePageOnDisk() {
    Page* freePage = nullptr;
    
    for (const auto& pair : disk) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

bool LRU::onDisk(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            return true;
        }
    }
    return false;
}

// MARK THE NEW PAGE AS CLEAN ONCE IT COMES INTO MAIN MEMORY

void LRU::swapPage(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            movePageToDisk();
            
            Page* overwritingPage = pair.second;
            overwritingPage->markClean();
            addToPageTable(overwritingPage);
            
            if (verbose) {
                std::cout << pair.second << std::endl;
            }
        }
    }
}

void LRU::movePageToDisk() {
    Page* evictedPage = evictPage();
    
    if (evictedPage->isDirty()) {
        writeToDisk(evictedPage);
    } else {
        drops++;
    }
}

Page* LRU::evictPage() {
    Page* leastRecentlyUsed = pageStack.top();
    pageStack.pop();
    
    pageTable.erase(leastRecentlyUsed->number());
    
    if (verbose) {
        std::cout << leastRecentlyUsed << " swapped by ";
    }
    
    return leastRecentlyUsed;
}

void LRU::performWrite(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        Page* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            // marking dirty simulates write
            freePage->addMapping(virtualAddress, virtualAddress);
            updateLeastRecentlyUsed(freePage);
            
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
                    addToPageTable(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    Page* newPage = new Page(nextPageNumber++, virtualAddress,virtualAddress);
                    
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

void LRU::writeToPageInMemory(std::string virtualAddress, bool swapPerformed) {
    Page* pageToWriteTo = findPageInMemory(virtualAddress);
    
    if (pageToWriteTo != nullptr) {
        pageToWriteTo->markDirty();
        
        if (!swapPerformed) {
            updateLeastRecentlyUsed(pageToWriteTo);
        }
    } else {
        std::cout << "WARNING: unable to find page in memory for write" << std::endl;
    }
}

Page* LRU::findPageInMemory(std::string virtualAddress) {
    Page* page = nullptr;
    
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {
            page = pair.second;
        }
    }
    
    return page;
}
