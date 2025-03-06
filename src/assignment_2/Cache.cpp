#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "Cache.h"
#include "psa.h"

bool Cache::system_busy() {
    return !requestQueue.empty() || !requestQueue.empty() || bus->system_busy();
}

void Cache::cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag) {
    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (cache[set_index].lines[i].tag == tag) {
            cache_hit = true;
            cache_hit_index = i;
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

void Cache::decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line, uint64_t &data) {
    tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
    set_index = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
    byte_in_line = addr % LINE_SIZE; // address modulo the line size
    data = 128 + addr; // Placeholder data
}

void Cache::set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, 
                            uint64_t data, uint64_t byte_in_line, bool valid, bool dirty) {
    log(name(), "SETTING CACHE LINE", cache_hit_index, "in set", set_index);
    log(name(), "tag", tag, "data", data, "byte", byte_in_line, "valid", valid, "dirty", dirty);

    cache[set_index].lines[cache_hit_index].tag = tag; // Set tag
    cache[set_index].lines[cache_hit_index].valid = valid; // Set valid bit
    cache[set_index].lines[cache_hit_index].dirty = dirty; // Set dirty bit
    cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data

    update_lru(cache[set_index], cache_hit_index);
    cout << sc_time_stamp() << ": UPDATED LRU Queue: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;
}

void Cache::cpu_read(uint64_t addr) {
    log(name(), "CPU READ for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, RequestType> req = {addr, RequestType::READ};
    requestQueue.push_back(req);
}

void Cache::cpu_write(uint64_t addr) {
    log(name(), "CPU WRITE for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, RequestType> req = {addr, RequestType::WRITE};
    requestQueue.push_back(req);
}

void Cache::snoop_read_response_cache(uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, ResponseType> res = {addr, ResponseType::BUS_READ_RESPONSE_CACHE};
    responseQueue.push_front(res);
}

void Cache::snoop_read_response_mem(uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, ResponseType> res = {addr, ResponseType::BUS_READ_RESPONSE_MEM};
    responseQueue.push_front(res);
}

void Cache::snoop_invalidate_response(uint64_t addr) {
    log(name(), "SNOOP INVALIDATE RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, ResponseType> res = {addr, ResponseType::INVALIDATE_RESPONSE};
    responseQueue.push_front(res);
}

void Cache::read_for_write_allocate_response(uint64_t addr, uint64_t data) {
    log(name(), "READ FOR WRITE ALLOCATE RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, ResponseType> res = {addr, ResponseType::READ_FOR_WRITE_ALLOCATE};
    responseQueue.push_front(res);
}

void Cache::write_to_main_memory_complete(uint64_t addr) {
    log(name(), "WRITE TO MAIN MEMORY RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::pair<uint64_t, ResponseType> res = {addr, ResponseType::WRITE_TO_MAIN_MEM};
    responseQueue.push_front(res);
}

void Cache::processRequestQueue() {
    while(true) {
        if (!requestQueue.empty()) {
            std::pair<uint64_t, RequestType> request = requestQueue.front();
            requestQueue.pop_front();

            uint64_t addr = request.first;
            RequestType req_type = request.second;

            log(name(), "PROCESSING REQUEST QUEUE on Cache", id, "for address", addr);

            uint64_t tag;
            int set_index;
            uint64_t byte_in_line;
            uint64_t data;

            bool cache_hit = false;
            size_t cache_hit_index = -1;
            bool cache_line_valid = false;
            //bool cache_line_dirty = false;

            decode_address(addr, set_index, tag, byte_in_line, data);

            cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

            if (cache_hit) {
                cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
                //cache_line_dirty = cache[set_index].lines[cache_hit_index].dirty;
            }

            switch (req_type) {
                case RequestType::READ:
                    if (!cache_hit || !cache_line_valid) {
                        log(name(), "READ MISS on tag", tag, "in set", set_index);
                        bus->read(id, addr);
                    } else {
                        log(name(), "READ HIT on tag", tag, "in set", set_index);
                        log(name(), "Sending Data to CPU");

                        cpu->read_response(addr, data); // READ HIT PROCESS ENDS HERE

                        stats_readhit(id);
                    } 
                    break;
                case RequestType::WRITE:
                    if (!cache_hit) {
                        log(name(), "WRITE MISS requires WRITE ALLOCATE READ on tag", tag, "in set", set_index);
                        
                        bus->read_for_write_allocate(id, addr);

                        stats_writemiss(id);

                    } else {
                        log(name(), "WRITE HIT on tag", tag, "in set", set_index);

                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, true);

                        bus->broadcast_invalidate(id, addr);

                        stats_writehit(id);
                    } 
                    break;
            }
        }
        wait();
    }
}

void Cache::processResponseQueue() {
    while(true) {
        if (!responseQueue.empty()) {
            std::pair<uint64_t, ResponseType> response = responseQueue.front();
            responseQueue.pop_front();

            uint64_t addr = response.first;
            ResponseType res_type = response.second;

            log(name(), "PROCESSING RESPONSE QUEUE on Cache", id, "for address", addr);

            uint64_t tag;
            int set_index;
            uint64_t byte_in_line;
            uint64_t data = 128; // Placeholder data

            bool cache_hit = false;
            size_t cache_hit_index = -1;
            bool cache_line_valid = false;
            bool cache_line_dirty = false;

            decode_address(addr, set_index, tag, byte_in_line, data);

            cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

            if (cache_hit) {
                cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
                cache_line_dirty = cache[set_index].lines[cache_hit_index].dirty;
            }

            switch (res_type) {
                case ResponseType::BUS_READ_RESPONSE_CACHE: // Bus read response from parallel cache after Cache read miss
                    log(name(), "BUS READ RESPONSE from parallel Cache for address", addr);

                    cache_hit_index = find_lru(cache[set_index]);

                    cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
                    cache_line_dirty = cache[set_index].lines[cache_hit_index].dirty;

                    if (cache_line_valid && cache_line_dirty) {
                        log(name(), "LINE DIRTY and VALID, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);

                        //uint64_t evicted_data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                        bus->write_to_main_memory(id, addr, data);
                    } 
                    // Dirty bit is true after snooping caches
                    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, true);

                    cpu->read_response(addr, data); // READ MISS TO CACHE SNOOP HIT PROCESS ENDS HERE

                    break;
                case ResponseType::BUS_READ_RESPONSE_MEM: // Bus read response from Main Memory after Cache read miss
                    log(name(), "BUS READ RESPONSE queue from Main Memory for address", addr);

                    cache_hit_index = find_lru(cache[set_index]);

                    cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
                    cache_line_dirty = cache[set_index].lines[cache_hit_index].dirty;

                    if (cache_line_valid && cache_line_dirty) {
                        log(name(), "RESPONSE queue LINE DIRTY and VALID, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);
                        
                        //uint64_t evicted_data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];
                        
                        bus->write_to_main_memory(id, addr, data);
                    }
                    // Dirty bit is false after reading from memory
                    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, false);

                    cpu->read_response(addr, data); // READ MISS TO MEMORY PROCESS ENDS HERE

                    break;
                case ResponseType::READ_FOR_WRITE_ALLOCATE:
                    log(name(), "READ FOR WRITE ALLOCATE RESPONSE queue on address", addr);

                    cache_hit_index = find_lru(cache[set_index]);

                    cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
                    cache_line_dirty = cache[set_index].lines[cache_hit_index].dirty;

                    if (cache_line_valid && cache_line_dirty) {
                        log(name(), "LINE DIRTY and VALID, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);

                        //uint64_t evicted_data = cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                        bus->write_to_main_memory(id, addr, data);
                    } 

                    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, false);

                    bus->broadcast_invalidate(id, addr);

                    break;
                case ResponseType::WRITE_TO_MAIN_MEM:
                    log(name(), "WRITE TO MAIN MEMORY RESPONSE queue on address", addr);

                    cpu->write_response(addr); // WRITE TO MAIN MEMORY CAUSED BY DIRTY BIT PROCESS ENDS HERE
                    break;
                case ResponseType::INVALIDATE_RESPONSE:
                    log(name(), "INVALIDATE RESPONSE queue on address", addr);

                    cpu->write_response(addr); // WRITE RESPONSE PROCESS ENDS HERE
                    break;
            }
        }
        wait();
    }
}

bool Cache::snoop_read(uint64_t requester_id, uint64_t addr) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;
    bool cache_line_valid = false;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    if (cache_hit) {
        cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
        if (cache_line_valid) {
            log(name(), "SNOOP READ HIT on tag", tag, "in set", set_index);
            
            bus->cache_snoop_read_response(requester_id, addr, data);
            return true;
        }
    }
    log(name(), "SNOOP READ MISS on tag", tag, "in set", set_index);
    return false;
}

void Cache::snoop_invalidate(uint64_t requester_id, uint64_t addr) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    if (cache_hit) {
        log(name(), "SNOOP HIT, INVALIDATE on tag", tag, "in set", set_index);
        cache[set_index].lines[cache_hit_index].valid = false;
        cache[set_index].lines[cache_hit_index].dirty = false;
    } else {
        log(name(), "SNOOP MISS, NO INVALIDATE on tag", tag, "in set", set_index);
    }
}