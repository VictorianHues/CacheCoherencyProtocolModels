#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

/**
 * READ REQUEST from CPU
 * 
 * @param addr The address of the Cache Line to READ.
 */
void Cache::cpu_read(uint64_t addr) {
    log(name(), "CPU READ for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> req = {addr, RequestType::READ};
    requestQueue.push_back(req);
}

/**
 * WRITE REQUEST from CPU
 * 
 * @param addr The address of the Cache Line to WRITE.
 */
void Cache::cpu_write(uint64_t addr) {
    log(name(), "CPU WRITE for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> req = {addr, RequestType::WRITE};
    requestQueue.push_back(req);
}

/**
 * Process the Request Queue for the Cache as a SystemC Thread.
 */
void Cache::processRequestQueue() {
    while(true) {
        if (!requestQueue.empty()) {
            std::vector<uint64_t> request = requestQueue.front();
            requestQueue.pop_front();

            uint64_t addr = request[0];
            uint64_t req_type = request[1];

            log(name(), "PROCESSING REQUEST QUEUE on Cache", id, "for address", addr);

            uint64_t tag;
            int set_index;
            uint64_t byte_in_line;
            uint64_t data;

            bool cache_hit = false;
            size_t cache_hit_index = -1;
            CacheState cache_line_state = CacheState::INVALID;

            decode_address(addr, set_index, tag, byte_in_line, data);

            cache_hit_check(cache_hit, cache_hit_index, cache_line_state, set_index, tag);

            switch (req_type) {
                case RequestType::READ:
                    if (!cache_hit || cache_line_state == CacheState::INVALID) {
                        log(name(), "READ MISS on tag", tag, "in set", set_index);
                        
                        wait_for_bus_arbitration();
                        bus->read(id, addr);
                    } else {
                        log(name(), "READ HIT on tag", tag, "in set", set_index);


                        cpu->read_response(addr, data); // READ HIT PROCESS ENDS HERE
                        stats_readhit(id);
                    } 
                    break;
                case RequestType::WRITE:
                    if (!cache_hit || cache_line_state == CacheState::INVALID) {
                        log(name(), "WRITE MISS on tag", tag, "in set", set_index);
                        
                        wait_for_bus_arbitration();
                        bus->read_for_write_allocate(id, addr);

                        stats_writemiss(id);
                    } else { 
                        log(name(), "WRITE HIT on tag", tag, "in set", set_index);

                        wait_for_bus_arbitration();
                        bus->broadcast_invalidate(id, addr);
                        
                        stats_writehit(id);
                    } 
                    break;
            }
        }
        wait();
    }
}