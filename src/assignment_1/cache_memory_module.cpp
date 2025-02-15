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

void CacheMemory::read_from_main_memory(uint64_t addr, uint64_t &data) {
    wait(MEM_LATENCY);

    Port_MemFunc.write(MainMemory::FUNC_READ_MEM); // Read from Main Memory
    //cout << sc_time_stamp() << ": PORT - Main Memory Function Sent" << endl;

    Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
    //cout << sc_time_stamp() << ": PORT - Main Memory Address Sent" << endl;

    //cout << sc_time_stamp() << ": PORT - Waiting for Main Memory Done Acknowledgement..." << endl;
    wait(Port_MemDone.value_changed_event());
    //cout << sc_time_stamp() << ": PORT - Main Memory Done Acknowledgement Received" << endl;

    //cout << sc_time_stamp() << ": PORT - Reading data from Main Memory..." << endl;
    data = Port_MemData.read().to_uint64();
    //cout << sc_time_stamp() << ": PORT - Data read from Main Memory: " << data << endl;
}

void CacheMemory::write_to_main_memory(uint64_t addr, uint64_t &data) {
    wait(MEM_LATENCY);

    //cout << sc_time_stamp() << ": PORT - Write-back to Main Memory..." << endl;
    Port_MemFunc.write(MainMemory::FUNC_WRITE_MEM); // Write to Main Memory
    //cout << sc_time_stamp() << ": PORT - Main Memory Function Sent" << endl;

    Port_MemAddr.write(addr); // Send address to Main Memory. Using CPU address in as placeholder
    //cout << sc_time_stamp() << ": PORT - Main Memory Address Sent" << endl;

    //cout << sc_time_stamp() << ": PORT - Writing data to Main Memory..." << endl;
    Port_MemData.write(data); // Write data to Main Memory
    //cout << sc_time_stamp() << ": PORT - Data written to Main Memory" << endl;

    //cout << sc_time_stamp() << ": PORT - Floating data wires..." << endl;
    Port_MemData.write(float_64_bit_wire); // Float data wires
    //cout << sc_time_stamp() << ": PORT - Data wires floated" << endl;

    //cout << sc_time_stamp() << ": PORT - Waiting for Main Memory Done Acknowledgement..." << endl;
    wait(Port_MemDone.value_changed_event());
    //cout << sc_time_stamp() << ": PORT - Main Memory Done Acknowledgement Received" << endl;
}

// Thread execution function
void CacheMemory::execute() {
    while (true) {
        cout << sc_time_stamp() << ": CACHE WAITING FOR NEXT MEMORY REQUEST..." << endl;
        wait(Port_Func.value_changed_event());
        cout << sc_time_stamp() << ": Cache Received request from CPU!" << endl;


        uint64_t addr = Port_Addr.read(); // Read the address
        Function f = Port_Func.read(); // Read the function

        uint64_t tag = addr / (LINE_SIZE * NUM_SETS); // address divided by num of sets
        uint64_t set_addr = (addr / LINE_SIZE) % NUM_SETS; // address divided by line size modulo the number of sets
        uint64_t byte_in_line = addr % LINE_SIZE; // address modulo the line size

        uint64_t data = 0; // Initialize data for read or write

        bool cache_hit = false;
        size_t cache_hit_index = -1;

        cout << sc_time_stamp() << ": CACHE Memory received " << (f == FUNC_READ ? "read" : "write") << " request for address " << addr << endl;
        cout << sc_time_stamp() << ": CACHE Line, Tag: " << tag << " Set Address: " << set_addr << " Byte in Line: " << byte_in_line << endl;
        cout << sc_time_stamp() << ": CACHE Initial LRU Queue: " << cache[set_addr].lru[0] << " " << cache[set_addr].lru[1] << " " << cache[set_addr].lru[2] << " " << cache[set_addr].lru[3] << " " << cache[set_addr].lru[4] << " " << cache[set_addr].lru[5] << " " << cache[set_addr].lru[6] << " " << cache[set_addr].lru[7] << endl;

        /* Check if the address is in the cache for Cache Hit */
        for (size_t i = 0 ; i < SET_ASSOCIATIVITY ; i++) { // Iterate over lines in set
            if (cache[set_addr].lines[i].tag == tag) {
                cache_hit = true; // Cache hit
                cache_hit_index = i; // Cache hit index

                break;
            }
        }

        if (cache_hit) {
            cout << sc_time_stamp() << ": CACHE HIT, reading data from cache" << endl;
        } else {
            cout << sc_time_stamp() << ": CACHE MISS, finding least recently used line" << endl;
        }

        if (f == FUNC_READ) {
            cout << sc_time_stamp() << ": CACHE received read at address " << addr << endl;

            bool cache_line_valid = cache[set_addr].lines[cache_hit_index].valid;
            data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];
            wait(SC_ZERO_TIME); // Single-cycle Cache Latency for Cache Line Read

            if (cache_hit) {
                if (!cache_line_valid) {
                    cout << sc_time_stamp() << ": CACHE Data INVALID for read, fetching from main memory" << endl;

                    read_from_main_memory(addr, data);              

                    wait(SC_ZERO_TIME); // Single-cycle Cache Latency for Cache Write
                    cache[set_addr].lines[cache_hit_index].tag = tag; // Set tag
                    cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit
                    cache[set_addr].lines[cache_hit_index].dirty = false; // Set dirty bit
                    cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data
                } else {
                    cout << sc_time_stamp() << ": CACHE Data VALID for read, fetching from cache" << endl;
                }
                stats_readhit(0);

            } else {
                cout << sc_time_stamp() << ": CACHE Fetching from main memory" << endl;

                read_from_main_memory(addr, data);      
                
                cout << sc_time_stamp() << ": CACHE searching for Least Recently Used line" << endl;

                cache_hit_index = find_lru(cache[set_addr]);

                cout << sc_time_stamp() << ": CACHE Line " << cache_hit_index << " in CACHE Set " << set_addr << " evicted" << endl;


                /* If evicted line is dirty, write to Main Memory */
                if (cache[set_addr].lines[cache_hit_index].dirty) {
                    cout << sc_time_stamp() << ": CACHE Write-back evicted to Main Memory" << endl;

                    uint64_t evicted_data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                    cout << sc_time_stamp() << ": CACHE Evicted data: " << evicted_data << endl;

                    write_to_main_memory(addr, evicted_data);

                    cout << sc_time_stamp() << ": CACHE Write-back evicted Complete" << endl;

                } /* If not dirty, replace evicted without consequence */

                wait(SC_ZERO_TIME);
                cache[set_addr].lines[cache_hit_index].tag = tag; // Set tag
                cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit
                cache[set_addr].lines[cache_hit_index].dirty = false; // Set dirty bit
                cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data
            
                stats_readmiss(0);
            }

            //cout << sc_time_stamp() << ": PORT - Data being sent to CPU ..." << endl;
            Port_Data.write(data); // Returns requested data to CPU
            //cout << sc_time_stamp() << ": PORT - Data sent to CPU" << endl;

            //cout << sc_time_stamp() << ": PORT - Read Done Acknowledgement Sent..." << endl;
            Port_Done.write(RET_READ_DONE); // Returns read done to CPU
            wait(CACHE_CYCLE_LATENCY);
            //cout << sc_time_stamp() << ": PORT - Read Done Acknowledgement Complete" << endl;
            Port_Data.write(float_64_bit_wire);
            Port_Done.write(RetCode());
            

        } else if (f == FUNC_WRITE) {
            cout << sc_time_stamp() << ": CACHE received write at address " << addr << endl;

            data = Port_Data.read().to_uint64();

            cout << sc_time_stamp() << ": CACHE receives data from CPU: " << data << endl;

            if (!cache_hit) {
                read_from_main_memory(addr, data); // Simulate reading a Cache Line from Main Memory

                cout << sc_time_stamp() << ": CACHE searching for Least Recently Used line" << endl;

                cache_hit_index = find_lru(cache[set_addr]);

                cout << sc_time_stamp() << ": CACHE Line " << cache_hit_index << " in CACHE Set " << set_addr << " evicted" << endl;

                /* If evicted line is dirty, write to Main Memory*/
                if (cache[set_addr].lines[cache_hit_index].dirty) {
                    cout << sc_time_stamp() << ": CACHE Write-back evicted to Main Memory" << endl;

                    uint64_t evicted_data = cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)];

                    cout << sc_time_stamp() << ": CACHE Evicted data: " << evicted_data << endl;

                    write_to_main_memory(addr, evicted_data);

                    cout << sc_time_stamp() << ": CACHE Write-back evicted Complete" << endl;
                } /* If not dirty, replace evicted without consequence */
            
                stats_writemiss(0);
            } else {
                stats_writehit(0);
            }

            wait(SC_ZERO_TIME); // Single-cycle Cache Latency for Cache Write
            cache[set_addr].lines[cache_hit_index].tag = tag; // Set tag
            cache[set_addr].lines[cache_hit_index].valid = true; // Set valid bit
            cache[set_addr].lines[cache_hit_index].dirty = true; // Set dirty bit
            cache[set_addr].lines[cache_hit_index].data[byte_in_line / sizeof(uint64_t)] = data; // Set data
            
            cout << sc_time_stamp() << ": CACHE writes data to CACHE Line: " << data << endl;

            //cout << sc_time_stamp() << ": PORT - Write Done Acknowledgement Sent..." << endl;
            Port_Done.write(RET_WRITE_DONE); // Returns read done to CPU
            wait(CACHE_CYCLE_LATENCY);
            //cout << sc_time_stamp() << ": PORT - Write Done Acknowledgement Complete" << endl;
            Port_Done.write(RetCode());
        }

        // Update LRU Queue
        update_lru(cache[set_addr], cache_hit_index);

        cout << sc_time_stamp() << ": Updated LRU Queue End: " << cache[set_addr].lru[0] << " " << cache[set_addr].lru[1] << " " << cache[set_addr].lru[2] << " " << cache[set_addr].lru[3] << " " << cache[set_addr].lru[4] << " " << cache[set_addr].lru[5] << " " << cache[set_addr].lru[6] << " " << cache[set_addr].lru[7] << endl;

        cout << sc_time_stamp() << ": CACHE MODULE DONE" << endl;
        cout << endl;
    }
}