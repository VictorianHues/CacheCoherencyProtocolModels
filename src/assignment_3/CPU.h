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
        sc_in_clk clk; // Clock
        sc_port<cache_if> cache; // Cache Port
        
        sc_event response_event; // Event to notify CPU of Cache responses

        /* Constructor */
        CPU(sc_module_name name_, int id_) : sc_module(name_), id(id_) {
            SC_THREAD(execute);
            sensitive << clk.pos();
            log(name(), "constructed with id", id);
            dont_initialize(); // don't call execute to initialise it.
        }

        SC_HAS_PROCESS(CPU); // Needed because we didn't use SC_TOR

        /**
         * Notification from the Cache that a READ request has been completed.
         * 
         * @param addr The address of the Cache Line that was READ.
         * @param data The data that was READ from the Cache Line.
         */
        void read_response(uint64_t addr, uint64_t data) {
            log(name(), "READ RESPONSE on address", addr);
            //wait(clk.posedge_event());
            response_event.notify();
        }

        /**
         * Notification from the Cache that a WRITE request has been completed.
         * 
         * @param addr The address of the Cache Line that was WRITTEN.
         */
        void write_response(uint64_t addr) {
            log(name(), "WRITE RESPONSE on address", addr);
            //wait(clk.posedge_event());
            response_event.notify();
        }

        /**
         * Wait for the Cache to respond to the CPU request.
         */
        void wait_for_cache() {
            while (!response_event.triggered()) {
                wait(clk.posedge_event());
                //log(name(), "waiting for response...");
            }
            //log(name(), "response received");
            response_event.cancel();
        }

    private:
        int id; // ID of the CPU

        /**
         * Execute the CPU tracefile.
         */
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
