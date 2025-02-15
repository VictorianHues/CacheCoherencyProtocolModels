#include <systemc.h>
#include <exception>
#include <iostream>

#include "main_memory_module.h"
#include "constants.h"

using namespace std;

void MainMemory::execute ( ) {
    while ( true ) {
        cout << sc_time_stamp() << ": MEM WAITING FOR MAIN MEMORY REQUEST..." << endl;
        wait(Port_MemFunc.value_changed_event());
        cout << sc_time_stamp() << ": MEM Received main memory request!" << endl;


        FunctionMem f = Port_MemFunc.read() ;
        uint64_t addr = Port_MemAddr.read();
        uint64_t data = 101010101; // Initialize placeholder data to simulate

        cout << sc_time_stamp() << ": MEM Function: " << f << endl;

        if (f == FUNC_READ_MEM) { // If the function is a read enum code
            cout << sc_time_stamp() << ": MEM Data being sent to Cache ..." << endl;
            Port_MemData.write(data); // Returns requested data to Cache

            //wait(MEM_LATENCY); // Simulate memory read latency
            cout << sc_time_stamp() << ": MEM Data sent to Cache" << endl;

            cout << sc_time_stamp() << ": MEM Read Done Acknowledgement Sent..." << endl;
            Port_MemDone.write(RET_READ_DONE_MEM); // Returns read done to Cache
            wait(SC_ZERO_TIME);
            cout << sc_time_stamp() << ": MEM Read Done Acknowledgement Complete" << endl;
            Port_MemDone.write(RetCodeMem());

        } else {
            cout << sc_time_stamp() << ": MEM received write at address " << addr << endl;

            //wait(MEM_LATENCY); // Memory Latency for write
            data = Port_MemData.read().to_uint64();
            cout << sc_time_stamp() << ": MEM Data received: " << data << endl;

            cout << sc_time_stamp() << ": MEM  Write Done Acknowledgement Sent..." << endl;
            Port_MemDone.write(RET_WRITE_DONE_MEM); // Returns write done to Cache
            wait(SC_ZERO_TIME);
            cout << sc_time_stamp() << ": MEM Write Done Acknowledgement Complete" << endl;
        }
    }
}