#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <systemc.h>
#include <unordered_map>
#include <deque>

#include "Memory.h"
#include "cpu_cache_if.h"
#include "helpers.h"
#include "cache_struct.h"
#include "constants.h"
#include "request_response_struct.h"

class Cache : public cpu_cache_if, public sc_module {
    public:
        int id;
        sc_time time_idle = SC_ZERO_TIME;
        sc_event response_event;

        bool new_write_pending = false;

        std::deque<RequestResponse> requestQueue;
        std::deque<RequestResponse> responseQueue;

        sc_in<bool> clk;
        sc_port<bus_slave_if> bus;

        int cpu_read(uint64_t addr);
        int cpu_write(uint64_t addr);


        SC_CTOR(Cache) {
            log(name(), "constructed with id", id);
        }

        Cache(sc_core::sc_module_name name, int cache_id) : sc_module(name), id(cache_id) {
            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();

            SC_THREAD(processResponseQueue);
            sensitive << clk.pos();
            //dont_initialize();
        }
        

        void notify_response(RequestResponse res);
        bool snoop(RequestResponse req_res, int bus_action);


        int system_busy() {
            return !requestQueue.empty() || !requestQueue.empty() || bus->system_busy();
        }

        sc_time get_time_idle() {
            return time_idle;
        }
    private:
        CacheSet cache[NUM_SETS];

        void cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag);
        void update_lru(CacheSet &set, size_t index);
        size_t find_lru(CacheSet &set);
        void decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line);
        void set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, uint64_t data, uint64_t byte_in_line, bool valid, bool dirty);

        void wait_for_bus_response();
        void processRequestQueue();
        void processResponseQueue();

        RequestResponse initialize_request_response(uint64_t addr, RequestResponse::RequestType request_type);
};

#endif
