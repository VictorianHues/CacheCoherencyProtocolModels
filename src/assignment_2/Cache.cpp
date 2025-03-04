#include <assert.h>
#include <systemc.h>
#include <unordered_map>

#include "Cache.h"
#include "psa.h"
#include "request_response_struct.h"

void Cache::cache_hit_check(bool &cache_hit, size_t &cache_hit_index, int set_index, uint64_t tag) {
    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) { // Iterate over lines in set
        if (cache[set_index].lines[i].tag == tag) {
            cache_hit = true; // Cache hit
            cache_hit_index = i; // Cache hit index

            break;
        }
    }
}

void Cache::update_lru(CacheSet &cache_set, size_t index) {
    size_t current = cache_set.lru[index];

    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (i != index && cache_set.lru[i] < current) {
            cache_set.lru[i]++;
        }
    }
    cache_set.lru[index] = 0;
}

size_t Cache::find_lru(CacheSet &cache_set) {
    size_t max_index = 0;

    for (size_t i = 1; i < SET_ASSOCIATIVITY; i++) {
        if (cache_set.lru[i] > cache_set.lru[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

void Cache::decode_address(uint64_t addr, int &set_index, uint64_t &tag, uint64_t &byte_in_line) {
    tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
    set_index = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
    byte_in_line = addr % LINE_SIZE; // address modulo the line size
}

void Cache::set_cache_line(int set_index, size_t cache_hit_index, uint64_t tag, 
                            uint64_t data, uint64_t byte_in_line, bool valid, bool dirty) {
    log(name(), "SETTING CACHE LINE", cache_hit_index, "in set", set_index);
    log(name(), "tag", tag, "data", data, "byte", byte_in_line, "valid", valid, "dirty", dirty);

    cache[set_index].lines[cache_hit_index].tag = tag; // Set tag
    cache[set_index].lines[cache_hit_index].valid = valid; // Set valid bit
    cache[set_index].lines[cache_hit_index].dirty = dirty; // Set dirty bit
    cache[set_index].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data

    update_lru(cache[set_index], cache_hit_index);
    cout << sc_time_stamp() << ": UPDATED LRU Queue: " << cache[set_index].lru[0] << " " << cache[set_index].lru[1] << " " << cache[set_index].lru[2] << " " << cache[set_index].lru[3] << " " << cache[set_index].lru[4] << " " << cache[set_index].lru[5] << " " << cache[set_index].lru[6] << " " << cache[set_index].lru[7] << endl;
}

RequestResponse Cache::initialize_request_response(uint64_t addr, RequestResponse::RequestType request_type) {
    int cache_id = id;

    uint64_t tag;
    int set_index;
    uint64_t byte_in_line;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    decode_address(addr, set_index, tag, byte_in_line);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    bool cache_line_valid = cache[set_index].lines[cache_hit_index].valid;

    return {cache_id, addr, tag, set_index, byte_in_line, cache_hit, cache_hit_index, cache_line_valid, request_type};
}

int Cache::cpu_read(uint64_t addr) {
    log(name(), "CPU READ on Cache", id);
    RequestResponse req = initialize_request_response(addr, RequestResponse::READ);

    requestQueue.push_back(req);

    log(name(), "READ REQUEST on tag", req.tag, "in set", req.set_index, "type", req.request_type);

    return 0;
}

int Cache::cpu_write(uint64_t addr) {
    log(name(), "CPU WRITE on Cache", id);
    RequestResponse req = initialize_request_response(addr, RequestResponse::WRITE);

    requestQueue.push_back(req);

    log(name(), "WRITE REQUEST on tag", req.tag, "in set", req.set_index, "type", req.request_type);

    return 0;
}

void Cache::processRequestQueue() {
    while(true) {
        if (!requestQueue.empty()) {
            log(name(), "PROCESSING REQUEST QUEUE on Cache", id);
            RequestResponse req = requestQueue.front();
            requestQueue.pop_front();

            //uint64_t data = 128; // Placeholder data

            switch (req.request_type) {
                case RequestResponse::READ:
                    if (req.cache_hit && req.line_valid) {
                        log(name(), "READ HIT on address on tag", req.tag, "in set", req.set_index);
                        responseQueue.push_back(req);
                        stats_readhit(id);
                    } else {
                        log(name(), "READ MISS or INVALID on tag", req.tag, "in set", req.set_index);

                        bus->read(req);
                        stats_readmiss(id);
                    }
                    break;
                case RequestResponse::WRITE:
                    if (req.cache_hit) {
                        log(name(), "WRITE HIT on tag", req.tag, "in set", req.set_index);
                        responseQueue.push_back(req);
                        stats_writehit(id);
                    } else {
                        log(name(), "WRITE MISS on tag", req.tag, "in set", req.set_index);
                        log(name(), "WRITE ALLOCATE on tag", req.tag, "in set", req.set_index);

                        bus->read(req);

                        stats_writemiss(id);
                    }
                    break;
                case RequestResponse::WRITE_INVALIDATE:
                    break;
                case RequestResponse::INVALIDATE_SELF:
                    log(name(), "INVALIDATING SELF on tag", req.tag, "in set", req.set_index);
                    cache[req.set_index].lines[req.cache_hit_index].valid = false;
                    break;
            }
        }
        wait();
    }
}

void Cache::processResponseQueue() {
    while(true) {
        if (!responseQueue.empty()) {
            RequestResponse res = responseQueue.front();
            responseQueue.pop_front();

            uint64_t data = 128; // Placeholder data

            switch (res.request_type) {
                case RequestResponse::READ:
                    log(name(), "READ RESPONSE on tag", res.tag, "in set", res.set_index);

                    if (!res.cache_hit || !res.line_valid) {
                        res.cache_hit_index = find_lru(cache[res.set_index]);
                        log(name(), "EVICTION and reWRITE of LRU line", res.cache_hit_index, "in set", res.set_index);
                        set_cache_line(res.set_index, res.cache_hit_index, res.tag, data, res.byte_in_line, true, false);
                    } else {
                        log(name(), "line valid for READ on tag", res.tag, "in set", res.set_index);
                    }
                    break;
                case RequestResponse::WRITE:
                    log(name(), "WRITE RESPONSE on tag", res.tag, "in set", res.set_index);

                    if (!res.cache_hit || !res.line_valid) {
                        res.cache_hit_index = find_lru(cache[res.set_index]);
                        log(name(), "EVICTION of LRU line", res.cache_hit_index, "in set", res.set_index);
                    }

                    log(name(), "WRITE on tag", res.tag, "in set", res.set_index);
                    set_cache_line(res.set_index, res.cache_hit_index, res.tag, data, res.byte_in_line, true, false);

                    bus->write_invalidate(res);  // Invalidate other caches

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


bool Cache::snoop(RequestResponse req_res, int bus_action) {
    
    if (req_res.cache_hit && req_res.line_valid) {
        log(name(), "SNOOP HIT on tag", req_res.tag, "in set", req_res.set_index);

        if (bus_action == 0) {
            log(name(), "SNOOP READ on tag", req_res.tag, "in set", req_res.set_index);
            return true;
        } else if (bus_action == 2) {
            log(name(), "SNOOP WRITE INVALIDATE on tag", req_res.tag, "in set", req_res.set_index);
            cache[req_res.set_index].lines[req_res.cache_hit_index].valid = false;
            return true;
        }
    } else {
        log(name(), "SNOOP MISS on tag", req_res.tag, "in set", req_res.set_index);
    }
    return false;
}


void Cache::notify_response(RequestResponse res) {
    
    if (res.request_type == RequestResponse::READ) {
        log(name(), "NOTIFY READ RESPONSE on tag", res.tag, "in set", res.set_index);
        responseQueue.push_back(res);
    } else if (res.request_type == RequestResponse::WRITE) {
        log(name(), "NOTIFY WRITE RESPONSE on tag", res.tag, "in set", res.set_index);
        responseQueue.push_back(res);
    }
}