#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include <vector>
#include <queue>

#include "bus_slave_if.h"
#include "Cache.h"
#include "psa.h"

struct BusRequestResponse {
    Cache* source_cache;
    uint64_t addr;
    int request_type;
    bool snoop_success;
};


class Bus : public bus_slave_if, public sc_module {
    public:
        sc_in<bool> clk;
        sc_port<bus_slave_if> memory;

        std::vector<Cache*> cache_list;

        std::queue<BusRequestResponse> requestQueue;
        std::queue<BusRequestResponse> responseQueue;
    
        SC_CTOR(Bus) {
            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();
            //dont_initialize();

            SC_THREAD(processResponsesQueue);
            sensitive << clk.pos();
            //dont_initialize();
        }
    
        int read(uint64_t addr, Cache* requester) {
            log(name(), "READ pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, 0, false};
            requestQueue.push(req);
            return 0;
        }
    
        int write(uint64_t addr, Cache* requester) {
            log(name(), "WRITE pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, 1, false};
            requestQueue.push(req);
            return 0;
        }

        int write_invalidate(uint64_t addr, Cache* requester) {
            log(name(), "WRITE INVALIDATE pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, 2, false};
            requestQueue.push(req);
            return 0;
        }

        void add_cache(Cache* new_cache) {
            cache_list.push_back(new_cache);
        }
    
    private:
        void processRequestQueue() {
            while(true) {
                if (!requestQueue.empty()) {
                    BusRequestResponse req = requestQueue.front();
                    requestQueue.pop();

                    log(name(), "processing REQUEST from CACHE", req.source_cache->id, " for address", req.addr);

                
                    assert((req.addr & 0x3) == 0);

                    if (req.request_type == 0) { // Search for data in caches
                        log(name(), "READ SNOOP for data from CACHE", req.source_cache->id);
                        for (auto& cache : cache_list) {
                            if (cache && cache != req.source_cache) {
                                req.snoop_success = cache->snoop(req.addr, false);
                            }
                        }
                        if (req.snoop_success) {
                            log(name(), "address HIT in SNOOPED caches, read from cache for address", req.addr);
                        } else {
                            log(name(), "address MISS in SNOOPED caches, read from Main Memory for address", req.addr);
                            memory->read(req.addr, req.source_cache);
                        }
                    } else if (req.request_type == 1) { // Write to Main Memory
                        log(name(), "WRITE to Main Memory from CACHE", req.source_cache->id);
                        memory->write(req.addr, req.source_cache);
                    } else if (req.request_type == 2) { // Write in local Cache invalidates all other caches
                        log(name(), "WRITE INVALIDATE from CACHE", req.source_cache->id);
                        for (auto& cache : cache_list) {
                            if (cache && cache != req.source_cache) {
                                req.snoop_success = cache->snoop(req.addr, true);
                            }
                        }
                    }

                    responseQueue.push(req);

                }
                wait();
            }
        }

        void processResponsesQueue() {
            while (true) {
                if (!responseQueue.empty()) {
                    BusRequestResponse response = responseQueue.front();
                    responseQueue.pop();
    
                    log(name(), "processing RESPONSE from CACHE", response.source_cache->id, " for address", response.addr);

                    /*
                        The Transaction Level Modeling (TLM) interface doesn't 
                        require a response to be sent back to the cache.
                    */
                    response.source_cache->notify_response();
                }

                wait();
            }
        }
    };

#endif
