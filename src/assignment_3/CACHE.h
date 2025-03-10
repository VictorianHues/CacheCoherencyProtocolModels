#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <systemc.h>
#include <deque>

#include "cache_if.h"
#include "bus_if.h"
#include "cpu_if.h"

#include "helpers.h"
#include "cache_struct.h"
#include "constants.h"

class Cache : public cache_if, public sc_module {
    public:
        struct RequestType {
            static const uint64_t READ = 0;
            static const uint64_t WRITE = 1;
        };
        struct ResponseType {
            static const uint64_t BUS_READ_RESPONSE_CACHE = 0;
            static const uint64_t BUS_READ_RESPONSE_MEM = 1;
            static const uint64_t READ_FOR_WRITE_ALLOCATE = 2;
            static const uint64_t WRITE_TO_MAIN_MEM = 3;
            static const uint64_t INVALIDATE_RESPONSE = 4;
        };

        sc_in<bool> clk;
        sc_port<bus_if> bus;
        sc_port<cpu_if> cpu;

        sc_event bus_arbitration;

        uint64_t id;

        std::deque<std::vector<uint64_t>> requestQueue;
        std::deque<std::vector<uint64_t>> responseQueue;

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
        
        /* Interface Start */
        void cpu_read(uint64_t addr);
        void cpu_write(uint64_t addr);

        void snoop_read_response_cache(uint64_t addr, uint64_t data);
        void snoop_read_response_mem(uint64_t addr, uint64_t data);
        void snoop_invalidate_response(uint64_t addr);

        void read_for_write_allocate_response(uint64_t addr, uint64_t data);
        void write_to_main_memory_complete(uint64_t addr);

        bool snoop_read(uint64_t requester_id, uint64_t addr);
        void snoop_invalidate(uint64_t requester_id, uint64_t addr);

        void bus_arbitration_notification() {
            bus_arbitration.notify();
            //log(name(), "BUS ARBITRATION NOTIFICATION on CACHE", id);
        }
        
        void wait_for_bus_arbitration() {
            bus->cache_notify_bus_arbitration(id);
            
            while (!bus_arbitration.triggered()) {
                wait(clk.negedge_event());
                //log(name(), "waiting for Bus arbitration...");
            }
            bus_arbitration.cancel();
        }
        /* Interface End */

        bool system_busy(){
            return !requestQueue.empty() || !requestQueue.empty() || bus->system_busy();
        }
    private:
        CacheSet cache[NUM_SETS];

        /* Helper Functions */
        void cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag);
        void update_lru(CacheSet &set, size_t index);
        size_t find_lru(CacheSet &set);
        void decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line, uint64_t &data);
        void set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, uint64_t data, uint64_t byte_in_line, bool valid, bool dirty);

        /* Processing Threads */
        void processRequestQueue();
        void processResponseQueue();
};

#endif
