#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "CACHE.h"
#include "psa.h"

/** 
 * "Snoops" the Bus for READ requests caused by READ MISSES or 
 * READS due to WRITE ALLOCATION. 
 * 
 * @param requester_id The ID of the Cache that requested the READ.
 * @param addr The address of the Cache Line to READ.
 * @param data_already_snooped True if the data was already snooped, False otherwise. Prevents multiple 
 * snoops on the same data.
 * 
 * @return bool True if the Cache Line was found in the Cache, False otherwise.
 * */
bool Cache::snoop_read(uint64_t requester_id, uint64_t addr, bool data_already_snooped) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;
    CacheState cache_line_state = CacheState::INVALID;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, cache_line_state, set_index, tag);

    if (cache_hit) {
        switch (cache_line_state) {
            case CacheState::INVALID:
                log(name(), "SNOOP READ MISS on INVALID STATE on tag", tag, "in set", set_index);
                return false;
            case CacheState::SHARED:
                log(name(), "SNOOP READ HIT on SHARED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::SHARED; // No state change

                if (!data_already_snooped) { bus->cache_snoop_read_response(requester_id, addr, data); }
                return true;
            case CacheState::EXCLUSIVE:
                log(name(), "SNOOP READ HIT on EXCLUSIVE STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::SHARED; // Change state to SHARED

                if (!data_already_snooped) { bus->cache_snoop_read_response(requester_id, addr, data); }
                return true;
            case CacheState::MODIFIED:
                log(name(), "SNOOP READ HIT on MODIFIED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::OWNED; // Change state to SHARED

                if (!data_already_snooped) { bus->cache_snoop_read_response(requester_id, addr, data); }
                return true;
            case CacheState::OWNED:
                log(name(), "SNOOP READ HIT on OWNED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::OWNED; // No state change

                if (!data_already_snooped) { bus->cache_snoop_read_response(requester_id, addr, data); }
                return true;
        }
        return false;
    }
    log(name(), "SNOOP READ MISS on tag", tag, "in set", set_index);
    return false;
}

bool Cache::snoop_read_allocate(uint64_t requester_id, uint64_t addr, bool data_already_snooped) {
    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;
    CacheState cache_line_state = CacheState::INVALID;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, cache_line_state, set_index, tag);

    if (cache_hit) {
        switch (cache_line_state) {
            case CacheState::INVALID:
                log(name(), "SNOOP READ MISS on INVALID STATE on tag", tag, "in set", set_index);
                return false;
            case CacheState::SHARED:
                log(name(), "SNOOP READ HIT on SHARED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::SHARED; // No state change

                if (!data_already_snooped) { bus->cache_snoop_read_allocate_response(requester_id, addr, data); }
                return true;
            case CacheState::EXCLUSIVE:
                log(name(), "SNOOP READ HIT on EXCLUSIVE STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::SHARED; // Change state to SHARED

                if (!data_already_snooped) { bus->cache_snoop_read_allocate_response(requester_id, addr, data); }
                return true;
            case CacheState::MODIFIED:
                log(name(), "SNOOP READ HIT on MODIFIED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::OWNED; // Change state to SHARED

                if (!data_already_snooped) { bus->cache_snoop_read_allocate_response(requester_id, addr, data); }
                return true;
            case CacheState::OWNED:
                log(name(), "SNOOP READ HIT on OWNED STATE on tag", tag, "in set", set_index);

                cache[set_index].lines[cache_hit_index].state = CacheState::OWNED; // No state change

                if (!data_already_snooped) { bus->cache_snoop_read_allocate_response(requester_id, addr, data); }
                return true;
        }
        return false;
    }
    log(name(), "SNOOP READ MISS on tag", tag, "in set", set_index);
    return false;
}

/**
 * "Snoops" the Bus for WRITE requests caused by WRITE HITS,
 * requiring that other Caches with matching Cache Lines be INVALIDATED.
 * 
 * @param requester_id The ID of the Cache that requested the WRITE.
 * @param addr The address of the Cache Line to WRITE.
 */
void Cache::snoop_invalidate(uint64_t requester_id, uint64_t addr) {

    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;
    uint64_t data;

    bool cache_hit = false;
    size_t cache_hit_index = -1;
    CacheState cache_line_state = CacheState::INVALID;

    decode_address(addr, set_index, tag, byte_in_line, data);

    cache_hit_check(cache_hit, cache_hit_index, cache_line_state, set_index, tag);

    if (cache_hit) {
        log(name(), "SNOOP HIT, INVALIDATE on tag", tag, "in set", set_index);
        cache[set_index].lines[cache_hit_index].state = CacheState::INVALID;
    } else {
        log(name(), "SNOOP MISS, NO INVALIDATE on tag", tag, "in set", set_index);
    }
}