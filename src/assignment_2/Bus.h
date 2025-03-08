#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_if.h"
#include "memory_if.h"
#include "Cache.h"
#include "psa.h"

class Bus : public bus_if, public sc_module {
    public:
        struct RequestType {
            static const uint64_t READ = 0;
            static const uint64_t WRITE_TO_MAIN_MEM = 1;
            static const uint64_t INVALIDATE = 2;
            static const uint64_t SNOOP_READ_RESPONSE = 3;
            static const uint64_t READ_WRITE_ALLOCATE = 4;
        };
        struct ResponseType {
            static const uint64_t SNOOP_READ_RESPONSE_MEM = 1;
            static const uint64_t SNOOP_READ_RESPONSE_CACHE = 2;
            static const uint64_t READ_WRITE_ALLOCATE_RESPONSE = 3;
            static const uint64_t WRITE_TO_MAIN_MEM_RESPONSE = 4;
        };

        sc_in<bool> clk;
        sc_port<memory_if> memory;

        std::vector<Cache*> cache_list;

        std::deque<std::vector<uint64_t>> requestQueue;
        std::deque<std::vector<uint64_t>> responseQueue;
    
        SC_CTOR(Bus) {
            SC_THREAD(bus_arbitration_thread);
            sensitive << clk.pos();

            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();
            //dont_initialize();

            SC_THREAD(processResponsesQueue);
            sensitive << clk.pos();
            //dont_initialize();
        }

        bool system_busy() {
            return !requestQueue.empty() || !responseQueue.empty() || memory->system_busy();
        }

        void add_cache(Cache* new_cache) {
            cache_list.push_back(new_cache);
        }
    
        void read(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ pushed to queue from Cache", requester_id, "for address", addr);

            std::vector<uint64_t> req = {requester_id, addr, RequestType::READ};
            requestQueue.push_back(req);
        }
    
        void write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "WRITE to Main Memory pushed to queue from Cache", requester_id, "for address", addr);

            std::vector<uint64_t> req = {requester_id, addr, RequestType::WRITE_TO_MAIN_MEM};
            requestQueue.push_back(req);
        }

        void read_for_write_allocate(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ from Main Memory for WRITE ALLOCATE from Cache", requester_id, "for address", addr);

            std::vector<uint64_t> req = {requester_id, addr, RequestType::READ_WRITE_ALLOCATE};
            requestQueue.push_back(req);
        }

        void broadcast_invalidate(uint64_t requester_id, uint64_t addr) {
            log(name(), "BROADCAST INVALIDATE pushed to queue from Cache", requester_id, "for address", addr);

            std::vector<uint64_t> req = {requester_id, addr, RequestType::INVALIDATE};
            requestQueue.push_back(req);
        }



        /* RESPONSES FROM MODULES */

        void mem_read_write_allocate_complete(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "READ WRITE ALLOCATE RESPONSE pushed to queue for Cache", requester_id, "address", addr);

            // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
            std::vector<uint64_t> res = {requester_id, addr, ResponseType::READ_WRITE_ALLOCATE_RESPONSE};
            responseQueue.push_back(res);
        }

        void mem_read_failed_snoop_complete(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "SNOOP READ RESPONSE pushed to queue for Cache", requester_id, "address", addr);

            // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
            std::vector<uint64_t> res = {requester_id, addr, ResponseType::SNOOP_READ_RESPONSE_MEM};
            responseQueue.push_back(res);
        }

        void mem_write_to_main_memory_complete(uint64_t requester_id, uint64_t addr) {
            log(name(), "WRITE to Main Memory RESPONSE pushed to queue for Cache", requester_id, "address", addr);

            std::vector<uint64_t> res = {requester_id, addr, ResponseType::WRITE_TO_MAIN_MEM_RESPONSE};
            responseQueue.push_back(res);
        }

        void cache_snoop_read_response(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "SNOOP READ RESPONSE pushed to queue for Cache", requester_id, "address", addr);

            // Literal Data transfer stops here, but could be implemented to complete the transfer to the Cache properly
            std::vector<uint64_t> res = {requester_id, addr, ResponseType::SNOOP_READ_RESPONSE_CACHE};
            responseQueue.push_back(res);
        }

    private:
        int last_served_cache_id = 0;

        void bus_arbitration_thread() {
            while (true) {
                last_served_cache_id = (last_served_cache_id + 1) % cache_list.size();
                cache_list[last_served_cache_id]->bus_arbitration_notification();
                wait();
            }
        }

        void processRequestQueue() {
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

        void processResponsesQueue() {
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
                        if (cache->id == res_cache_id) {
                            switch (res_type) {
                                case ResponseType::SNOOP_READ_RESPONSE_MEM: // Bus read response from Main Memory after Cache read miss
                                    cache->snoop_read_response_mem(res_addr, data);
                                    break;
                                case ResponseType::SNOOP_READ_RESPONSE_CACHE: // Bus read response from parallel cache after Cache read miss
                                    cache->snoop_read_response_cache(res_addr, data);
                                    break;
                                case ResponseType::READ_WRITE_ALLOCATE_RESPONSE:
                                    cache->read_for_write_allocate_response(res_addr, data);
                                    break;
                                case ResponseType::WRITE_TO_MAIN_MEM_RESPONSE:
                                    cache->write_to_main_memory_complete(res_addr);
                                    break;
                            }
                        }
                    }
                }
                wait();
            }
        }
    };

#endif
