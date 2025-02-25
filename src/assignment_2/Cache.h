#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <systemc.h>

#include "Memory.h"
#include "cpu_cache_if.h"
#include "helpers.h"
#include "cache_struct.h"
#include "constants.h"

// Class definition without the SC_ macro because we implement the
// cpu_cache_if interface.
class Cache : public cpu_cache_if, public sc_module {
    public:
    sc_port<bus_slave_if> memory;

    // cpu_cache interface methods.
    int cpu_read(uint64_t addr);
    int cpu_write(uint64_t addr);

    // Constructor without SC_ macro.
    Cache(sc_module_name name_, int id_) : sc_module(name_), id(id_) {
        // Passive for now, just handle cpu requests.
    }

    ~Cache() {
    }

    private:
    int id;

    CacheSet cache[NUM_SETS];

    void cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag);
    void update_lru(CacheSet &set, size_t index);
    size_t find_lru(CacheSet &set);
    void decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line);
    void set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, uint64_t data, uint64_t byte_in_line, bool valid, bool dirty);
};

#endif
