#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <systemc.h>

#include "Memory.h"
#include "cpu_cache_if.h"
#include "helpers.h"
#include "cache_struct.h"
#include "constants.h"

class Cache : public cpu_cache_if, public sc_module {
    /*
        Cache class that implements the cpu_cache_if interface to allow
        communication between the CPU and Cache modules.

        The Cache class is responsible for handling requests from the CPU
        and checking if a Cache Hit or Cache Miss occurs and modifying Cache 
        Lines accordingly, sending them to the Bus module if otherwise. 
        The Cache class maintains a list of Cache Sets, each with a number 
        of Cache Lines. The Cache class is responsible for checking if a 
        Cache Hit or Cache Miss occurs and updating the Cache Line accordingly.

        The Cache class is implemented as a SystemC module and uses the
        SystemC TLM interface for communication with the CPU and Bus modules.
    */
    public:
        int id;
        sc_time time_idle = SC_ZERO_TIME;

        sc_in<bool> clk;
        //sc_port<bus_slave_if> memory;
        sc_port<bus_slave_if> bus;

        sc_event response_event;

        // cpu_cache interface methods.
        void read_hit(int set_index, size_t &cache_hit_index, 
            uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr);
        void read_miss(int set_index, size_t &cache_hit_index, 
            uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr);
        void write_hit(int set_index, size_t &cache_hit_index, 
            uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr);
        void write_miss(int set_index, size_t &cache_hit_index, 
            uint64_t tag, uint64_t data, uint64_t byte_in_line, uint64_t addr);

        int cpu_read(uint64_t addr);
        int cpu_write(uint64_t addr);

        // Constructor without SC_ macro.
        Cache(sc_module_name name_, int id_) : sc_module(name_), id(id_) {
            // Passive for now, just handle cpu requests.
        }

        ~Cache() {
        }

        void notify_response() { 
            log(name(), "received response from BUS");
            response_event.notify(); // Called by Bus when request completes
            log(name(), "notified response event");
        }

        sc_time get_time_idle() {
            return time_idle;
        }

        bool snoop(uint64_t addr, bool is_write);

    private:
        CacheSet cache[NUM_SETS];

        void cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag);
        void update_lru(CacheSet &set, size_t index);
        size_t find_lru(CacheSet &set);
        void decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line);
        void set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, uint64_t data, uint64_t byte_in_line, bool valid, bool dirty);
        void eviction_write_back_check(int set_index, size_t cache_hit_index, uint64_t byte_in_line);
        void wait_for_bus_response();
};

#endif
