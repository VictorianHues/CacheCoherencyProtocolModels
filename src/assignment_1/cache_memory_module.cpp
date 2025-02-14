#include <systemc.h>
#include <exception>
#include <iostream>

#include "cache_memory_module.h"
#include "cpu_module.h"
#include "psa.h"

using namespace std;
using namespace sc_core;

void CacheMemory::dump() {
    for (size_t i = 0; i < NUM_SETS; i++) { // Iterate over all sets
        cout << "Set " << setw(3) << i << ": "; // Set number
        for (size_t j = 0; j < SET_ASSOCIATIVITY; j++) { // Iterate over all lines in set
            const CacheLine &line = cache[i].lines[j]; // Get line
            cout << "Tag: " << hex << setw(5) << line.tag
                 << " , Valid: " << line.valid
                 << " , Dirty: " << line.dirty;
        }
        cout << endl;
    }
}

void CacheMemory::update_lru(CacheSet &cache_set, size_t index) {
    size_t current = cache_set.lru[index];

    for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) {
        if (i != index && cache_set.lru[i] < current) {
            cache_set.lru[i]++;
        }
    }
    cache_set.lru[index] = 0;
}

size_t CacheMemory::find_lru(CacheSet &cache_set) {
    size_t max_index = 0;

    for (size_t i = 1; i < SET_ASSOCIATIVITY; i++) {
        if (cache_set.lru[i] > cache_set.lru[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}


// Thread execution function
void CacheMemory::execute() {
    while (true) {
        cout << sc_time_stamp() << ": CACHE WAITING FOR NEXT MEMORY REQUEST..." << endl;
        wait(Port_Func.value_changed_event());
        cout << sc_time_stamp() << ": Cache Received memory request!" << endl;


        uint64_t addr = Port_Addr.read(); // Read the address
        Function f = Port_Func.read(); // Read the function

        uint64_t tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
        uint64_t set_addr = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
        uint64_t byte_in_line = addr % LINE_SIZE; // address modulo the line size

        bool cache_hit = false;
        size_t cache_hit_index = -1;

        cout << sc_time_stamp() << ": Cache Memory received " << (f == FUNC_READ ? "read" : "write") << " request for address " << addr << endl;
        cout << sc_time_stamp() << ": Tag: " << tag << " Set: " << set_addr << " Byte in Line: " << byte_in_line << endl;
        cout << sc_time_stamp() << ": Current LRU Queue: " << cache[set_addr].lru[0] << " " << cache[set_addr].lru[1] << " " << cache[set_addr].lru[2] << " " << cache[set_addr].lru[3] << " " << cache[set_addr].lru[4] << " " << cache[set_addr].lru[5] << " " << cache[set_addr].lru[6] << " " << cache[set_addr].lru[7] << endl;

        /* Check if the address is in the cache for Cache Hit */
        for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) { // Iterate over lines in set
            if (cache[set_addr].lines[i].tag == tag // If the tag matches the set's tag
                && cache[set_addr].lines[i].valid) {  // If the line is valid 
                cache_hit = true; // Cache hit
                cache_hit_index = i; // Cache hit index

                cout << sc_time_stamp() << ": CACHE HIT at index " << cache_hit_index << endl;

                break;
            }
        }

        uint64_t data = 0;


        /* Cache Hit */
        if (cache_hit) {
            data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];
            bool cache_line_valid = cache[set_addr].lines[cache_hit_index].valid;
            
            /* Memory Access */
            if (f == FUNC_READ) {
                cout << sc_time_stamp() << ": Cache received read at address " << addr << endl;

                wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency for Cache Read

                /* If Cache Line is Invalid, retrieve data from Main Memory*/
                if (!cache_line_valid) {
                    cout << sc_time_stamp() << ": Cache Data not valid for read, fetching from main memory" << endl;

                    Port_MemFunc.write(MainMemory::FUNC_READ_MEM); // Read from Main Memory
                    cout << sc_time_stamp() << ": Main Memory Function Sent" << endl;

                    Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
                    cout << sc_time_stamp() << ": Main Memory Address Sent" << endl;

                    cout << sc_time_stamp() << ": Waiting for Main Memory Done Acknowledgement..." << endl;
                    wait(Port_MemDone.value_changed_event());
                    cout << sc_time_stamp() << ": Main Memory Done Acknowledgement Received" << endl;

                    cout << sc_time_stamp() << ": Reading data from Main Memory..." << endl;
                    data = Port_MemData.read().to_uint64();
                    cout << sc_time_stamp() << ": Data read from Main Memory: " << data << endl;                

                    wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency for Cache Write
                    cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data;
                    cache[set_addr].lines[cache_hit_index].valid = true;
                    cache[set_addr].lines[cache_hit_index].dirty = false;
                } /* Otherwise, retrieve data from cache */

                cout << sc_time_stamp() << ": Data being sent to CPU ..." << endl;
                Port_Data.write(data); // Returns requested data to CPU
                cout << sc_time_stamp() << ": Data sent to CPU" << endl;

                cout << sc_time_stamp() << ": Read Done Acknowledgement Sent..." << endl;
                Port_Done.write(RET_READ_DONE); // Returns read done to CPU
                wait(SC_ZERO_TIME);
                cout << sc_time_stamp() << ": Read Done Acknowledgement Complete" << endl;
                Port_Done.write(RetCode());
            }
            else if (f == FUNC_WRITE) {
                cout << sc_time_stamp() << ": Cache received write at address " << addr << endl;

                data = Port_Data.read().to_uint64();
                wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency for Cache Write
                
                /* If the tags match to get a cache hit, then simply replace the data in the cache line */
                cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Write data to cache
                cache[set_addr].lines[cache_hit_index].dirty = true; // Set dirty bit
                cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit

                cout << sc_time_stamp() << ": Data written to cache: " << data << endl;

                cout << sc_time_stamp() << ": Write Done Acknowledgement Sent..." << endl;
                Port_Done.write(RET_WRITE_DONE); // Returns read done to CPU
                wait(SC_ZERO_TIME);
                cout << sc_time_stamp() << ": Write Done Acknowledgement Complete" << endl;
                Port_Done.write(RetCode());
            }
        } else if (!cache_hit) {
            cout << sc_time_stamp() << ": CACHE MISS, finding least recently used line" << endl;

            cache_hit_index = find_lru(cache[set_addr]);

            cout << sc_time_stamp() << ": Evict line " << cache_hit_index << endl;

            if (f == FUNC_READ) {
                /* Data not in cache, so read from Main Memory */
                cout << sc_time_stamp() << ": Cache Fetching from main memory" << endl;

                Port_MemFunc.write(MainMemory::FUNC_READ_MEM); // Read from Main Memory
                cout << sc_time_stamp() << ": Main Memory Function Sent" << endl;

                Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
                cout << sc_time_stamp() << ": Main Memory Address Sent" << endl;

                cout << sc_time_stamp() << ": Waiting for Main Memory Done Acknowledgement..." << endl;
                wait(Port_MemDone.value_changed_event());
                cout << sc_time_stamp() << ": Main Memory Done Acknowledgement Received" << endl;

                cout << sc_time_stamp() << ": Reading data from Main Memory..." << endl;
                data = Port_MemData.read().to_uint64();
                cout << sc_time_stamp() << ": Data read from Main Memory: " << data << endl;                

                wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency to read Dirty Bit

                /* If evicted line is dirty, write to Main Memory*/
                if (cache[set_addr].lines[cache_hit_index].dirty) {
                    cout << sc_time_stamp() << ": Write-back" << endl;

                    uint64_t evicted_data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                    cout << sc_time_stamp() << ": Evicted data: " << evicted_data << endl;

                    cout << sc_time_stamp() << ": Write-back to Main Memory..." << endl;
                    Port_MemFunc.write(MainMemory::FUNC_WRITE_MEM); // Write to Main Memory
                    cout << sc_time_stamp() << ": Main Memory Function Sent" << endl;

                    Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
                    cout << sc_time_stamp() << ": Main Memory Address Sent" << endl;

                    cout << sc_time_stamp() << ": Writing data to Main Memory..." << endl;
                    Port_MemData.write(evicted_data); // Write data to Main Memory
                    cout << sc_time_stamp() << ": Data written to Main Memory" << endl;

                    cout << sc_time_stamp() << ": Floating data wires..." << endl;
                    Port_MemData.write(float_64_bit_wire); // Float data wires
                    cout << sc_time_stamp() << ": Data wires floated" << endl;

                    cout << sc_time_stamp() << ": Waiting for Main Memory Done Acknowledgement..." << endl;
                    wait(Port_MemDone.value_changed_event());
                    cout << sc_time_stamp() << ": Main Memory Done Acknowledgement Received" << endl;
                } /* If not dirty, replace evicted without consequence */
                
                wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency for Cache Write

                cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit
                cache[set_addr].lines[cache_hit_index].dirty = false; // Set dirty bit
                cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data

                cout << sc_time_stamp() << ": Data being sent to CPU ..." << endl;
                Port_Data.write(data); // Returns requested data to CPU
                cout << sc_time_stamp() << ": Data sent to CPU" << endl;

                cout << sc_time_stamp() << ": Read Done Acknowledgement Sent..." << endl;
                Port_Done.write(RET_READ_DONE); // Returns read done to CPU
                wait(SC_ZERO_TIME);
                cout << sc_time_stamp() << ": Read Done Acknowledgement Complete" << endl;
                Port_Done.write(RetCode());
            }
            else if (f == FUNC_WRITE) {
                cout << sc_time_stamp() << ": Cache received write at address " << addr << endl;

                data = Port_Data.read().to_uint64();

                /* If evicted line is dirty, write to Main Memory*/
                if (cache[set_addr].lines[cache_hit_index].dirty) {
                    cout << sc_time_stamp() << ": Write-back" << endl;

                    uint64_t evicted_data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                    cout << sc_time_stamp() << ": Evicted data: " << evicted_data << endl;

                    cout << sc_time_stamp() << ": Write-back to Main Memory..." << endl;
                    Port_MemFunc.write(MainMemory::FUNC_WRITE_MEM); // Write to Main Memory
                    cout << sc_time_stamp() << ": Main Memory Function Sent" << endl;

                    Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
                    cout << sc_time_stamp() << ": Main Memory Address Sent" << endl;

                    cout << sc_time_stamp() << ": Writing data to Main Memory..." << endl;
                    Port_MemData.write(evicted_data); // Write data to Main Memory
                    cout << sc_time_stamp() << ": Data written to Main Memory" << endl;

                    cout << sc_time_stamp() << ": Floating data wires..." << endl;
                    Port_MemData.write(float_64_bit_wire); // Float data wires
                    cout << sc_time_stamp() << ": Data wires floated" << endl;

                    cout << sc_time_stamp() << ": Waiting for Main Memory Done Acknowledgement..." << endl;
                    wait(Port_MemDone.value_changed_event());
                    cout << sc_time_stamp() << ": Main Memory Done Acknowledgement Received" << endl;
                } /* If not dirty, replace evicted without consequence */
                
                wait(CACHE_CYCLE_LATENCY); // Single-cycle Cache Latency for Cache Write

                cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit
                cache[set_addr].lines[cache_hit_index].dirty = true; // Set dirty bit
                cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data
                //memcpy(&cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)], &data, sizeof(data));

                cout << sc_time_stamp() << ": Data written to cache: " << data << endl;

                cout << sc_time_stamp() << ": Write Done Acknowledgement Sent..." << endl;
                Port_Done.write(RET_WRITE_DONE); // Returns read done to CPU
                wait(SC_ZERO_TIME);
                cout << sc_time_stamp() << ": Write Done Acknowledgement Complete" << endl;
                Port_Done.write(RetCode());
            }            
        }

        // Update LRU Queue
        update_lru(cache[set_addr], cache_hit_index);

        cout << sc_time_stamp() << ": Updated LRU Queue End: " << cache[set_addr].lru[0] << " " << cache[set_addr].lru[1] << " " << cache[set_addr].lru[2] << " " << cache[set_addr].lru[3] << " " << cache[set_addr].lru[4] << " " << cache[set_addr].lru[5] << " " << cache[set_addr].lru[6] << " " << cache[set_addr].lru[7] << endl;

        cout << sc_time_stamp() << ": CACHE MODULE DONE" << endl;
        cout << endl;
    }
}