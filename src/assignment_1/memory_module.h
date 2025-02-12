#ifndef MEMORY_MODULE_H
#define MEMORY_MODULE_H

#include <systemc.h>
#include <iostream>

#include "constants.h"

SC_MODULE(Memory) {
public:
    enum Function { FUNC_READ, FUNC_WRITE };

    enum RetCode { RET_READ_DONE, RET_WRITE_DONE };

    sc_in<bool> Port_CLK;
    sc_in<Function> Port_Func;
    sc_in<uint64_t> Port_Addr;
    sc_out<RetCode> Port_Done;
    sc_inout_rv<64> Port_Data;

    SC_CTOR(Memory) {
        SC_THREAD(execute);
        sensitive << Port_CLK.pos();
        dont_initialize();

        m_data = new uint64_t[MEM_SIZE];
    }

    ~Memory ();

    void dump();

private:
    uint64_t *m_data;

    void execute ();
};

#endif