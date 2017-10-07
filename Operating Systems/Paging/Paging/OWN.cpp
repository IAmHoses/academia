//
//  OWN.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/27/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "OWN.hpp"

#include <iostream>

OWN::OWN(const int* RAMframes, bool verbose) {
    OWN::RAMframes = RAMframes;
    OWN::verbose = verbose;
    accesses = misses = writes = drops = 0;
    
    nextPageNumber = 0;
    initializeMemory();
}

void OWN::initializeMemory() {
    while (!framesNotFull()) {
        OwnPage* newPage = new OwnPage(nextPageNumber++);
        addToClock(newPage);
        writeToDisk(newPage);
        rotateClockHand();
    }
}

bool OWN::framesNotFull() {
    return clock.size() == *RAMframes;
}

void OWN::addToClock(OwnPage* page, bool initializing) {
    if (!framesNotFull()) {
        std::list<OwnPage*>::iterator it = clock.begin();
        
        int i = 0;
        while (i++ <= clockHand) { it++; }
        
        clock.insert(it, page);
    }
    else {
        std::cout << "WARNING: attempted to add page to full page table" << std::endl;
    }
}

void OWN::writeToDisk(OwnPage* page) {
    if (!onDisk(page)) {
        disk.insert(std::make_pair(page->number(), page));
    } else {
        if (page->isDirty()) {
            disk[page->number()] = page;
            writes++;
        }
    }
}

bool OWN::onDisk(OwnPage* page) {
    std::unordered_map<int, OwnPage*>::const_iterator got = disk.find(page->number());
    
    if (got == disk.end()) {
        return false;
    }
    
    return true;
}

void OWN::rotateClockHand() {
    clockHand = ++clockHand % *RAMframes;
}

OwnPage* OWN::getPageAtClockHand() {
    std::list<OwnPage*>::iterator it = clock.begin();
    
    int i = 0;
    while (i++ < clockHand) { it++; }
    
    return *it;
}

bool OWN::clockContains(std::string virtualAddress) {
    std::list<OwnPage*>::iterator it = clock.begin();
    
    while (it != clock.end()) {
        if ((*it)->contains(virtualAddress)) {
            return true;
        }
        it++;
    }
    
    return false;
}


void OWN::updateClock(std::string virtualAddress) {
    OwnPage* currentPage = getPageAtClockHand();
    
    while (!currentPage->contains(virtualAddress)) {
        misses++;
        
        // page doesn't contain address, decrement ref int if needed
        
        if (currentPage->getReferenceInteger() > 0) {
            currentPage->decrementReferenceInteger();
        }
        
        rotateClockHand();
        currentPage = getPageAtClockHand();
    }
    
    // got to page that contains address, increment ref int if needed
    
    if (currentPage->getReferenceInteger() >= 0) {
        currentPage->incrementReferenceInteger();
    }
}

void OWN::simulate() {
    
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
    
    std::cout << "\n---------------------------------------\n\tOUTPUT STATISTICS (OWN)\t\n---------------------------------------\n" << std::endl;
    std::cout << "Number of memory accesses: " << accesses << std::endl;
    std::cout << "Number of misses: " << misses << std::endl;
    std::cout << "Number of writes: " << writes << std::endl;
    std::cout << "Number of drops: " << drops << std::endl;
}
 
void OWN::performRead(std::string virtualAddress) {
    if (!clockContains(virtualAddress)) {   // then no hit
        
        OwnPage* freePage = getFirstFreePageInMemory();
        
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
                    OwnPage* newPage = new OwnPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
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

OwnPage* OWN::getFirstFreePageInMemory() {
    OwnPage* freePage = nullptr;
    
    for (const auto& page : clock) {
        if (!page->isFull()) {
            freePage = page;
        }
    }
    return freePage;
}

OwnPage* OWN::getFirstFreePageOnDisk() {
    OwnPage* freePage = nullptr;
    
    for (const auto& pair : disk) {
        if (!pair.second->isFull()) {
            freePage = pair.second;
        }
    }
    return freePage;
}

bool OWN::onDisk(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            return true;
        }
    }
    return false;
}

// MARK THE new OwnPage AS CLEAN ONCE IT COMES INTO MAIN MEMORY

void OWN::swapPage(std::string virtualAddress) {
    for (const auto& pair : disk) {
        if (pair.second->contains(virtualAddress)) {
            movePageToDisk();
            
            OwnPage* overwritingPage = pair.second;
            overwritingPage->markClean();
            addToClock(overwritingPage);
            
            if (verbose) {
                std::cout << pair.second << std::endl;
            }
        }
    }
}

void OWN::movePageToDisk() {
    OwnPage* evictedPage = evictPage();
    
    if (evictedPage->isDirty()) {
        writeToDisk(evictedPage);
    } else {
        drops++;
    }
}

OwnPage* OWN::evictPage() {
    OwnPage* evicted = nullptr;
    
    while (evicted == nullptr) {
        OwnPage* currentPage = getPageAtClockHand();
        
        if (currentPage->getReferenceInteger() == 0) {
            evicted = currentPage;
            clock.remove(currentPage);
        }
        
        if (currentPage->getReferenceInteger() > 0) {
            currentPage->decrementReferenceInteger();
            rotateClockHand();
        }
        
        misses++;
    }
    
    if (verbose) {
        std::cout << evicted << " swapped by ";
    }
    
    return evicted;
}

void OWN::performWrite(std::string virtualAddress) {
    if (!clockContains(virtualAddress)) {   // then no hit
        
        OwnPage* freePage = getFirstFreePageInMemory();
        
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
                    OwnPage* newPage = new OwnPage(nextPageNumber++, virtualAddress,virtualAddress);
                    
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

void OWN::writeToPageInMemory(std::string virtualAddress) {
    OwnPage* pageToWriteTo = getPageAtClockHand();
    
    if (pageToWriteTo != nullptr) {
        pageToWriteTo->markDirty();
    } else {
        std::cout << "WARNING: unable to find page in memory for write" << std::endl;
    }
}
