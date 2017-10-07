//
//  FIFO.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "FIFO.hpp"

#include <iostream>

FIFO::FIFO(const int* RAMframes, bool verbose) {
    FIFO::RAMframes = RAMframes;
    FIFO::verbose = verbose;
    accesses = misses = writes = drops = 0;
    
    nextPageNumber = 0;
    initializeMemory();
}

void FIFO::initializeMemory() {
    while (!framesNotFull()) {
        Page* newPage = new Page(nextPageNumber++);
        addToPageTable(newPage);
        writeToDisk(newPage);
    }
}

bool FIFO::framesNotFull() {
    return pageTable.size() == *RAMframes;
}

void FIFO::addToPageTable(Page* page) {
    if (!framesNotFull()) {
        pageTable.insert(std::make_pair(page->number(), page));
        evictionQueue.push(page);
    }
    else {
        std::cout << "WARNING: attempted to add page to full page table" << std::endl;
    }
}

void FIFO::writeToDisk(Page* page) {
    if (!onDisk(page)) {
        disk.insert(std::make_pair(page->number(), page));
    } else {
        if (page->isDirty()) {
            disk[page->number()] = page;
            writes++;
        }
    }
}

bool FIFO::onDisk(Page* page) {
    std::unordered_map<int, Page*>::const_iterator got = disk.find(page->number());
    
    if (got == disk.end()) {
        return false;
    }
    
    return true;
}

void FIFO::simulate() {
    
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
    
    std::cout << "\n----------------------------------------\n\tOUTPUT STATISTICS (FIFO)\t\n----------------------------------------\n" << std::endl;
    std::cout << "Number of memory accesses: " << accesses << std::endl;
    std::cout << "Number of misses: " << misses << std::endl;
    std::cout << "Number of writes: " << writes << std::endl;
    std::cout << "Number of drops: " << drops << std::endl;
}

void FIFO::performRead(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        Page* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            freePage->addMapping(virtualAddress, virtualAddress);
            
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

bool FIFO::pageFault(std::string virtualAddress) {
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {    // then hit
            return false;
        } else {
            misses++;
        }
    }
    return true;
}

Page* FIFO::getFirstFreePageInMemory() {
    Page* freePage = nullptr;
    
    for (const auto& pair : pageTable) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

Page* FIFO::getFirstFreePageOnDisk() {
    Page* freePage = nullptr;
    
    for (const auto& pair : disk) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

bool FIFO::onDisk(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            return true;
        }
    }
    return false;
}

// MARK THE NEW PAGE AS CLEAN ONCE IT COMES INTO MAIN MEMORY

void FIFO::swapPage(std::string virtualAddress) {
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

void FIFO::movePageToDisk() {
    Page* evictedPage = evictPage();
    
    if (evictedPage->isDirty()) {
        writeToDisk(evictedPage);
    } else {
        drops++;
    }
}

Page* FIFO::evictPage() {
    Page* firstIn = evictionQueue.front();
    evictionQueue.pop();
    
    pageTable.erase(firstIn->number());
    
    if (verbose) {
        std::cout << firstIn << " swapped by ";
    }
    
    return firstIn;
}

void FIFO::performWrite(std::string virtualAddress) {
    if (pageFault(virtualAddress)) {   // then no hit
        
        Page* freePage = getFirstFreePageInMemory();
        
        if (freePage != nullptr) {
            
            // marking dirty simulates write
            freePage->addMapping(virtualAddress, virtualAddress);
            
        } else {
            if (onDisk(virtualAddress)) {
                swapPage(virtualAddress);
                writeToPageInMemory(virtualAddress);
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
        writeToPageInMemory(virtualAddress);
    }
}

void FIFO::writeToPageInMemory(std::string virtualAddress) {
    Page* pageToWriteTo = findPageInMemory(virtualAddress);
    
    if (pageToWriteTo != nullptr) {
        pageToWriteTo->markDirty();
    } else {
        std::cout << "WARNING: unable to find page in memory for write" << std::endl;
    }
}

Page* FIFO::findPageInMemory(std::string virtualAddress) {
    Page* page = nullptr;
    
    for (const auto& pair : pageTable) {
        if (pair.second->contains(virtualAddress)) {
            page = pair.second;
        }
    }
    
    return page;
}
