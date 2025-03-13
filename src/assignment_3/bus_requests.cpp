#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_if.h"
#include "memory_if.h"
#include "CACHE.h"
#include "psa.h"
#include "BUS.h"


/**
 * Pushes a READ request to the Bus from a Cache.
 * Results from READ MISSES.
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 */
void Bus::read(uint64_t requester_id, uint64_t addr) {
    log(name(), "READ pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::READ};
    requestQueue.push_back(req);
}

/**
 * Pushes a WRITE TO MAIN MEMORY request to the Bus from a Cache.
 * Results from READ and WRITE MISSES that cause Cache Line Evictions.
 * 
 * @param requester_id The ID of the Cache that requested the WRITE.
 * @param addr The address of the Cache Line to WRITE.
 * @param data The data to WRITE to Main Memory.
 */
void Bus::write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "WRITE to Main Memory pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::WRITE_TO_MAIN_MEM};
    requestQueue.push_back(req);
}

/**
 * Pushes a READ FOR WRITE ALLOCATE request to the Bus from a Cache.
 * Results from WRITE MISSES that require data to be read from Main Memory.
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 */
void Bus::read_for_write_allocate(uint64_t requester_id, uint64_t addr) {
    log(name(), "READ from Main Memory for WRITE ALLOCATE from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::READ_WRITE_ALLOCATE};
    requestQueue.push_back(req);
}

/**
 * Broadcasts an INVALIDATE request to all Caches except the requester Cache.
 * Results from WRITE HITS that require other Caches to invalidate their Cache Lines.
 * 
 * @param requester_id The ID of the Cache that requested the WRITE.
 * @param addr The address of the Cache Line to INVALIDATE.
 */
void Bus::broadcast_invalidate(uint64_t requester_id, uint64_t addr) {
    log(name(), "BROADCAST INVALIDATE pushed to queue from Cache", requester_id, "for address", addr);

    std::vector<uint64_t> req = {requester_id, addr, RequestType::INVALIDATE};
    requestQueue.push_back(req);
}

/**
 * Process the Request Queue for the Bus.
 * Runs as a thread to process requests from the Cache.
 */
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
                    /**
                     * Read request from Cache for READ MISSES.
                     *  Caches Snoop the Bus for READS with matching CACHE LINES.
                     *  If no Cache has a matching Cache Line, then read from Main Memory.
                     */
                    for (Cache* cache : cache_list) {
                        log(name(), "READ SNOOPING request for Cache ", req_cache_id, "on Cache", cache->id);
                        if (cache->id != req_cache_id) {
                            if (cache->snoop_read(cache->id, req_addr, snoop_hit)) {
                                snoop_hit = true;
                            }
                        }
                    }
                    if (!snoop_hit) {
                        log(name(), "READ FAILED SNOOP for Cache", req_cache_id, "address", req_addr);
                        memory->read_failed_snoop(req_cache_id, req_addr);
                        stats_readmiss(req_cache_id);
                    } else {
                        log(name(), "READ SNOOP HIT for Cache", req_cache_id, "address", req_addr);
                        std::vector<uint64_t> res = {req_cache_id, req_addr, ResponseType::SNOOP_READ_RESPONSE_CACHE};
                        responseQueue.push_front(res);

                        stats_readhit(req_cache_id);
                    }
                    break;
                case RequestType::WRITE_TO_MAIN_MEM:
                    /** 
                     * WRITE directly to Main Memory during Cache Line Evictions. 
                     * 
                     * This could be implemented to allow the cache to skip waiting for the Main Memory WRITE to complete.
                    */
                    memory->write(req_cache_id, req_addr, data); 
                    break;
                case RequestType::INVALIDATE: 
                    /* Broadcast invalidation to all Caches except the requester Cache. 
                       Occurs at WRITE HITS to invalidate old data on other Caches. */
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

                case RequestType::READ_WRITE_ALLOCATE:
                    /* Read from Main Memory for WRITE ALLOCATE requests. 
                       Occurs at WRITE MISSES to read data from Cache or Main Memory */
                    log(name(), "READ WRITE ALLOCATE from Cache", req_cache_id, "address", req_addr);

                    for (Cache* cache : cache_list) {
                        log(name(), "READ WRITE ALLOCATE request for Cache ", req_cache_id, "on Cache", cache->id);
                        if (cache->id != req_cache_id) {
                            if (cache->snoop_read(cache->id, req_addr, snoop_hit)) {
                                snoop_hit = true;
                            }
                        }
                    }
                    if (!snoop_hit) {
                        log(name(), "READ WRITE ALLOCATE FAILED SNOOP for Cache", req_cache_id, "address", req_addr);
                        memory->read_write_allocate(req_cache_id, req_addr);

                    } else {
                        log(name(), "READ WRITE ALLOCATE SNOOP HIT for Cache", req_cache_id, "address", req_addr);
                        std::vector<uint64_t> res = {req_cache_id, req_addr, ResponseType::READ_WRITE_ALLOCATE_RESPONSE};
                        responseQueue.push_front(res);
                    }
                    break;
            }
        }
        wait();
    }
}