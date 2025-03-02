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
    enum RequestType { READ, WRITE, WRITE_INVALIDATE } request_type;
    bool snoop_success;
};


class Bus : public bus_slave_if, public sc_module {
    /*
        Bus class that implements the bus_slave_if interface to allow
        communication between the Cache, Bus, and Memory modules.

        The Bus class is responsible for handling requests from the Cache
        modules and sending them to the Memory module and other Cache Modules. 
        The TLM interface doesn't require actual data to be sent back to the
        cache, nor does it require data from Main Memory.

        The Bus class maintains a queue of requests from the Cache modules
        and a queue of responses from the Memory Module or other Caches. 
        It processes the requests and responses in separate threads to 
        allow for concurrent operation.

        The Bus class also maintains a list of Cache modules that are connected
        to the Bus. This allows for the Bus to send requests to all connected
        Cache modules as part of the Snooping process.

        The Bus class is implemented as a SystemC module and uses the SystemC
        TLM interface for communication with the Cache and Memory modules.
    */
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
            /*
                Read request from Cache module to the Bus.
                Pushes the request to the requestQueue for processing.

                Parameters:
                    addr: address to read from
                    requester: Cache module that is requesting the read

                Returns:
                    0
            */
            log(name(), "READ pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, BusRequestResponse::READ, false};
            requestQueue.push(req);
            return 0;
        }
    
        int write(uint64_t addr, Cache* requester) {
            /*
                Write request from Cache module to the Bus.
                Pushes the request to the requestQueue for processing.

                Parameters:
                    addr: address to write to
                    requester: Cache module that is requesting the write

                Returns:
                    0
            */
            log(name(), "WRITE pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, BusRequestResponse::WRITE, false};
            requestQueue.push(req);
            return 0;
        }

        int write_invalidate(uint64_t addr, Cache* requester) {
            /*
                Write Invalidate request from Cache module to the Bus.
                Pushes the request to the requestQueue for processing.

                Parameters:
                    addr: address to write to
                    requester: Cache module that is requesting the write

                Returns:
                    0
            */
            log(name(), "WRITE INVALIDATE pushed to queue for address", addr);
            BusRequestResponse req = {requester, addr, BusRequestResponse::WRITE_INVALIDATE, false};
            requestQueue.push(req);
            return 0;
        }

        void add_cache(Cache* new_cache) {
            /*
                Add a new Cache module to the list of connected Cache modules.

                Parameters:
                    new_cache: Cache module to add to the list of connected Cache modules
            */
            cache_list.push_back(new_cache);
        }
    
    private:
        void processRequestQueue() {
            /*
                Process the requestQueue to handle requests from the Cache modules.
                The Bus class processes the requests and sends them to the Memory module
                or other Cache modules as needed.

                The Bus class uses the TLM interface to communicate with the Cache and
                Memory modules. The Bus class doesn't require a response to be sent back
                to the Cache modules.

                The Bus class processes the requests in a loop and waits for the next
                request to be available in the requestQueue.
            */
            while(true) {
                if (!requestQueue.empty()) {
                    BusRequestResponse req = requestQueue.front();
                    requestQueue.pop();

                    log(name(), "processing REQUEST from CACHE", req.source_cache->id, " for address", req.addr);

                
                    assert((req.addr & 0x3) == 0);

                    if (req.request_type == BusRequestResponse::READ) { // Search for data in caches
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
                    } else if (req.request_type == BusRequestResponse::WRITE) { // Write to Main Memory
                        log(name(), "WRITE to Main Memory from CACHE", req.source_cache->id);
                        memory->write(req.addr, req.source_cache);
                    } else if (req.request_type == BusRequestResponse::WRITE_INVALIDATE) { // Write in local Cache invalidates all other caches
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
            /*
                Process the responseQueue to handle responses from the Memory module
                or other Cache modules. The Bus class processes the responses and
                sends them to the Cache modules as needed.

                The Bus class uses the TLM interface to communicate with the Cache and
                Memory modules. The Bus class doesn't require a response to be sent back
                to the Cache modules.

                The Bus class processes the responses in a loop and waits for the next
                response to be available in the responseQueue.
            */
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
