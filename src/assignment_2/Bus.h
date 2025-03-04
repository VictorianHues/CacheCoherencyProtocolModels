#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_slave_if.h"
#include "Cache.h"
#include "psa.h"
#include "request_response_struct.h"

class Bus : public bus_slave_if, public sc_module {
    public:
        sc_in<bool> clk;
        sc_port<bus_slave_if> memory;

        std::vector<Cache*> cache_list;

        std::queue<std::pair<RequestResponse, int>> requestQueue;
        std::queue<std::pair<RequestResponse, int>> responseQueue;
    
        SC_CTOR(Bus) {
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
    
        int read(RequestResponse req) {
            log(name(), "READ pushed to queue from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);
            std::pair<RequestResponse, int> req_with_bus_action = {req, 0};
            requestQueue.push(req_with_bus_action);
            return 0;
        }
    
        int write(RequestResponse req, uint64_t data) {
            log(name(), "WRITE pushed to queue from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);
            std::pair<RequestResponse, int> req_with_bus_action = {req, 1};
            requestQueue.push(req_with_bus_action);
            return 0;
        }

        int write_invalidate(RequestResponse req) {
            log(name(), "WRITE INVALIDATE pushed to queue from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);
            std::pair<RequestResponse, int> req_with_bus_action = {req, 2};
            requestQueue.push(req_with_bus_action);
            return 0;
        }

        void add_cache(Cache* new_cache) {
            cache_list.push_back(new_cache);
        }

        void notify_response(RequestResponse res) {
            log(name(), "received response from MAIN MEMORY");
            std::pair<RequestResponse, int> res_with_bus_action = {res, 3};
            responseQueue.push(res_with_bus_action);
        }
    
    private:
        void processRequestQueue() {
            while(true) {
                if (!requestQueue.empty()) {
                    std::pair<RequestResponse, int> req_with_bus_action = requestQueue.front();
                    requestQueue.pop();

                    RequestResponse req = req_with_bus_action.first;
                    int bus_action = req_with_bus_action.second;
                    
                    log(name(), "processing request queue for Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);

                    uint64_t data = 128; // Placeholder data
                    bool snoop_hit = false;

                    log(name(), "processing request from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);

                    switch (bus_action) {
                        case 0:
                            for (Cache* cache : cache_list) {
                                log(name(), "READ SNOOPING from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);
                                if (cache->id != req.cache_id) {
                                    if (cache->snoop(req, bus_action)) {
                                        log(name(), "SNOOP HIT on Cache", cache->id, "from CACHE", req.cache_id, "for tag", req.tag, "in set", req.set_index);
                                        snoop_hit = true;
                                    }
                                }
                            }
                            if (!snoop_hit) {
                                memory->read(req);
                            }
                            break;
                        case 1:
                            memory->write(req, data);
                            break;
                        case 2:
                            for (Cache* cache : cache_list) {
                                log(name(), "INVALIDATION SNOOPING from Cache", req.cache_id, "for tag", req.tag, "in set", req.set_index);
                                if (cache->id != req.cache_id) {
                                    if (cache->snoop(req, bus_action)) {
                                        log(name(), "SNOOP HIT on Cache", cache->id, "from CACHE", req.cache_id, "for tag", req.tag, "in set", req.set_index);
                                        snoop_hit = true;
                                    }
                                }
                            }
                            break;
                    }
                }
                wait();
            }
        }

        void processResponsesQueue() {
            while (true) {
                if (!responseQueue.empty()) {
                    std::pair<RequestResponse, int> res_with_bus_action = responseQueue.front();
                    responseQueue.pop();

                    RequestResponse res = res_with_bus_action.first;
                    //int bus_action = res_with_bus_action.second;

                    log(name(), "processing response queue for Cache", res.cache_id, "for tag", res.tag, "in set", res.set_index);

                    switch (res.request_type) {
                        case RequestResponse::READ:
                            for (Cache* cache : cache_list) {
                                if (cache->id == res.cache_id) {
                                    cache->notify_response(res);
                                }
                            }
                            break;
                        case RequestResponse::WRITE:
                            for (Cache* cache : cache_list) {
                                if (cache->id == res.cache_id) {
                                    cache->notify_response(res);
                                }
                            }
                            break;
                        case RequestResponse::WRITE_INVALIDATE:
                            break;
                        case RequestResponse::INVALIDATE_SELF:
                            break;
                    }
                }

                wait();
            }
        }
    };

#endif
