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
 * RESPONSE from MAIN MEMORY after a READ for WRITE ALLOCATION.
 * read_for_write_allocate -> mem_read_write_allocate_complete
 * 
 * @param requester_id The ID of the Cache that requested the READ WRITE ALLOCATE
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from Main Memory.
 */
void Bus::mem_read_write_allocate_complete(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "READ WRITE ALLOCATE RESPONSE pushed to queue for Cache", requester_id, "address", addr);

    // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
    std::vector<uint64_t> res = {requester_id, addr, ResponseType::READ_WRITE_ALLOCATE_RESPONSE};
    responseQueue.push_back(res);
}

/**
 * RESPONSE from MAIN MEMORY after a READ MISS into a SNOOP READ FAILURE.
 * read -> snoop_read -> mem_read_failed_snoop_complete
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from Main Memory.
 */
void Bus::mem_read_failed_snoop_complete(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "FAILED SNOOP MAIN MEM READ RESPONSE pushed to queue for Cache", requester_id, "address", addr);

    // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
    std::vector<uint64_t> res = {requester_id, addr, ResponseType::SNOOP_READ_RESPONSE_MEM};
    responseQueue.push_back(res);
}

/**
 * RESPONSE from MAIN MEMORY after a direct WRITE to MAIN MEMORY.
 * write_to_main_memory -> mem_write_to_main_memory_complete
 * 
 * @param requester_id The ID of the Cache that requested the WRITE.
 * @param addr The address of the Cache Line to WRITE.
 */
void Bus::mem_write_to_main_memory_complete(uint64_t requester_id, uint64_t addr) {
    log(name(), "WRITE to Main Memory RESPONSE pushed to queue for Cache", requester_id, "address", addr);

    std::vector<uint64_t> res = {requester_id, addr, ResponseType::WRITE_TO_MAIN_MEM_RESPONSE};
    responseQueue.push_back(res);
}

/**
 * RESPONSE from a Cache after a SUCCESSFUL SNOOP READ request.
 * snoop_read -> cache_snoop_read_response
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from the Cache.
 */
void Bus::cache_snoop_read_response(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE pushed to queue for Cache", requester_id, "address", addr);

    // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
    std::vector<uint64_t> res = {requester_id, addr, ResponseType::SNOOP_READ_RESPONSE_CACHE};
    responseQueue.push_back(res);
}

/**
 * RESPONSE from a Cache after a SUCCESSFUL SNOOP READ ALLOCATE request.
 * snoop_read_allocate -> cache_snoop_read_allocate_response
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 * @param data The data read from the Cache.
 */
void Bus::cache_snoop_read_allocate_response(uint64_t requester_id, uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ ALLOCATE RESPONSE pushed to queue for Cache", requester_id, "address", addr);

    // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
    std::vector<uint64_t> res = {requester_id, addr, ResponseType::READ_WRITE_ALLOCATE_RESPONSE};
    responseQueue.push_back(res);
}

/**
 * Process the Request Queue for the Bus as a SystemC Thread.
 */
void Bus::processResponsesQueue() {
    while (true) {
        if (!responseQueue.empty()) {
            std::vector<uint64_t> res = responseQueue.front();
            responseQueue.pop_front();

            uint64_t res_cache_id = res[0];
            uint64_t res_addr = res[1];
            uint64_t res_type = res[2];
            uint64_t data = 128; // Placeholder data

            log(name(), "PROCESSING RESPONSE QUEUE on Cache", res_cache_id, "for address", res_addr);

            for (Cache* cache : cache_list) {
                if (cache->id == res_cache_id) { // Find the Cache that requested the response
                    switch (res_type) {
                        case ResponseType::SNOOP_READ_RESPONSE_MEM: // Bus read response from Main Memory after Cache read miss
                            cache->snoop_read_response_mem(res_addr, data);
                            break;
                        case ResponseType::SNOOP_READ_RESPONSE_CACHE: // Bus read response from parallel cache after Cache read miss
                            cache->snoop_read_response_cache(res_addr, data);
                            break;
                        case ResponseType::READ_WRITE_ALLOCATE_RESPONSE:
                            cache->read_for_write_allocate_response(res_addr, data); // Bus read response from Main Memory for WRITE ALLOCATE
                            break;
                        case ResponseType::WRITE_TO_MAIN_MEM_RESPONSE: // Bus write response to Main Memory
                            cache->write_to_main_memory_complete(res_addr);
                            break;
                    }
                }
            }
        }
        wait();
    }
}