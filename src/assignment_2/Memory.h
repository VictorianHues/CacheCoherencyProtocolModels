#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <systemc.h>
#include <deque>

#include "memory_if.h"
#include "helpers.h"
#include "constants.h"
#include "psa.h"
#include "Cache.h"



class Memory : public memory_if, public sc_module {
    public:
        struct RequestType {
            static const uint64_t SNOOP_READ_RESPONSE = 0;
            static const uint64_t WRITE = 1;
            static const uint64_t READ_WRITE_ALLOCATE = 2;
        };

        sc_in_clk clk;
        sc_port<bus_if> bus;

        std::deque<std::vector<uint64_t>> requestQueue;
        std::deque<std::vector<uint64_t>> responseQueue;

        SC_CTOR(Memory) : read_count(0), write_count(0) {
            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();
        }

        ~Memory() {
            // nothing to do here right now.
        }

        bool system_busy() {
            return !requestQueue.empty();
        }

        void read_failed_snoop(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ from MAIN MEMORY after failed SNOOP");

            std::vector<uint64_t> req = {requester_id, addr, RequestType::SNOOP_READ_RESPONSE};
            requestQueue.push_back(req);
        }

        void read_write_allocate(uint64_t requester_id, uint64_t addr) {
            log(name(), "READ from MAIN MEMORY for WRITE ALLOCATE");

            std::vector<uint64_t> req = {requester_id, addr, RequestType::READ_WRITE_ALLOCATE};
            requestQueue.push_back(req);
        }

        void write(uint64_t requester_id, uint64_t addr, uint64_t data) {
            log(name(), "WRITE to MAIN MEMORY requested");

            // No Literal Data is processed here, but it is passed in the request
            std::vector<uint64_t> req = {requester_id, addr, RequestType::WRITE};
            requestQueue.push_back(req);
        }

        int get_read_count() const {
            return read_count;
        }

        int get_write_count() const {
            return write_count;
        }

    private:
        int read_count;
        int write_count;

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
                            
                            bus->mem_read_failed_snoop_complete(requester_id, addr, data);

                            read_count++;
                            break;
                        case RequestType::WRITE:
                            log(name(), "PROCESSING WRITE from Cache", requester_id, "for address", addr);
                            
                            write_count++;
                            break;
                        case RequestType::READ_WRITE_ALLOCATE:
                            log(name(), "PROCESSING READ for WRITE ALLOCATE from Cache", requester_id, "for address", addr);
                            
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
