#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

/**
 * RESPONSE from BUS after a READ FOR WRITE ALLOCATION request.
 * read_for_write_allocate -> read_for_write_allocate_response
 * 
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from Main Memory.
 */
void Cache::read_for_write_allocate_response(uint64_t addr, uint64_t data) {
    log(name(), "READ FOR WRITE ALLOCATE RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::READ_FOR_WRITE_ALLOCATE};
    responseQueue.push_front(res);
}

/**
 * RESPONSSE from BUS after a WRITE TO MAIN MEMORY request.
 * write_to_main_memory -> write_to_main_memory_complete
 * 
 * @param addr The address of the Cache Line to WRITE.
 */
void Cache::write_to_main_memory_complete(uint64_t addr) {
    log(name(), "WRITE TO MAIN MEMORY RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::WRITE_TO_MAIN_MEM};
    responseQueue.push_front(res);
}

/**
 * RESPONSE from BUS after a READ request completed with a SUCCESSFUL SNOOP.
 * snoop_read -> cache_snoop_read_response -> snoop_read_response_cache
 * 
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from the Cache.
 */
void Cache::snoop_read_response_cache(uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::BUS_READ_RESPONSE_CACHE};
    responseQueue.push_front(res);
}

/**
 * RESPONSE from BUS after a READ request completed with a UNSUCCESSFUL SNOOP
 * and resulting READ from MAIN MEMORY.
 * snoop_read -> cache_snoop_read_response -> snoop_read_response_mem
 * 
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from Main Memory.
 */
void Cache::snoop_read_response_mem(uint64_t addr, uint64_t data) {
    log(name(), "FAILED SNOOP MAIN MEM READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::BUS_READ_RESPONSE_MEM};
    responseQueue.push_front(res);
}

/**
 * RESPONSE from BUS after an INVALIDATE request on SNOOPED CACHE HITS.
 * snoop_invalidate -> cache_snoop_invalidate_response -> snoop_invalidate_response
 * 
 * @param addr The address of the Cache Line to INVALIDATE.
 */
void Cache::snoop_invalidate_response(uint64_t addr) {
    log(name(), "SNOOP INVALIDATE RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::INVALIDATE_RESPONSE};
    responseQueue.push_front(res);
}

/**
 * Process the Response Queue for the Bus as a SystemC Thread.
 */
void Cache::processResponseQueue() {
    while(true) {
        if (!responseQueue.empty()) {
            std::vector<uint64_t> response = responseQueue.front();
            responseQueue.pop_front();

            uint64_t addr = response[0];
            uint64_t res_type = response[1];

            log(name(), "PROCESSING RESPONSE QUEUE on Cache", id, "for address", addr);

            uint64_t tag;
            int set_index;
            uint64_t byte_in_line;
            uint64_t data = 128; // Placeholder data

            bool cache_hit = false;
            size_t cache_hit_index = -1;
            CacheState cache_line_state = CacheState::INVALID;

            decode_address(addr, set_index, tag, byte_in_line, data);

            cache_hit_check(cache_hit, cache_hit_index, cache_line_state, set_index, tag);

            switch (res_type) {
                case ResponseType::BUS_READ_RESPONSE_CACHE:
                    /**
                     * RESPONSE for a READ MISS on local CACHE leading to a READ HIT on a SNOOPING CACHE
                     * Cache Line is in SHARED state.
                     * 
                     * WRITE BACK to MAIN MEMORY if Cache Line is MODIFIED or OWNED.
                     */
                    log(name(), "BUS READ RESPONSE from parallel Cache for address", addr);

                    cache_hit_index = find_lru(cache[set_index]);

                    cache_line_state = cache[set_index].lines[cache_hit_index].state;

                    if (cache_line_state == CacheState::MODIFIED || cache_line_state == CacheState::OWNED) {
                        log(name(), "LINE MODIFED or OWNED, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);

                        wait_for_bus_arbitration();
                        bus->write_to_main_memory(id, addr, data);

                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::SHARED);
                    } else {
                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::SHARED);

                        cpu->read_response(addr, data); // READ MISS TO CACHE SNOOP HIT PROCESS ENDS HERE
                    }
                    
                    break;
                case ResponseType::BUS_READ_RESPONSE_MEM:
                    /**
                     * RESPONSE for READ MISS on local Cache leading to a FAILED SNOOP and READ from Main Memory.
                     * Cache Line is in EXCLUSIVE state.
                     * 
                     * WRITE BACK to MAIN MEMORY if Cache Line is MODIFIED or OWNED.
                     */
                    log(name(), "BUS READ RESPONSE queue from Main Memory for address", addr);

                    cache_hit_index = find_lru(cache[set_index]);

                    cache_line_state = cache[set_index].lines[cache_hit_index].state;

                    if (cache_line_state == CacheState::MODIFIED || cache_line_state == CacheState::OWNED) {
                        log(name(), "LINE MODIFED or OWNED, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);

                        wait_for_bus_arbitration();
                        bus->write_to_main_memory(id, addr, data);

                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::EXCLUSIVE);
                    } else {
                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::EXCLUSIVE);

                        cpu->read_response(addr, data); // READ MISS TO MEMORY PROCESS ENDS HERE
                    }
                    
                    break;
                case ResponseType::READ_FOR_WRITE_ALLOCATE:
                    /**
                     * RESPONSE for WRITE MISS on local CACHE leading to a READ for WRITE ALLOCATION from CACHE/MAIN MEMORY.
                     * Cache Line is in MODIFIED state.
                     * 
                     * WRITE BACK to MAIN MEMORY if Cache Line is MODIFIED or OWNED.
                     */
                    log(name(), "READ FOR WRITE ALLOCATE RESPONSE queue on address", addr);

                    cache_hit_index = find_lru(cache[set_index]);
                    
                    cache_line_state = cache[set_index].lines[cache_hit_index].state;

                    if (cache_line_state == CacheState::MODIFIED || cache_line_state == CacheState::OWNED) {
                        log(name(), "LINE MODIFED or OWNED, WRITE-BACK to Main Memory on tag", tag, "in set", set_index);

                        wait_for_bus_arbitration();
                        bus->write_to_main_memory(id, addr, data);

                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::MODIFIED);
                    } else {
                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::MODIFIED);

                        cpu->write_response(addr); // READ FOR WRITE ALLOCATE PROCESS ENDS HERE
                    }
                    break;
                case ResponseType::WRITE_TO_MAIN_MEM:
                    /**
                     * RESPONSE caused by a WRITE BACK to MAIN MEMORY
                     */
                    log(name(), "WRITE TO MAIN MEMORY RESPONSE queue on address", addr);

                    cpu->write_response(addr); // May be a result of a READ MISS or WRITE MISS, might change cpu notification to be general
                    break;
                case ResponseType::INVALIDATE_RESPONSE:
                    /**
                     * RESPONSE caused by an INVALIDATE broadcast from this Cache caused by a WRITE HIT.
                     */
                    log(name(), "INVALIDATE RESPONSE queue on address", addr);

                    set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, CacheState::MODIFIED);

                    cpu->write_response(addr); // WRITE RESPONSE PROCESS ENDS HERE
                    break;
            }
        }
        wait();
    }
}