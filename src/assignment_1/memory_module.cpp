#include <systemc.h>
#include <exception>
#include <iostream>

#include "memory_module.h"
#include "psa.h"

using namespace std;

extern const size_t MEM_SIZE;


// Destructor
Memory::~Memory() {
    delete[] m_data;
}

void dump() {
    for (size_t i = 0; i < MEM_SIZE; i++) {
        cout << setw(5) << i << ": " << setw(5) << m_data[i];
        if (i % 8 == 7) {
            cout << endl;
        }
    }
}

// Thread execution function
void Memory::execute() {
    while (true) {
        wait(Port_Func.value_changed_event());

        Function f = Port_Func.read();
        uint64_t addr = Port_Addr.read();
        uint64_t data = 0;
        if (f == FUNC_WRITE) {
            cout << sc_time_stamp() << ": MEM received write" << endl;
            data = Port_Data.read().to_uint64();
        } else {
            cout << sc_time_stamp() << ": MEM received read" << endl;
        }
        cout << sc_time_stamp() << ": MEM address " << addr << endl;

        // This simulates memory read/write delay
        wait(100);

        if (f == FUNC_READ) {
            Port_Data.write((addr < MEM_SIZE) ? m_data[addr] : 0);
            Port_Done.write(RET_READ_DONE);
            wait();
            Port_Data.write(float_64_bit_wire); // string with 64 "Z"'s
        } else {
            if (addr < MEM_SIZE) {
                m_data[addr] = data;
            }
            Port_Done.write(RET_WRITE_DONE);
        }
    }
}