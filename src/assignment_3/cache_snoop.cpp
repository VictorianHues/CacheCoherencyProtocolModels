#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

void Cache::snoop_read_response_cache(uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::BUS_READ_RESPONSE_CACHE};
    responseQueue.push_front(res);
}

void Cache::snoop_read_response_mem(uint64_t addr, uint64_t data) {
    log(name(), "SNOOP READ RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::BUS_READ_RESPONSE_MEM};
    responseQueue.push_front(res);
}

void Cache::snoop_invalidate_response(uint64_t addr) {
    log(name(), "SNOOP INVALIDATE RESPONSE on address", addr);

    wait(CACHE_CYCLE_LATENCY, SC_NS);

    std::vector<uint64_t> res = {addr, ResponseType::INVALIDATE_RESPONSE};
    responseQueue.push_front(res);
}

bool Cache::snoop_read(uint64_t requester_id, uint64_t addr) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;
    bool cache_line_valid = false;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    if (cache_hit) {
        cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
        if (cache_line_valid) {
            log(name(), "SNOOP READ HIT on tag", tag, "in set", set_index);
            
            bus->cache_snoop_read_response(requester_id, addr, data);
            return true;
        }
    }
    log(name(), "SNOOP READ MISS on tag", tag, "in set", set_index);
    return false;
}

void Cache::snoop_invalidate(uint64_t requester_id, uint64_t addr) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    if (cache_hit) {
        log(name(), "SNOOP HIT, INVALIDATE on tag", tag, "in set", set_index);
        cache[set_index].lines[cache_hit_index].valid = false;
        cache[set_index].lines[cache_hit_index].dirty = false;
    } else {
        log(name(), "SNOOP MISS, NO INVALIDATE on tag", tag, "in set", set_index);
    }
}