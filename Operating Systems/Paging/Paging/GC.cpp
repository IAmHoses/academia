//
//  GC.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "GC.hpp"

#include <iostream>

GC::GC(const int* RAMframes, bool verbose) {
    GC::RAMframes = RAMframes;
    GC::verbose = verbose;
    accesses = misses = writes = drops = 0;
    
    nextPageNumber = 0;
    initializeMemory();
}

void GC::initializeMemory() {
    while (!framesNotFull()) {
        GlobalClockPage* newPage = new GlobalClockPage(nextPageNumber++);
        addToClock(newPage);
        writeToDisk(newPage);
        rotateClockHand();
    }
}

bool GC::framesNotFull() {
    return clock.size() == *RAMframes;
}

void GC::addToClock(GlobalClockPage* page, bool initializing) {
    if (!framesNotFull()) {
        std::list<GlobalClockPage*>::iterator it = clock.begin();
        
        int i = 0;
        while (i++ <= clockHand) { it++; }
        
        clock.insert(it, page);
    }
    else {
        std::cout << "WARNING: attempted to add page to full page table" << std::endl;
    }
}

void GC::writeToDisk(GlobalClockPage* page) {
    if (!onDisk(page)) {
        disk.insert(std::make_pair(page->number(), page));
    } else {
        if (page->isDirty()) {
            disk[page->number()] = page;
            writes++;
        }
    }
}

bool GC::onDisk(GlobalClockPage* page) {
    std::unordered_map<int, GlobalClockPage*>::const_iterator got = disk.find(page->number());
    
    if (got == disk.end()) {
        return false;
    }
    
    return true;
}

void GC::rotateClockHand() {
    clockHand = ++clockHand % *RAMframes;
}

GlobalClockPage* GC::getPageAtClockHand() {
    std::list<GlobalClockPage*>::iterator it = clock.begin();
    
    int i = 0;
    while (i++ < clockHand) { it++; }
    
    return *it;
}

bool GC::clockContains(std::string virtualAddress) {
    std::list<GlobalClockPage*>::iterator it = clock.begin();
    
    while (it != clock.end()) {
        if ((*it)->contains(virtualAddress)) {
            return true;
        }
        it++;
    }
    
    return false;
}


void GC::updateClock(std::string virtualAddress) {
    GlobalClockPage* currentPage = getPageAtClockHand();
    
    while (!currentPage->contains(virtualAddress)) {
        misses++;
        
        // page doesn't contain address, change ref bit to 0 if needed
        
        if (currentPage->getReferenceBit() == 1) {
            currentPage->setReferenceBitToZero();
        }
        
        rotateClockHand();
        currentPage = getPageAtClockHand();
    }
    
    // got to page that contains address, change ref bit to 1 if needed
    
    if (currentPage->getReferenceBit() == 0) {
        currentPage->setReferenceBitToOne();
    }
}

void GC::simulate() {
    
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
    
    std::cout << "\n--------------------------------------\n\tOUTPUT STATISTICS (GC)\t\n--------------------------------------\n" << std::endl;
    std::cout << "Number of memory accesses: " << accesses << std::endl;
    std::cout << "Number of misses: " << misses << std::endl;
    std::cout << "Number of writes: " << writes << std::endl;
    std::cout << "Number of drops: " << drops << std::endl;
}

void GC::performRead(std::string virtualAddress) {
    if (!clockContains(virtualAddress)) {   // then no hit
        
        GlobalClockPage* freePage = getFirstFreePageInMemory();
        
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
                    addToClock(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    GlobalClockPage* newPage = new GlobalClockPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
                    addToClock(newPage);
                    writeToDisk(newPage);
                    
                    if (verbose) {
                        std::cout << newPage << std::endl;
                    }
                }
            }
        }
    }
    else {
        updateClock(virtualAddress);
    }
}

GlobalClockPage* GC::getFirstFreePageInMemory() {
    GlobalClockPage* freePage = nullptr;
    
    for (const auto& page : clock) {
        if (!page->isFull()) {
            freePage = page;
        }
    }
    return freePage;
}

GlobalClockPage* GC::getFirstFreePageOnDisk() {
    GlobalClockPage* freePage = nullptr;
    
    for (const auto& pair : disk) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

bool GC::onDisk(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            return true;
        }
    }
    return false;
}

// MARK THE new GlobalClockPage AS CLEAN ONCE IT COMES INTO MAIN MEMORY

void GC::swapPage(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            movePageToDisk();
            
            GlobalClockPage* overwritingPage = pair.second;
            overwritingPage->markClean();
            addToClock(overwritingPage);
            
            if (verbose) {
                std::cout << pair.second << std::endl;
            }
        }
    }
}

void GC::movePageToDisk() {
    GlobalClockPage* evictedPage = evictPage();
    
    if (evictedPage->isDirty()) {
        writeToDisk(evictedPage);
    } else {
        drops++;
    }
}

GlobalClockPage* GC::evictPage() {
    GlobalClockPage* evicted = nullptr;
    
    while (evicted == nullptr) {
        GlobalClockPage* currentPage = getPageAtClockHand();
        
        if (currentPage->getReferenceBit() == 0) {
            evicted = currentPage;
            clock.remove(currentPage);
        } else {
            currentPage->setReferenceBitToZero();
            rotateClockHand();
        }
        
        misses++;
    }
    
    if (verbose) {
        std::cout << evicted << " swapped by ";
    }
    
    return evicted;
}

void GC::performWrite(std::string virtualAddress) {
    if (!clockContains(virtualAddress)) {   // then no hit
        
        GlobalClockPage* freePage = getFirstFreePageInMemory();
        
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
                    addToClock(freePage);
                    
                    if (verbose) {
                        std::cout << freePage << std::endl;
                    }
                    
                } else {
                    GlobalClockPage* newPage = new GlobalClockPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
                    addToClock(newPage);
                    writeToDisk(newPage);
                    
                    if (verbose) {
                        std::cout << newPage << std::endl;
                    }
                }
            }
        }
    } else {
        updateClock(virtualAddress);
        writeToPageInMemory(virtualAddress);
    }
}

void GC::writeToPageInMemory(std::string virtualAddress) {
    GlobalClockPage* pageToWriteTo = getPageAtClockHand();
    
    if (pageToWriteTo != nullptr) {
        pageToWriteTo->markDirty();
    } else {
        std::cout << "WARNING: unable to find page in memory for write" << std::endl;
    }
}
