#include <assert.h>
#include <systemc.h>

#include "Cache.h"
#include "psa.h"

void Cache::cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag) {
    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) { // Iterate over lines in set
        if (cache[set_index].lines[i].tag == tag) {
            cache_hit = true; // Cache hit
            cache_hit_index = i; // Cache hit index

            break;
        }
    }
}

void Cache::update_lru(CacheSet &cache_set, size_t index) {
    size_t current = cache_set.lru[index];

    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (i != index && cache_set.lru[i] < current) {
            cache_set.lru[i]++;
        }
    }
    cache_set.lru[index] = 0;
}

size_t Cache::find_lru(CacheSet &cache_set) {
    size_t max_index = 0;

    for (size_t i = 1; i < SET_ASSOCIATIVITY; i++) {
        if (cache_set.lru[i] > cache_set.lru[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

void Cache::decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line) {
    tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
    set_index = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
    byte_in_line = addr % LINE_SIZE; // address modulo the line size
}

void Cache::set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, 
                            uint64_t data, uint64_t byte_in_line, bool valid, bool dirty) {
    log(name(), "SETTING CACHE LINE", cache_hit_index, "in set", set_index);
    log(name(), "tag", tag, "data", data, "byte", byte_in_line, "valid", valid, "dirty", dirty);

    cache[set_index].lines[cache_hit_index].tag = tag; // Set tag
    cache[set_index].lines[cache_hit_index].valid = valid; // Set valid bit
    cache[set_index].lines[cache_hit_index].dirty = dirty; // Set dirty bit
    cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data
}

void Cache::eviction_write_back_check(int set_index, size_t cache_hit_index, uint64_t byte_in_line) {
    log(name(), "EVICTION of line", cache_hit_index, "in set", set_index);

    /* If evicted line is dirty, write to Main Memory */
    if (cache[set_index].lines[cache_hit_index].dirty) {
        log(name(), "WRITE BACK dirty evicted line to Main Memory");

        //uint64_t evicted_data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

        log(name(), "WRITE BACK evicted data to MAIN MEMORY");

        bus->write(128, this);

        while (!response_event.triggered()) {
            wait(clk.posedge_event());
            //log(name(), "waiting for response...");
        }
    }
    /* If not dirty, replace evicted without consequence */
}

void Cache::wait_for_bus_response() {
    while (!response_event.triggered()) {
        wait(clk.posedge_event());
        //log(name(), "waiting for response...");
    }
    wait(clk.posedge_event());
}

void Cache::read_hit(int set_index, size_t &cache_hit_index, 
    uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr) {
    log(name(), "READ HIT on address", addr);
    if (!cache[set_index].lines[cache_hit_index].valid) {
        /* If a Cache Line is Invalid, but that Tags match in a Cache Hit
            we can use this same line when storing data from Bus
            so that there is no need to evict the lru line */
        log(name(), "line INVALID for read, SNOOPING BUS");

        bus->read(addr, this);
        wait_for_bus_response();

        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, false);

        stats_readmiss(id);
    } else {
        log(name(), "line VALID for read, fetching from CACHE");
        stats_readhit(id);
    }
    //stats_readhit(id);
}

void Cache::read_miss(int set_index, size_t &cache_hit_index, 
    uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr) {
    /* No matching tag in Cache Set (Cache Miss), so the data must 
        be read from Bus and stored in the Least-recently 
        used Cache line by evicting the data in that line */
    log(name(), "READ MISS on address", addr);
    log(name(), "WRITE ALLOCATE SNOOP BUS");

    bus->read(addr, this);
    wait_for_bus_response();

    log(name(), "WRITE ALLOCATE COMPLETE, searching for LRU line...");

    cache_hit_index = find_lru(cache[set_index]);

    eviction_write_back_check(set_index, cache_hit_index, byte_in_line);

    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, false);

    stats_readmiss(id);
}

void Cache::write_hit(int set_index, size_t &cache_hit_index, 
    uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr) {
    /* If a Cache Hit occurs, simply write the data from the
    CPU to the Cache Line */
    log(name(), "WRITE HIT on address", addr);

    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, true);

    bus->write_invalidate(addr, this);
    wait_for_bus_response();
    
    stats_writehit(id);
}

void Cache::write_miss(int set_index, size_t &cache_hit_index, 
    uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr) {
    /* If the Tags don't match (Catch Miss), find the Least Recently
    used Cache Line and replace it with the data from CPU, evicting
    the data in that line */

    log(name(), "WRITE MISS on address", addr);
    log(name(), "WRITE ALLOCATE SNOOP BUS");

    bus->read(addr, this); // Simulates WRITE-ALLOCATE reading a Cache Line from Main Memory
    wait_for_bus_response();

    log(name(), "WRITE ALLOCATE COMPLETE, searching for LRU line...");

    cache_hit_index = find_lru(cache[set_index]);

    eviction_write_back_check(set_index, cache_hit_index, byte_in_line);

    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, true);

    bus->write_invalidate(addr, this);
    wait_for_bus_response();
    
    stats_writemiss(id);
}


int Cache::cpu_read(uint64_t addr) {
    int set_index;
    uint64_t tag;
    uint64_t byte_in_line;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    uint64_t data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

    log(name(), "CPU READ on tag", tag, "set", set_index, "byte", byte_in_line);

    cout << sc_time_stamp() << ": INITIAL LRU Queue End: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;


    if (cache_hit) {
        read_hit(set_index, cache_hit_index, tag, data, byte_in_line, addr);
    } else {
        read_miss(set_index, cache_hit_index, tag, data, byte_in_line, addr);
    }

    update_lru(cache[set_index], cache_hit_index);

    cout << sc_time_stamp() << ": UPDATED LRU Queue End: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;

    return 0;
}

int Cache::cpu_write(uint64_t addr) {
    int set_index;
    uint64_t tag;
    uint64_t byte_in_line;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    uint64_t data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];
    
    log(name(), "CPU WRITE on tag", tag, "set", set_index, "byte", byte_in_line);

    cout << sc_time_stamp() << ": INITIAL LRU Queue: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;

    if (cache_hit) {
        write_hit(set_index, cache_hit_index, tag, data, byte_in_line, addr);
    } else {
        write_miss(set_index, cache_hit_index, tag, data, byte_in_line, addr);
    }

    update_lru(cache[set_index], cache_hit_index);

    cout << sc_time_stamp() << ": UPDATED LRU Queue: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;

    return 0;
}


bool Cache::snoop(uint64_t addr, bool invalidate) {
    int set_index;
    uint64_t tag;
    uint64_t byte_in_line;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    bool cache_line_valid = cache[set_index].lines[cache_hit_index].valid;

    if (cache_hit) {
        log(name(), "SNOOP HIT on address", addr);
        if (invalidate) {
            log(name(), "PROBE WRITE invalidates cache", id);
            cache[set_index].lines[cache_hit_index].valid = false;
        } else {
            if (cache_line_valid) {
                log(name(), "PROBE READ for valid cache", id);
                return true;
            } else {
                log(name(), "PROBE READ for invalid cache", id);
                return false;
            }
        }
    }

    return false;
}