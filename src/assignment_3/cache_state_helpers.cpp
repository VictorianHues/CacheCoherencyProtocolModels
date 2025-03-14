#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

/**
 * Checks the Cache Set for a Cache Hit and writes the Cache Hit Index, Cache Line State, and Cache Hit status.
 * 
 * @param cache_hit True if the Cache Line is found in the Cache Set, False otherwise.
 * @param cache_hit_index The index of the Cache Line in the Cache Set.
 * @param cache_line_state The state of the Cache Line in the Cache Set.
 * @param set_index The index of the Cache Set in the Cache.
 * @param tag The tag of the Cache Line.
 * 
 */
void Cache::cache_hit_check(bool &cache_hit, size_t &cache_hit_index, CacheState &cache_line_state, int set_index, uint64_t tag) {
    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (cache[set_index].lines[i].tag == tag && cache[set_index].lines[i].state != CacheState::INVALID) {
            cache_line_state = cache[set_index].lines[i].state;
            cache_hit = true;
            cache_hit_index = i;
        }
    }
}

/**
 * Updates the LRU Queue for the Cache Set.
 * 
 * Iterates through the LRU Queue and increments the LRU value 
 * for all Cache Lines except the Cache Line that was hit.
 * 
 * @param cache_set The Cache Set to update the LRU Queue for.
 * @param index The index of the Cache Line in the Cache Set.
 * 
 */
void Cache::update_lru(CacheSet &cache_set, size_t index) {
    size_t current = cache_set.lru[index];

    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (i != index && cache_set.lru[i] < current) {
            cache_set.lru[i]++;
        }
    }
    cache_set.lru[index] = 0;
}

/**
 * Finds the Least Recently Used (LRU) Cache Line in the Cache Set.
 * 
 * Iterates through the LRU Queue and returns the index of the Cache Line with the highest LRU value.
 * 
 * @param cache_set The Cache Set to find the LRU Cache Line in.
 * 
 * @return size_t The index of the LRU Cache Line.
 */
size_t Cache::find_lru(CacheSet &cache_set) {
    size_t max_index = 0;

    for (size_t i = 1; i < SET_ASSOCIATIVITY; i++) {
        if (cache_set.lru[i] > cache_set.lru[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

/**
 * Decodes the address into the Cache Set Index, Tag, Byte in Line, and Data.
 * 
 * @param addr The address to decode.
 * @param set_index The index of the Cache Set.
 * @param tag The tag of the Cache Line.
 * @param byte_in_line The byte offset in the Cache Line.
 * @param data The data to store in the Cache Line.
 */
void Cache::decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line, uint64_t &data) {
    tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
    set_index = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
    byte_in_line = addr % LINE_SIZE; // address modulo the line size
    data = 128 + addr; // Placeholder data
}

/**
 * Sets the Cache Line in the Cache Set.
 * 
 * Sets the tag, state, and data for the Cache Line in the Cache Set.
 * 
 * @param set_index The index of the Cache Set.
 * @param cache_hit_index The index of the Cache Line in the Cache Set.
 * @param tag The tag of the Cache Line.
 * @param data The data to store in the Cache Line.
 * @param byte_in_line The byte offset in the Cache Line.
 * @param state The state of the Cache Line.
 * 
 */
void Cache::set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, 
                            uint64_t data, uint64_t byte_in_line, CacheState state) {
    log(name(), "SETTING CACHE LINE", cache_hit_index, "in set", set_index);
    log(name(), "tag", tag, "data", data, "byte", byte_in_line);

    cache[set_index].lines[cache_hit_index].tag = tag; // Set tag
    cache[set_index].lines[cache_hit_index].state = state; // Set state
    cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data

    update_lru(cache[set_index], cache_hit_index);
    cout << sc_time_stamp() << ": UPDATED LRU Queue: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;
}