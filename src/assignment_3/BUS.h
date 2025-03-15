#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_if.h"
#include "memory_if.h"
#include "CACHE.h"
#include "psa.h"
#include "constants.h"

/**
 * Bus Module
 * 
 * The Bus is the central communication hub for the Cache Coherence Protocol.
 * It arbitrates requests from the Caches and Memory, and processes them in order.
 * 
 */
class Bus : public bus_if, public sc_module {
    public:
        /* Request and Response Types */
        struct RequestType {
            static const uint64_t READ = 0;
            static const uint64_t WRITE_TO_MAIN_MEM = 1;
            static const uint64_t INVALIDATE = 2;
            static const uint64_t SNOOP_READ_RESPONSE = 3;
            static const uint64_t READ_WRITE_ALLOCATE = 4;
        };
        struct ResponseType {
            static const uint64_t SNOOP_READ_RESPONSE_MEM = 1;
            static const uint64_t SNOOP_READ_RESPONSE_CACHE = 2;
            static const uint64_t READ_WRITE_ALLOCATE_RESPONSE = 3;
            static const uint64_t WRITE_TO_MAIN_MEM_RESPONSE = 4;
        };

        sc_in<bool> clk; // Clock
        sc_port<memory_if> memory; // Memory Port

        std::vector<Cache*> cache_list; // List of Caches connected to the Bus

        std::deque<std::vector<uint64_t>> requestQueue; // Request Queue
        std::deque<std::vector<uint64_t>> responseQueue; // Response Queue
    
        /* Initialize Threads */
        SC_CTOR(Bus) {
            SC_THREAD(bus_arbitration_thread);
            sensitive << clk.neg();

            SC_THREAD(processRequestQueue);
            sensitive << clk.neg();
            //dont_initialize();

            SC_THREAD(processResponsesQueue);
            sensitive << clk.neg();
            //dont_initialize();
        }

        /* HELPERS */
        /**
         * Checks if the Bus or the Memory are still processing requests.
         * 
         * @return bool True if the Bus or Memory are still processing requests, False otherwise.
         */
        bool system_busy() {
            return !requestQueue.empty() || !responseQueue.empty() || memory->system_busy();
        }

        /**
         * Adds a Cache to the Bus list of Caches.
         * 
         * @param new_cache The Cache to add to the Bus.
         */
        void add_cache(Cache* new_cache) {
            cache_list.push_back(new_cache);
}
    
        /* REQUESTS TO BUS */
        void read(uint64_t requester_id, uint64_t addr);
        void write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data);
        void read_for_write_allocate(uint64_t requester_id, uint64_t addr);

        void broadcast_invalidate(uint64_t requester_id, uint64_t addr);

        /* RESPONSES FROM MODULES */
        void mem_read_write_allocate_complete(uint64_t requester_id, uint64_t addr, uint64_t data);
        void mem_read_failed_snoop_complete(uint64_t requester_id, uint64_t addr, uint64_t data);
        void mem_write_to_main_memory_complete(uint64_t requester_id, uint64_t addr);

        void cache_snoop_read_response(uint64_t requester_id, uint64_t addr, uint64_t data);
        void cache_snoop_read_allocate_response(uint64_t requester_id, uint64_t addr, uint64_t data);

        /* BUS ARBITRATION */
        void memory_notify_bus_arbitration();
        void cache_notify_bus_arbitration(uint64_t cache_id);

    private:
        /* BUS ARBITRATION */
        int last_served_cache_id = 0;
        bool memory_waiting = false;
        std::deque<uint64_t> cache_arbitration;
        void bus_arbitration_thread();

        /* REQUESTS and RESPONSES THREADS */
        void processRequestQueue();
        void processResponsesQueue();
    };

#endif
