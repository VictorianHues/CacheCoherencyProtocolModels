#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

/* Cache Memory Module */
static const size_t CACHE_SIZE = 32 * 1024; // 32KB to Bytes
static const size_t SET_ASSOCIATIVITY = 8; // 8 way set assoc
static const size_t LINE_SIZE = 32; // 32 bytes per cache line
static const size_t SET_SIZE = SET_ASSOCIATIVITY * LINE_SIZE; // 256 byte set
static const size_t NUM_SETS = CACHE_SIZE / SET_SIZE; // 128 sets in cache
static const size_t MEM_LATENCY = 100; // 100 cycles Memory Latency
static const size_t CACHE_CYCLE_LATENCY = 1; // 1 cycle Cache Latency

#endif