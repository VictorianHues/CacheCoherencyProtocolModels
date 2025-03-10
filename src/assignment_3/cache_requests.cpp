#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

void Cache::cpu_read(uint64_t addr) {
    log(name(), "CPU READ for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> req = {addr, RequestType::READ};
    requestQueue.push_back(req);
}

void Cache::cpu_write(uint64_t addr) {
    log(name(), "CPU WRITE for address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> req = {addr, RequestType::WRITE};
    requestQueue.push_back(req);
}

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
                        
                        wait_for_bus_arbitration();
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
                        
                        wait_for_bus_arbitration();
                        bus->read_for_write_allocate(id, addr);

                        stats_writemiss(id);

                    } else {
                        log(name(), "WRITE HIT on tag", tag, "in set", set_index);

                        set_cache_line(set_index, cache_hit_index, tag, data, byte_in_line, true, true);

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