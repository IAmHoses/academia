#!/bin/bash

# compile code

touch mah343
g++ -std=c++11 ../mah343-paging-src/*cpp -o mah343

# execute runs

gzcat ~clay/gpp.gz | ./mah343 50 FIFO
# gzcat ~clay/gpp.gz | ./mah343 50 GC
# gzcat ~clay/gpp.gz | ./mah343 50 LRU
# gzcat ~clay/gpp.gz | ./mah343 50 LFU
# gzcat ~clay/gpp.gz | ./mah343 50 MFU
# gzcat ~clay/gpp.gz | ./mah343 50 OWN
