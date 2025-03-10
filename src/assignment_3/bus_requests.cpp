#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_if.h"
#include "memory_if.h"
#include "CACHE.h"
#include "psa.h"
#include "BUS.h"



void Bus::read(uint64_t requester_id, uint64_t addr) {
    log(name(), "READ pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::READ};
    requestQueue.push_back(req);
}

void Bus::write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "WRITE to Main Memory pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::WRITE_TO_MAIN_MEM};
    requestQueue.push_back(req);
}

void Bus::read_for_write_allocate(uint64_t requester_id, uint64_t addr) {
    log(name(), "READ from Main Memory for WRITE ALLOCATE from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::READ_WRITE_ALLOCATE};
    requestQueue.push_back(req);
}

void Bus::broadcast_invalidate(uint64_t requester_id, uint64_t addr) {
    log(name(), "BROADCAST INVALIDATE pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::INVALIDATE};
    requestQueue.push_back(req);
}

void Bus::processRequestQueue() {
    while(true) {
        if (!requestQueue.empty()) {
            std::vector<uint64_t> req = requestQueue.front();
            requestQueue.pop_front();

            uint64_t req_cache_id = req[0];
            uint64_t req_addr = req[1];
            uint64_t req_type = req[2];
            uint64_t data = 128; // Placeholder data

            log(name(), "PROCESSING REQUEST QUEUE for Cache", req_cache_id, "address", req_addr);

            bool snoop_hit = false;

            switch (req_type) {
                case RequestType::READ: 
                    // Search for the address in all caches
                    for (Cache* cache : cache_list) {
                        log(name(), "READ SNOOPING request for Cache ", req_cache_id, "on Cache", cache->id);

                        if (cache->id != req_cache_id) {
                            if (cache->snoop_read(cache->id, req_addr)) {
                                snoop_hit = true;

                                std::vector<uint64_t> res = {cache->id, req_addr, ResponseType::SNOOP_READ_RESPONSE_CACHE};
                                responseQueue.push_front(res);

                                stats_readhit(req_cache_id);
                                break;
                            }
                        }
                    }
                    // If no cache has the data, read from Main Memory
                    if (!snoop_hit) {
                        log(name(), "READ FAILED SNOOP for Cache", req_cache_id, "address", req_addr);
                        memory->read_failed_snoop(req_cache_id, req_addr);
                        stats_readmiss(req_cache_id);
                    }
                    break;
                case RequestType::WRITE_TO_MAIN_MEM: // Write to Main Memory
                    memory->write(req_cache_id, req_addr, data); 
                    break;
                case RequestType::INVALIDATE: // Broadcast invalidation to all Caches
                    for (Cache* cache : cache_list) {
                        if (cache->id != req_cache_id) {
                            log(name(), "INVALIDATION SNOOPING from Cache", req_cache_id, "on Cache", cache->id);
                            cache->snoop_invalidate(cache->id, req_addr);
                        }
                    }
                    for (Cache* cache : cache_list) {
                        if (cache->id == req_cache_id) {
                            cache->snoop_invalidate_response(req_addr);
                        }
                    }
                    break;
                case RequestType::READ_WRITE_ALLOCATE: // Read for Write Allocate
                log(name(), "READ WRITE ALLOCATE from Cache", req_cache_id, "address", req_addr);
                    memory->read_write_allocate(req_cache_id, req_addr);
                    break;
            }
        }
        wait();
    }
}