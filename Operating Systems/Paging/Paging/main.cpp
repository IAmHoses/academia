//
//  main.cpp
//  Paging
//
//  Created by Marc Hosecloth on 4/23/17.
//  Copyright © 2017 Marc Hosecloth. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <iomanip>

#include "FIFO.hpp"
#include "GC.hpp"
#include "LRU.hpp"
#include "LFU.hpp"
#include "MFU.hpp"
#include "OWN.hpp"

enum Algorithm {
    FIFO_ALG, GC_ALG, LRU_ALG, LFU_ALG, MFU_ALG, OWN_ALG, INVALID_ALG
};

Algorithm algEnumHash(std::string algorithmString) {
    if (algorithmString == "FIFO" || algorithmString == "fifo") return FIFO_ALG;
    if (algorithmString == "GC" || algorithmString == "gc") return GC_ALG;
    if (algorithmString == "LRU" || algorithmString == "lru") return LRU_ALG;
    if (algorithmString == "LFU" || algorithmString == "lfu") return LFU_ALG;
    if (algorithmString == "MFU" || algorithmString == "mfu") return MFU_ALG;
    if (algorithmString == "OWN" || algorithmString == "own") return OWN_ALG;
    return INVALID_ALG;
 }

int main(int argc, const char * argv[]) {
    
    std::clock_t start;
    double duration;
    
    start = std::clock();
    
    const int RAM_FRAMES = std::stoi(argv[1]);
    std::string algorithm = argv[2];
    bool verbose = (argc == 4);
    
    switch (algEnumHash(algorithm)) {
        case FIFO_ALG: {
            FIFO* fifo = new FIFO(&RAM_FRAMES, verbose);
            fifo->simulate();
            break;
        }
            
        case GC_ALG: {
            GC* gc = new GC(&RAM_FRAMES, verbose);
            gc->simulate();
            break;
        }
            
        case LRU_ALG: {
            LRU* lru = new LRU(&RAM_FRAMES, verbose);
            lru->simulate();
            break;
        }
            
        case LFU_ALG: {
            LFU* lfu = new LFU(&RAM_FRAMES, verbose);
            lfu->simulate();
            break;
        }
            
        case MFU_ALG: {
            MFU* mfu = new MFU(&RAM_FRAMES, verbose);
            mfu->simulate();
            break;
        }
            
        case OWN_ALG: {
            OWN* own = new OWN(&RAM_FRAMES, verbose);
            own->simulate();
            break;
        }
            
        default:
            std::cout << "ERROR: invalid algorithm command line argument\n\t" << algorithm << std::endl;
            break;
    }
    
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    
    std::cout<< "Runtime: " << std::fixed << std::setprecision(2) << duration << "s" << std::endl;
    
    return 0;
}
