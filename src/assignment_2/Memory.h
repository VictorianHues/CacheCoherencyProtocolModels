#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <systemc.h>
#include <queue>

#include "bus_slave_if.h"
#include "helpers.h"
#include "constants.h"
#include "psa.h"
#include "Cache.h"
#include "request_response_struct.h"



class Memory : public bus_slave_if, public sc_module {
    public:
        sc_in_clk clk;
        sc_port<bus_slave_if> bus;

        std::queue<std::pair<RequestResponse, int>> requestQueue;

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

        int read(RequestResponse req) {
            log(name(), "READ from MAIN MEMORY requested");
            std::pair<RequestResponse, int> req_with_mem_action = {req, 0};
            requestQueue.push(req_with_mem_action);
            read_count++;
            return 0;
        }

        int write(RequestResponse req, uint64_t data) {
            log(name(), "WRITE to MAIN MEMORY requested");
            std::pair<RequestResponse, int> req_with_mem_action = {req, 1};
            requestQueue.push(req_with_mem_action);
            write_count++;
            return 0;
        }

        int write_invalidate(RequestResponse req_res) {
            return 0;
        }

        int get_read_count() const {
            return read_count;
        }

        int get_write_count() const {
            return write_count;
        }

        void notify_response(RequestResponse req) {
            log(name(), "received response from BUS");
        }

    private:
        int read_count;
        int write_count;

        void processRequestQueue() {

            while (true) {
                if (!requestQueue.empty()) {
                    std::pair<RequestResponse, int> req_with_mem_action = requestQueue.front();
                    requestQueue.pop();

                    RequestResponse req = req_with_mem_action.first;
                    int action = req_with_mem_action.second;


                    wait(MEM_LATENCY);

                    if (action == 0) { 
                        log(name(), "READ from Main Memory COMPLETE");
                        bus->notify_response(req);

                    } else if (action == 1) { 
                        log(name(), "WRITE to Main Memory COMPLETE");
                        //bus->notify_response(req);
                    } 
                }
                wait();
            }
        }
};
#endif
