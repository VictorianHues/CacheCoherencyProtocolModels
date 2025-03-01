#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include <vector>
#include <queue>

#include "bus_slave_if.h"
#include "Cache.h"

struct BusRequest {
    Cache* source_cache;
    uint64_t addr;
    bool is_write;
};


class Bus : public bus_slave_if, public sc_module {
    public:
        sc_in<bool> clk;
        sc_port<bus_slave_if> memory;

        std::queue<BusRequest> requestQueue;
    
        SC_CTOR(Bus) {
            SC_THREAD(processRequestQueue);
            sensitive << clk.pos();
            dont_initialize();
        }
    
        int read(uint64_t addr, Cache* requester) {
            log(name(), "read pushed to queue for address", addr);
            BusRequest req = {requester, addr, false};
            requestQueue.push(req);
            return 0;
        }
    
        int write(uint64_t addr, Cache* requester) {
            log(name(), "write pushed to queue for address", addr);
            BusRequest req = {requester, addr, true};
            requestQueue.push(req);
            return 0;
        }
    
    private:
        void processRequestQueue() {
            while(true) {
                if (!requestQueue.empty()) {
                    BusRequest req = requestQueue.front();
                    requestQueue.pop();

                    log(name(), "processing request for address", req.addr);
                
                    assert((req.addr & 0x3) == 0);

                    if (req.is_write) {
                        memory->write(req.addr, req.source_cache);
                    } else {
                        memory->read(req.addr, req.source_cache);
                    }
                    
                    req.source_cache->notify_response();
                }
                wait();
            }
        }

    };

#endif
