#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <systemc.h>
#include <deque>

#include "memory_if.h"
#include "helpers.h"
#include "constants.h"
#include "psa.h"
#include "CACHE.h"

class Memory : public memory_if, public sc_module {
    public:
        struct RequestType {
            static const uint64_t SNOOP_READ_RESPONSE = 0;
            static const uint64_t WRITE = 1;
            static const uint64_t READ_WRITE_ALLOCATE = 2;
        };

        sc_in_clk clk;
        sc_port<bus_if> bus;

        sc_event bus_arbitration;

        std::deque<std::vector<uint64_t>> requestQueue;
        std::deque<std::vector<uint64_t>> responseQueue;

        /* Constructor */
        SC_CTOR(Memory) : read_count(0), write_count(0) {
            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();
        }

        /* Destructor */
        ~Memory() {
            // nothing to do here right now.
        }

        /* System busy check */
        bool system_busy() {
            return !requestQueue.empty();
        }

        /**
         * Read request from Cache for READ MISSES.
         * Caches Snoop the Bus for READS with matching CACHE LINES.
         * If no Cache has a matching Cache Line, then read from Main Memory.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         */
        void read_failed_snoop(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ from MAIN MEMORY after failed SNOOP");

            std::vector<uint64_t> req = {requester_id, addr, RequestType::SNOOP_READ_RESPONSE};
            requestQueue.push_back(req);
        }

        /**
         * Read request from Cache for WRITE MISSES with WRITE ALLOCATE.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * 
         */
        void read_write_allocate(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ from MAIN MEMORY for WRITE ALLOCATE");

            std::vector<uint64_t> req = {requester_id, addr, RequestType::READ_WRITE_ALLOCATE};
            requestQueue.push_back(req);
        }

        /**
         * Write request from Cache for WRITES to MAIN MEMORY.
         * 
         * @param requester_id The ID of the Cache that requested the WRITE.
         * @param addr The address of the Cache Line to WRITE.
         * @param data The data to WRITE to Main Memory.
         */
        void write(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "WRITE to MAIN MEMORY requested");

            // No Literal Data is processed here, but it is passed in the request
            std::vector<uint64_t> req = {requester_id, addr, RequestType::WRITE};
            requestQueue.push_back(req);
        }

        /**
         * Get the number of READ requests processed by the Memory.
         */
        int get_read_count() const {
            return read_count;
        }

        /**
         * Get the number of WRITE requests processed by the Memory.
         */
        int get_write_count() const {
            return write_count;
        }

        /**
         * Notification from the Bus that it is available for communication.
         */
        void bus_arbitration_notification() {
            bus_arbitration.notify();
            //log(name(), "BUS ARBITRATION NOTIFICATION");
        }

        /**
         * Wait for the Bus to complete arbitration.
         */
        void wait_for_bus_arbitration() {
            bus->memory_notify_bus_arbitration();

            while (!bus_arbitration.triggered()) {
                wait(clk.negedge_event());
                //log(name(), "waiting for Bus arbitration...");
            }
            bus_arbitration.cancel();
        }

    private:
        int read_count;
        int write_count;

        /**
         * Process the Request Queue for the Main Memory as a SystemC Thread.
         */
        void processRequestQueue() {
            while (true) {
                if (!requestQueue.empty()) {
                    std::vector<uint64_t> req = requestQueue.front();
                    requestQueue.pop_front();

                    uint64_t requester_id = req[0];
                    uint64_t addr = req[1];
                    uint64_t req_type = req[2];
                    uint64_t data = 128; // Placeholder data

                    wait(MEM_LATENCY);

                    switch (req_type) {
                        case RequestType::SNOOP_READ_RESPONSE:
                            log(name(), "PROCESSING READ after FAILED SNOOP from Cache", requester_id, "for address", addr);
                            
                            wait_for_bus_arbitration();
                            bus->mem_read_failed_snoop_complete(requester_id, addr, data);

                            read_count++;
                            break;
                        case RequestType::WRITE:
                            log(name(), "PROCESSING WRITE from Cache", requester_id, "for address", addr);

                            wait_for_bus_arbitration();
                            bus->mem_write_to_main_memory_complete(requester_id, addr);
                            
                            write_count++;
                            break;
                        case RequestType::READ_WRITE_ALLOCATE:
                            log(name(), "PROCESSING READ for WRITE ALLOCATE from Cache", requester_id, "for address", addr);
                            
                            wait_for_bus_arbitration();
                            bus->mem_read_write_allocate_complete(requester_id, addr, data);
                            
                            read_count++;
                            break;
                    }
                }
                wait();
            }
        }
};
#endif
