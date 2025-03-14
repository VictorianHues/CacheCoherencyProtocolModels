#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <systemc.h>

#include "cache_if.h"
#include "cpu_if.h"
#include "helpers.h"
#include "psa.h"

class CPU : public cpu_if, public sc_module {
    public:
        sc_in_clk clk;
        sc_port<cache_if> cache;
        
        sc_event response_event;

        CPU(sc_module_name name_, int id_) : sc_module(name_), id(id_) {
            SC_THREAD(execute);
            sensitive << clk.pos();
            log(name(), "constructed with id", id);
            dont_initialize(); // don't call execute to initialise it.
        }

        SC_HAS_PROCESS(CPU); // Needed because we didn't use SC_TOR

        void read_response(uint64_t addr, uint64_t data) {
            response_event.notify();
            log(name(), "READ RESPONSE on address", addr);
        }

        void write_response(uint64_t addr) {
            response_event.notify();
            log(name(), "WRITE RESPONSE on address", addr);
        }

        void wait_for_cache() {
            while (!response_event.triggered()) {
                wait(clk.posedge_event());
                //log(name(), "waiting for response...");
            }
            log(name(), "RESPONSE RECEIVED");
            response_event.cancel();
        }

    private:
        int id;

        void execute() {
            TraceFile::Entry tr_data;
            // Loop until end of tracefile
            while (!tracefile_ptr->eof()) {
                // Get the next action for the processor in the trace
                if (!tracefile_ptr->next(id, tr_data)) {
                    cerr << "Error reading trace for CPU" << endl;
                    break;
                }

                switch (tr_data.type) {
                    case TraceFile::ENTRY_TYPE_READ:
                        log(name(), "reading from address", tr_data.addr);
                        cache->cpu_read(tr_data.addr);
                        wait_for_cache();
                        break;
                    case TraceFile::ENTRY_TYPE_WRITE:
                        log(name(), "writing to address", tr_data.addr);
                        cache->cpu_write(tr_data.addr);
                        wait_for_cache();
                        break;
                    case TraceFile::ENTRY_TYPE_NOP:
                        //log(name(), "NOP");
                        //wait_for_cache();
                        break;
                    default:
                        cerr << "ERROR, got invalid data from Trace" << endl;
                        exit(0);
                }
                wait();
            }

            log(name(), "END OF TRACE");
            
            while (cache->system_busy()) {
                wait(1);
            }
            sc_stop();
        }
};

#endif
