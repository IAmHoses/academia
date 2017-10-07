#!/bin/bash

# touch mah343-new-compilation
# g++ ../mah343-paging-src/*cpp -o mah343-new-compilation
gzcat latex.gz | ./mah343 50 FIFO
gzcat latex.gz | ./mah343 50 GC
gzcat latex.gz | ./mah343 50 LRU
gzcat latex.gz | ./mah343 50 LFU
gzcat latex.gz | ./mah343 50 MFU
gzcat latex.gz | ./mah343 50 OWN
