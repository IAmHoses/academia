//
//  Page.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/26/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include "Page.hpp"

#include <iostream>

/*
    Page Base Class
 */

Page::Page(int number) {
    Page::pageNumber = number;
    markClean();
}

Page::Page(int number, std::string virtualAddress, std::string physicalAddress) {
    Page::pageNumber = number;
    addMapping(virtualAddress, physicalAddress);
    markClean();
}

int Page::number() {
    return Page::pageNumber;
}

bool Page::isFull() {
    return memoryMap.size() == CAPACITY;
}

void Page::addMapping(std::string virtualAddress, std::string physicalAddress) {
    if (!isFull()) {
        memoryMap.insert(std::make_pair(virtualAddress, physicalAddress));
        markDirty();
        
    } else {
        std::cout << "WARNING: attempted to overflow memoryMap capacity" << std::endl;
        std::cout << "\t{" << virtualAddress << ", " << physicalAddress << "}" << std::endl;
    }
}

bool Page::contains(std::string virtualAddress) {
    std::unordered_map<std::string, std::string>::const_iterator got = memoryMap.find(virtualAddress);
    if (got == memoryMap.end()) {
        return false;
    }
    return true;
}

bool Page::isDirty() {
    return dirty;
}

void Page::markDirty() {
    dirty = true;
}

void Page::markClean() {
    dirty = false;
}

/*
    FrequncyPage, Child of Page Base Class
 */

FrequencyPage::FrequencyPage(int number) : Page(number) {
    frequency = 0;
}

FrequencyPage::FrequencyPage(int number, std::string virtualAddress, std::string physicalAddress) : Page(number, virtualAddress, physicalAddress) {
    frequency = 1;
}

int FrequencyPage::getFrequency() const {
    return frequency;
}

void FrequencyPage::incrementFrequency() {
    frequency++;
}

/*
    GlobalClockPage, Child of Page Base Class
 */

GlobalClockPage::GlobalClockPage(int number) : Page(number) {
    referenceBit = 0;
}

GlobalClockPage::GlobalClockPage(int number, std::string virtualAddress, std::string physicalAddress) : Page(number, virtualAddress, physicalAddress) {
    referenceBit = 0;
}

bool GlobalClockPage::getReferenceBit() {
    return referenceBit;
}

void GlobalClockPage::setReferenceBitToOne() {
    referenceBit = 1;
}

void GlobalClockPage::setReferenceBitToZero() {
    referenceBit = 0;
}

/*
    OwnPage, Child of Page Base Class
 */

OwnPage::OwnPage(int number) : Page(number) {
    referenceInteger = 0;
}

OwnPage::OwnPage(int number, std::string virtualAddress, std::string physicalAddress) : Page(number, virtualAddress, physicalAddress) {
    referenceInteger = 0;
}

int OwnPage::getReferenceInteger() {
    return referenceInteger;
}

void OwnPage::incrementReferenceInteger() {
    referenceInteger++;
}

void OwnPage::decrementReferenceInteger() {
    referenceInteger--;
}
