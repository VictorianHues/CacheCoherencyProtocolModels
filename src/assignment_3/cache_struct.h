#ifndef CACHE_STRUCT_H
#define CACHE_STRUCT_H

#include "constants.h"

/**
 * MOESI Cache State Enum
 * 
 * The Cache State Enum defines the possible states of a Cache Line in the Cache.
 * 
 * INVALID: The Cache Line is invalid and does not contain any data.
 * SHARED: The Cache Line is shared with other Caches and is up-to-date in Main Memory.
 * EXCLUSIVE: The Cache Line is exclusive to this Cache and is up-to-date in Main Memory.
 * MODIFIED: The Cache Line is modified and may be stale in Main Memory.
 * OWNED: The Cache Line is owned by this Cache and may be stale in Main Memory
 * 
 */
enum class CacheState : uint64_t {
    INVALID = 0,
    SHARED = 1,
    EXCLUSIVE = 2,
    MODIFIED = 3,
    OWNED = 4
};

/**
 * Cache Line Struct
 * 
 * The Cache Line Struct defines the structure of a Cache Line in the Cache.
 * 
 * tag: The tag of the Cache Line.
 * state: The state of the Cache Line.
 * data: The data stored in the Cache Line.
 * 
 */
struct CacheLine {
    uint64_t tag = -1;

    CacheState state = CacheState::INVALID;

    uint64_t data[LINE_SIZE / sizeof(uint64_t)] = {0};
};

/**
 * Cache Set Struct
 * 
 * The Cache Set Struct defines the structure of a Cache Set in the Cache.
 * 
 * lines: The Cache Lines in the Cache Set.
 * lru: The Least Recently Used counter for the Cache Set.
 */
struct CacheSet {
    CacheLine lines[SET_ASSOCIATIVITY]; // 8 lines per set
    size_t lru[SET_ASSOCIATIVITY] = {0, 1, 2, 3, 4, 5, 6, 7}; // Least Recently Used counter
};

#endif