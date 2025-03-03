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

    //log(name(), "SET INDEX", set_index, "TAG", tag, "BYTE IN LINE", byte_in_line);

    cache_hit_check(cache_hit, cache_hit_index, set_index, tag);

    bool cache_line_valid = false;
    if (cache_hit) {
        cache_line_valid = cache[set_index].lines[cache_hit_index].valid;
    }

    //log(name(), "CACHE line VALID", cache_line_valid);
    //log(name(), "CACHE HIT bool", cache_hit, "CACHE HIT INDEX", cache_hit_index);

    return {cache_id, addr, tag, set_index, byte_in_line, cache_hit, cache_hit_index, cache_line_valid, request_type};
}

int Cache::cpu_read(uint64_t addr) {
    log(name(), "CPU READ on Cache", id);
    RequestResponse req = initialize_request_response(addr, RequestResponse::READ);
    log(name(), "READ REQUEST on tag", req.tag, "in set", req.set_index, "type", RequestResponse::READ);

    if (pending_writes_queue[req.addr].size() > 0) {
        pending_reads_queue[req.addr].back().push_back(req);
    }  else {
        if (pending_reads_queue[req.addr].empty()) {
            pending_reads_queue[req.addr].emplace_back();
            requestQueue.push_back(req);
        }
        pending_reads_queue[req.addr].back().push_back(req);
    }               

    log(name(), "READ pending queue of size", pending_reads_queue[req.addr].back().size());
    log(name(), "READ vectors of pending queues of size", pending_reads_queue[req.addr].size());
    log(name(), "WRITE pending queue of size", pending_writes_queue[req.addr].size());


    return 0;
}

int Cache::cpu_write(uint64_t addr) {
    log(name(), "CPU WRITE on Cache", id);
    RequestResponse req = initialize_request_response(addr, RequestResponse::WRITE);
    log(name(), "WRITE REQUEST on tag", req.tag, "in set", req.set_index, "type", RequestResponse::WRITE);

    if (!pending_reads_queue[req.addr].empty()) {
        pending_reads_queue[req.addr].emplace_back();
        pending_writes_queue[req.addr].push_back(req);
    } else {
        requestQueue.push_back(req);
    }

    log(name(), "READ pending queue of size", pending_reads_queue[req.addr].back().size());
    log(name(), "READ vectors of pending queues of size", pending_reads_queue[req.addr].size());
    log(name(), "WRITE pending queue of size", pending_writes_queue[req.addr].size());

    return 0;
}

void Cache::processRequestQueue() {
    while(true) {
        if (!requestQueue.empty()) {
            RequestResponse req = requestQueue.front();
            requestQueue.pop_front();

            req = initialize_request_response(req.addr, req.request_type);

            //uint64_t data = 128; // Placeholder data

            if (req.request_type == RequestResponse::READ) {
                if (req.cache_hit && req.line_valid) {
                    log(name(), "READ HIT on address on tag", req.tag, "in set", req.set_index);

                    responseQueue.push_back(req);
                } else {
                    log(name(), "READ MISS or INVALID on tag", req.tag, "in set", req.set_index);

                    if (pending_reads_queue.find(req.addr) == pending_reads_queue.end()) {
                        log(name(), "No READS pending, sending READ to MAIN MEMORY");
                        pending_reads_queue[req.addr] = {};  // Create queue entry for this address
                        bus->read(req);  // Fetch from main memory
                    }

                    pending_reads_queue[req.addr].back().push_back(req);
                }           
            } else if (req.request_type == RequestResponse::WRITE) {
                if (req.cache_hit) {
                    log(name(), "WRITE HIT on tag", req.tag, "in set", req.set_index);

                    bus->write_invalidate(req);

                    responseQueue.push_back(req);
                } else {
                    log(name(), "WRITE MISS requiring WRITE ALLOCATE on tag", req.tag, "in set", req.set_index);

                    if (pending_reads_queue.find(req.addr) == pending_reads_queue.end()) {
                        pending_writes_queue[req.addr].push_back(req);
                        bus->read(req);  // Read data from memory first (write-allocate)
                    } else {
                        pending_writes_queue[req.addr].push_back(req);
                    }
                }
                
                
            } else if (req.request_type == RequestResponse::INVALIDATE_SELF) {
                log(name(), "INVALIDATING SELF on tag", req.tag, "in set", req.set_index);
                cache[req.set_index].lines[req.cache_hit_index].valid = false;
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

            res = initialize_request_response(res.addr, res.request_type);

            uint64_t data = 128; // Placeholder data

            if(res.request_type == RequestResponse::READ) {
                log(name(), "READ RESPONSE on tag", res.tag, "in set", res.set_index);

                for (RequestResponse req : pending_reads_queue[res.addr].front()) {
                    log(name(), "PENDING READS Processing for tag", req.tag, "in set", req.set_index);
                    stats_readhit(id);
                }

                pending_reads_queue[res.addr].pop_front();
                
                if (!pending_writes_queue[res.addr].empty()) {
                    log(name(), "PENDING WRITE Processing for tag", res.tag, "in set", res.set_index);
                    RequestResponse req = pending_writes_queue[res.addr].front();
                    pending_writes_queue[res.addr].pop_front();

                    requestQueue.push_back(req);
                }

            } else if (res.request_type == RequestResponse::WRITE) {
                log(name(), "WRITE RESPONSE on tag", res.tag, "in set", res.set_index);
                set_cache_line(res.set_index, res.cache_hit_index, res.tag, data, res.byte_in_line, true, true);
                

                
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