#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "cache.h"
#include "psa.h"

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

                        wait_for_bus_arbitration();
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
                        
                        wait_for_bus_arbitration();
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

                        wait_for_bus_arbitration();
                        bus->write_to_main_memory(id, addr, data);
                    } 

                    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, false);

                    wait_for_bus_arbitration();
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