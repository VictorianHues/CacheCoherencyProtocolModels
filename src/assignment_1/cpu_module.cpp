#include <iostream>
#include <iomanip>
#include <systemc>
#define SC_ALLOW_DEPRECATED_IEEE_API

#include "cpu_module.h"
#include "cache_memory_module.h"
#include "psa.h"
#include "constants.h"

using namespace std;
using namespace sc_core; // This pollutes namespace, better: only import what you need.

void CPU::execute() {
    TraceFile::Entry tr_data;
    CacheMemory::Function f;

    // Loop until end of tracefile
    while (!tracefile_ptr->eof()) {
        cout << sc_time_stamp() << ": CPU CYCLE START" << endl;
        // Get the next action for the processor in the trace
        if (!tracefile_ptr->next(0, tr_data)) {
            cerr << "Error reading trace for CPU" << endl;
            break;
        }

        switch (tr_data.type) {
        case TraceFile::ENTRY_TYPE_READ:
            f = CacheMemory::FUNC_READ;
            break;

        case TraceFile::ENTRY_TYPE_WRITE:
            f = CacheMemory::FUNC_WRITE;
            break;

        case TraceFile::ENTRY_TYPE_NOP: break;

        default:
            cerr << "Error, got invalid data from Trace" << endl;
            exit(0);
        }

        if (tr_data.type != TraceFile::ENTRY_TYPE_NOP) {
            Port_CpuAddr.write(tr_data.addr);
            Port_CpuFunc.write(f);

            if (f == CacheMemory::FUNC_WRITE) {
                cout << sc_time_stamp() << ": CPU sends write" << endl;

                // Don't have data, we write the address as the data value.
                cout << sc_time_stamp() << ": CPU writes address as placeholder: " << tr_data.addr << endl;
                Port_CpuData.write(tr_data.addr);
                wait(CACHE_CYCLE_LATENCY);
                cout << sc_time_stamp() << ": CPU resumed execution after wait(). Now floating data wires." << endl;
                
                // Now float the data wires with 64 "Z"'s
                Port_CpuData.write(float_64_bit_wire);

            } else {
                cout << sc_time_stamp() << ": CPU sends read" << endl;
            }
            
            cout << sc_time_stamp() << ": CPU waits for memory response" << endl;
            wait(Port_CpuDone.value_changed_event());
            cout << sc_time_stamp() << ": CPU resumed execution after wait()." << endl;
            
            wait(CACHE_CYCLE_LATENCY);

            if (f == CacheMemory::FUNC_READ) {
                cout << sc_time_stamp()
                        << ": CPU reads: " << Port_CpuData.read() << endl;
            }
        } else {
            cout << sc_time_stamp() << ": CPU executes NOP" << endl;
        }
        // Advance one cycle in simulated time
        wait();
    }

    // Finished the Tracefile, now stop the simulation
    sc_stop();

}