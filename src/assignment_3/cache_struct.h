#ifndef CACHE_STRUCT_H
#define CACHE_STRUCT_H

#include "constants.h"

struct CacheLine {
    uint64_t tag = -1;

    bool valid = false;
    bool dirty = false;
    bool exclusive = false;
    bool shared = false;
    bool owned = false;
    bool modified = false;

    uint64_t data[LINE_SIZE / sizeof(uint64_t)] = {0};
};

struct CacheSet {
    CacheLine lines[SET_ASSOCIATIVITY]; // 8 lines per set
    size_t lru[SET_ASSOCIATIVITY] = {0, 1, 2, 3, 4, 5, 6, 7}; // Least Recently Used counter
};

#endif