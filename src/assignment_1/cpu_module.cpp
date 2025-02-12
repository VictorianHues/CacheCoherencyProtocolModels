#include <iostream>
#include <iomanip>
#include <systemc>
#define SC_ALLOW_DEPRECATED_IEEE_API

#include "cpu_module.h"
#include "memory_module.h"
#include "psa.h"

using namespace std;
using namespace sc_core; // This pollutes namespace, better: only import what you need.

void CPU::execute() {
    TraceFile::Entry tr_data;
    Memory::Function f;

    // Loop until end of tracefile
    while (!tracefile_ptr->eof()) {
        // Get the next action for the processor in the trace
        if (!tracefile_ptr->next(0, tr_data)) {
            cerr << "Error reading trace for CPU" << endl;
            break;
        }

        // To demonstrate the statistic functions, we generate a 50%
        // probability of a 'hit' or 'miss', and call the statistic
        // functions below
        int j = rand() % 2;

        switch (tr_data.type) {
        case TraceFile::ENTRY_TYPE_READ:
            f = Memory::FUNC_READ;
            if (j)
                stats_readhit(0);
            else
                stats_readmiss(0);
            break;

        case TraceFile::ENTRY_TYPE_WRITE:
            f = Memory::FUNC_WRITE;
            if (j)
                stats_writehit(0);
            else
                stats_writemiss(0);
            break;

        case TraceFile::ENTRY_TYPE_NOP: break;

        default:
            cerr << "Error, got invalid data from Trace" << endl;
            exit(0);
        }

        if (tr_data.type != TraceFile::ENTRY_TYPE_NOP) {
            Port_MemAddr.write(tr_data.addr);
            Port_MemFunc.write(f);

            if (f == Memory::FUNC_WRITE) {
                cout << sc_time_stamp() << ": CPU sends write" << endl;

                // Don't have data, we write the address as the data value.
                Port_MemData.write(tr_data.addr);
                wait();
                // Now float the data wires with 64 "Z"'s
                Port_MemData.write(float_64_bit_wire);

            } else {
                cout << sc_time_stamp() << ": CPU sends read" << endl;
            }

            wait(Port_MemDone.value_changed_event());

            if (f == Memory::FUNC_READ) {
                cout << sc_time_stamp()
                        << ": CPU reads: " << Port_MemData.read() << endl;
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